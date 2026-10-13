#!/usr/bin/env python3
"""Browser-based editor for placing trainers, item balls, hidden items and
field-move obstacles (Rock Smash rocks, Cut trees, Strength boulders) on maps.

Edits res/field/events/*.json in place (the same files the build reads), so
changes show up as ordinary git diffs. New/edited item balls also touch
res/field/scripts/scripts_visible_items.s, hidden items touch
include/data/field/hidden_items.h, and both may rename an unused flag in
generated/vars_flags.txt in place (never appended, for save compatibility).
Run from anywhere:

    python3 tools/trainer_placer/trainer_placer.py [--port 8137] [--host 127.0.0.1]

then open http://localhost:8137 in a browser.
"""

import argparse
import glob
import json
import os
import re
import struct
import subprocess
import sys
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import parse_qs, urlparse

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
HERE = os.path.dirname(os.path.abspath(__file__))
EVENTS_DIR = os.path.join(ROOT, "res", "field", "events")
SCRIPTS_DIR = os.path.join(ROOT, "res", "field", "scripts")
MATRIX_DIR = os.path.join(ROOT, "res", "field", "matrices")
LAND_DIR = os.path.join(ROOT, "res", "field", "maps", "data")
TRAINER_DIR = os.path.join(ROOT, "res", "trainers", "data")
VISIBLE_ITEMS_S = os.path.join(SCRIPTS_DIR, "scripts_visible_items.s")
HIDDEN_ITEMS_H = os.path.join(ROOT, "include", "data", "field", "hidden_items.h")
VARS_FLAGS_TXT = os.path.join(ROOT, "generated", "vars_flags.txt")
AREA_DIR = os.path.join(ROOT, "res", "field", "area_data")
PROPS_DIR = os.path.join(ROOT, "res", "field", "props")
PROP_ORDER = os.path.join(PROPS_DIR, "models", "map_prop_models.order")

CHUNK = 32
TERRAIN_OFFSET = 0x10
COLLISION_BIT = 0x8000

# SCRIPT_ID_OFFSET_VISIBLE_ITEMS / SCRIPT_ID_OFFSET_HIDDEN_ITEMS (include/script_manager.h)
VISIBLE_BASE, HIDDEN_BASE, HIDDEN_LIMIT = 7000, 8000, 8800
BG_EVENT_HIDDEN_ITEM = 2

# Map props (src/overlay005/map_prop.c MapPropFile): model id, position, rotation, scale (fx32), 2 unused words.
PROP_FMT = "<i3i3i3i2i"
PROP_SIZE = struct.calcsize(PROP_FMT)
MAX_PROPS_PER_CHUNK = 32          # MAX_LOADED_MAP_PROPS
FX_ONE = 4096
# One-tile ledge decals (from PDSMS) and the texture/palette they bind from the area's prop texture set.
# Piece keys are the end as seen facing the jump direction; PDSMS's _l/_r tiles are named the other way round.
# Each style binds one texture/palette from the area's prop texture set, copied in from `source` when first used.
LEDGE_STYLES = {
    "outdoor": {"models": {"b": "ledge_jump_b", "l": "ledge_jump_r", "r": "ledge_jump_l"},
                "texture": "allpeak", "palette": "apeak", "source": os.path.join("res", "field", "props", "models", "ledge_jump_b.nsbmd")},
    "cave": {"models": {"b": "ledge_jump_cave_b", "l": "ledge_jump_cave_r", "r": "ledge_jump_cave_l"},
             "texture": "dun_allpeak", "palette": "dun_apeak", "source": os.path.join("res", "field", "maps", "texture_sets", "map_texture_set_054.nsbtx")},
}
LEDGE_MODEL_STYLE = {m + ".nsbmd": style for style, s in LEDGE_STYLES.items() for m in s["models"].values()}


def read_lines(path):
    with open(path, encoding="utf-8") as f:
        return [l.strip() for l in f if l.strip()]


def const_names(name):
    return [l.split("=")[0].strip() for l in read_lines(os.path.join(ROOT, "generated", name))]


def parse_map_headers():
    src = open(os.path.join(ROOT, "include", "data", "map_headers.h"), encoding="utf-8").read()
    headers = {}
    for m in re.finditer(r"\[(MAP_HEADER_\w+)\]\s*=\s*\{(.*?)\n\s*\},", src, re.S):
        body = m.group(2)
        def field(key):
            fm = re.search(rf"\.{key}\s*=\s*(\w+)", body)
            return fm.group(1) if fm else None
        headers[m.group(1)] = {
            "matrix": field("mapMatrixID"),
            "events": field("eventsArchiveID"),
            "scripts": field("scriptsArchiveID"),
            "init_scripts": field("initScriptsArchiveID"),
            "area": field("areaDataArchiveID"),
        }
    return headers


def camel(s):
    return "".join(w.capitalize() for w in s.split("_"))


def parse_tile_behaviors():
    src = open(os.path.join(ROOT, "include", "constants", "field", "map_tile_behaviors.h"), encoding="utf-8").read()
    names, value = {}, 0
    for m in re.finditer(r"^\s*(TILE_BEHAVIOR_\w+)\s*(?:=\s*(0x[0-9A-Fa-f]+|\d+))?\s*,", src, re.M):
        if m.group(2) is not None:
            value = int(m.group(2), 0)
        names[value] = m.group(1)
        value += 1
    return [names.get(i, "") for i in range(256)]


def land_path(map_name):
    if map_name == "MAP_NONE":
        return None
    path = os.path.join(LAND_DIR, f"map_data_{int(map_name.split('_')[1]):03d}.bin")
    return path if os.path.exists(path) else None


def load_land(map_name):
    path = land_path(map_name)
    if not path:
        return None
    with open(path, "rb") as f:
        f.seek(TERRAIN_OFFSET)
        return struct.unpack(f"<{CHUNK * CHUNK}H", f.read(CHUNK * CHUNK * 2))


def split_land(data):
    """map_data_NNN.bin = 4 u32 section sizes, then terrain, props, NSBMD model, BDHC."""
    sizes = struct.unpack_from("<4I", data)
    out, off = [], 16
    for s in sizes:
        out.append(data[off:off + s])
        off += s
    return out


def join_land(sections):
    return struct.pack("<4I", *(len(s) for s in sections)) + b"".join(sections)


def parse_props(blob):
    return [list(struct.unpack_from(PROP_FMT, blob, i)) for i in range(0, len(blob) - PROP_SIZE + 1, PROP_SIZE)]


def bdhc_height(bdhc, x, z):
    """Floor height (fx32) under chunk-local point (x, z), using the same plate maths as CalculateObjectHeight."""
    if len(bdhc) < 16 or bdhc[:4] != b"BDHC":
        return 0
    npts, nnrm, ncon, nplt = struct.unpack_from("<4H", bdhc, 4)
    off = 16
    pts = [struct.unpack_from("<2i", bdhc, off + 8 * i) for i in range(npts)]; off += 8 * npts
    nrm = [struct.unpack_from("<3i", bdhc, off + 12 * i) for i in range(nnrm)]; off += 12 * nnrm
    con = [struct.unpack_from("<i", bdhc, off + 4 * i)[0] for i in range(ncon)]; off += 4 * ncon
    for i in range(nplt):
        p1, p2, n, c = struct.unpack_from("<4H", bdhc, off + 8 * i)
        (ax, az), (bx, bz) = pts[p1], pts[p2]
        if min(ax, bx) <= x <= max(ax, bx) and min(az, bz) <= z <= max(az, bz) and nrm[n][1]:
            nx, ny, nz = nrm[n]
            return int(-((nx * x + nz * z) / FX_ONE + con[c]) * FX_ONE / ny)
    return 0


def ledge_placement(local_tx, local_tz, rot_y):
    """Prop position (fx32) that puts the one-tile ledge quad (x 0..16, z -16..0) on the tile after rotation."""
    import math
    a = rot_y / 0x10000 * 2 * math.pi
    s, c = round(math.sin(a)), round(math.cos(a))
    corners = [(x * c + z * s, -x * s + z * c) for x, z in ((0, 0), (16, 0), (16, -16), (0, -16))]
    min_x, min_z = min(p[0] for p in corners), min(p[1] for p in corners)
    return (local_tx * 16 - 256 - min_x) * FX_ONE, (local_tz * 16 - 256 - min_z) * FX_ONE


def ledge_center(px, pz, rot_y):
    """Chunk-local fx32 centre of a placed ledge quad (local centre (8, -8) rotated about Y)."""
    import math
    a = rot_y / 0x10000 * 2 * math.pi
    s, c = round(math.sin(a)), round(math.cos(a))
    return px + (8 * c - 8 * s) * FX_ONE, pz + (-8 * s - 8 * c) * FX_ONE


# ---- NSBTX / TEX0 editing (layout from NitroSystem's res_struct.h: NNSG3dResTex) ----
# Bits per texel for texImageParam formats; format 5 (4x4 compressed) keeps its own blocks and isn't appendable.
TEX_BPP = {1: 8, 2: 2, 3: 4, 4: 8, 6: 8, 7: 16}


def find_tex0(d):
    nsec = struct.unpack_from("<H", d, 14)[0]
    return next(s for s in struct.unpack_from(f"<{nsec}I", d, 16) if d[s:s + 4] == b"TEX0")


def read_dict(d, off):
    """NNSG3dResDict -> [(name bytes(16), data bytes)] in entry order."""
    n = d[off + 1]
    e = off + struct.unpack_from("<H", d, off + 6)[0]
    unit, names_off = struct.unpack_from("<HH", d, e)
    return [(d[e + names_off + 16 * i:e + names_off + 16 * (i + 1)], d[e + 4 + unit * i:e + 4 + unit * (i + 1)]) for i in range(n)]


def name_bit(name, k):
    return (struct.unpack_from("<I", name, (k >> 5) * 4)[0] >> (k & 31)) & 1


def build_dict(entries, unit):
    """Serialise an NNSG3dResDict, including the PATRICIA tree NNS_G3dGetResDictIdxByName walks
    for dictionaries of 16+ entries (built Sedgewick-style: node 0 is the bit-127 head)."""
    nodes = [[127, 0, 0, 0]]                   # refBit, idxLeft, idxRight, idxEntry
    keys = [bytes(16)]

    def child(i, name):
        return nodes[i][2] if name_bit(name, nodes[i][0]) else nodes[i][1]

    for idx, (name, _) in enumerate(entries):
        p, x = 0, nodes[0][1]
        while nodes[p][0] > nodes[x][0]:
            p, x = x, child(x, name)
        found = keys[x]
        diff = next(k for k in range(127, -1, -1) if name_bit(name, k) != name_bit(found, k))
        p, x = 0, nodes[0][1]
        while nodes[p][0] > nodes[x][0] and nodes[x][0] > diff:
            p, x = x, child(x, name)
        t = len(nodes)
        nodes.append([diff, x, t, idx] if name_bit(name, diff) else [diff, t, x, idx])
        keys.append(name)
        if p == 0 or not name_bit(name, nodes[p][0]):
            nodes[p][1] = t
        else:
            nodes[p][2] = t
    n = len(entries)
    ofs_entry = 8 + 4 * (n + 1)
    body = b"".join(struct.pack("<4B", *nd) for nd in nodes)
    body += struct.pack("<HH", unit, 4 + unit * n) + b"".join(data for _, data in entries) + b"".join(name for name, _ in entries)
    return struct.pack("<BBHHH", 0, n, 8 + len(body), 8, ofs_entry) + body


def dict_lookup(d, off, name):
    """NNS_G3dGetResDictIdxByName, for verifying dictionaries we build."""
    n = d[off + 1]
    ent = read_dict(d, off)
    if n < 16:
        return next((i for i, (nm, _) in enumerate(ent) if nm == name), -1)
    node = lambda i: struct.unpack_from("<4B", d, off + 8 + 4 * i)
    p, x = node(0), node(0)[1]
    if p[1] == 0:
        return -1
    xn = node(x)
    while p[0] > xn[0]:
        p = xn
        x = xn[2] if name_bit(name, xn[0]) else xn[1]
        xn = node(x)
    return xn[3] if ent[xn[3]][0] == name else -1


def btx_contents(path_or_bytes):
    """(textures [(name, format, transparent)], palettes [name]) of an NSBTX/NSBMD's TEX0, in file order."""
    d = path_or_bytes if isinstance(path_or_bytes, bytes) else open(path_or_bytes, "rb").read()
    t = find_tex0(d)
    texs = []
    for name, data in read_dict(d, t + struct.unpack_from("<H", d, t + 0x0E)[0]):
        param = struct.unpack_from("<I", data)[0]
        texs.append((name.rstrip(b"\0").decode(), (param >> 26) & 7, bool(param >> 29 & 1)))
    pals = [name.rstrip(b"\0").decode() for name, _ in read_dict(d, t + struct.unpack_from("<H", d, t + 0x34)[0])]
    return texs, pals


def parse_tex0(d):
    t = find_tex0(d)
    u16 = lambda o: struct.unpack_from("<H", d, t + o)[0]
    u32 = lambda o: struct.unpack_from("<I", d, t + o)[0]
    tex = {"size": u16(0x0C) << 3, "ofs": u32(0x14)}
    t4 = {"size": u16(0x1C) << 3, "ofs": u32(0x24), "pidx": u32(0x28)}
    pal = {"size": u16(0x30) << 3, "flag": u16(0x32), "ofs": u32(0x38)}
    return {
        "texs": read_dict(d, t + u16(0x0E)), "pals": read_dict(d, t + u16(0x34)),
        "tex_flag": u16(0x10), "t4_flag": u16(0x20), "pal_flag": pal["flag"],
        "tex_data": d[t + tex["ofs"]:t + tex["ofs"] + tex["size"]],
        "t4_data": d[t + t4["ofs"]:t + t4["ofs"] + t4["size"]],
        "t4_pidx": d[t + t4["pidx"]:t + t4["pidx"] + t4["size"] // 2],
        "pal_data": d[t + pal["ofs"]:t + pal["ofs"] + pal["size"]],
    }


def texture_bytes(tex0, data):
    param = struct.unpack_from("<I", data)[0]
    fmt = (param >> 26) & 7
    if fmt not in TEX_BPP:
        raise ValueError("4x4-compressed textures can't be copied between texture sets.")
    w, h = 8 << ((param >> 20) & 7), 8 << ((param >> 23) & 7)
    off = (param & 0xFFFF) << 3
    return tex0["tex_data"][off:off + w * h * TEX_BPP[fmt] // 8]


def palette_bytes(tex0, name):
    offs = sorted((struct.unpack_from("<H", data)[0] << 3, nm) for nm, data in tex0["pals"])
    for i, (off, nm) in enumerate(offs):
        if nm == name:
            end = next((o for o, _ in offs[i + 1:] if o > off), len(tex0["pal_data"]))
            return tex0["pal_data"][off:end]
    raise KeyError(name)


def btx_append(target, source, only=None):
    """New NSBTX bytes: `target` (an NSBTX) plus every texture/palette of `source` (an NSBTX or NSBMD)
    that target doesn't already have by name (or just those named in `only`, a set of str).
    Existing texture/palette bytes are kept verbatim."""
    a, b = parse_tex0(target), parse_tex0(source)
    texs, pals = list(a["texs"]), list(a["pals"])
    tex_data, pal_data = bytearray(a["tex_data"]), bytearray(a["pal_data"])
    have_t, have_p = {n for n, _ in texs}, {n for n, _ in pals}
    wanted = lambda name: only is None or name.rstrip(b"\0").decode() in only
    for name, data in b["texs"]:
        if name in have_t or not wanted(name):
            continue
        raw = texture_bytes(b, data)
        tex_data += bytes(-len(tex_data) % 8)
        param, extra = struct.unpack_from("<II", data)
        texs.append((name, struct.pack("<II", (param & ~0xFFFF) | (len(tex_data) >> 3), extra)))
        tex_data += raw
    for name, data in b["pals"]:
        if name in have_p or not wanted(name):
            continue
        raw = palette_bytes(b, name)
        pal_data += bytes(-len(pal_data) % 16)
        pals.append((name, struct.pack("<HH", len(pal_data) >> 3, struct.unpack_from("<H", data, 2)[0])))
        pal_data += raw
    tex_data += bytes(-len(tex_data) % 8)
    pal_data += bytes(-len(pal_data) % 8)

    tex_dict, pal_dict = build_dict(texs, 8), build_dict(pals, 4)
    o_tdict = 0x3C
    o_pdict = o_tdict + len(tex_dict)
    o_tex = o_pdict + len(pal_dict)
    o_t4 = o_tex + len(tex_data)
    o_pidx = o_t4 + len(a["t4_data"])
    o_pal = o_pidx + len(a["t4_pidx"])
    size = o_pal + len(pal_data)
    head = struct.pack("<4sI", b"TEX0", size)
    head += struct.pack("<IHHHHI", 0, len(tex_data) >> 3, o_tdict, a["tex_flag"], 0, o_tex)
    head += struct.pack("<IHHHHII", 0, len(a["t4_data"]) >> 3, o_tdict, a["t4_flag"], 0, o_t4, o_pidx)
    head += struct.pack("<IHHHHI", 0, len(pal_data) >> 3, a["pal_flag"] | b["pal_flag"], o_pdict, 0, o_pal)
    tex0 = head + tex_dict + pal_dict + bytes(tex_data) + a["t4_data"] + a["t4_pidx"] + bytes(pal_data)
    return struct.pack("<4sHHIHHI", b"BTX0", 0xFEFF, 1, 20 + len(tex0), 16, 1, 20) + tex0


def read_json_keep_tail(path):
    raw = open(path, encoding="utf-8").read()
    tail = raw[len(raw.rstrip()):]
    return json.loads(raw), tail


def write_json_keep_tail(path, data, tail):
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(json.dumps(data, indent=4, ensure_ascii=False) + (tail or "\n"))


def read_text(path):
    with open(path, encoding="utf-8", newline="") as f:
        return f.read()


def write_text(path, text):
    with open(path, "w", encoding="utf-8", newline="") as f:
        f.write(text)


def visible_block_span(src, label):
    m = re.search(rf"^{re.escape(label)}:\n(.*?)(?=^\S|\Z)", src, re.M | re.S)
    return m.span(1) if m else None


def parse_visible_items(src):
    """ScriptEntry order in scripts_visible_items.s; entry i is script VISIBLE_BASE + i."""
    entries = []
    for i, label in enumerate(re.findall(r"^\s*ScriptEntry (\w+)\s*$", src, re.M)):
        span = visible_block_span(src, label)
        block = src[span[0]:span[1]] if span else ""
        item = re.search(r"SetVar VAR_0x8008, (ITEM_\w+)", block)
        qty = re.search(r"SetVar VAR_0x8009, (\d+)", block)
        entries.append({
            "index": i, "label": label,
            "item": item.group(1) if item else None, "qty": int(qty.group(1)) if qty else None,
            "editable": bool(item and qty and "GoTo VisibleItems_TryGiveItem" in block),
        })
    return entries


HIDDEN_ENTRY_RE = r"HIDDEN_ITEM_ENTRY\(\s*(ITEM_\w+),\s*(\d+),\s*(\d+),\s*({flag})\s*\)"


def parse_flags(text):
    """Flag names in generated/vars_flags.txt order, plus the region bounds we care about."""
    lines = [l.strip() for l in text.splitlines() if l.strip()]
    names = [l.split("=")[0].strip() for l in lines]
    def pos(marker):
        return names.index(marker)
    hidden = [n for n in names[pos("HIDDEN_ITEM_FLAGS_START") + 1:pos("HIDDEN_ITEM_FLAGS_END")] if n.startswith("FLAG_")]
    # Persistent, non-hidden-item flags that an item ball may use (between the
    # map-local block and the trainer-defeated block).
    region = [n for n in names[pos("MAP_LOCAL_FLAGS_END") + 1:pos("TRAINER_DEFEATED_FLAGS_START")]
              if n.startswith("FLAG_") and n not in set(hidden)]
    return {"all": set(n for n in names if n.startswith("FLAG_")), "hidden": hidden, "item_region": region}


class Index:
    """Cross-map facts that need a full scan: where each trainer is placed and which ones scripts start."""

    def __init__(self):
        self.headers = parse_map_headers()
        self.behaviors = parse_tile_behaviors()
        self.gfx = const_names("object_events_gfx.txt")
        self.movement_types = const_names("movement_types.txt")
        self.trainer_types = const_names("trainer_types.txt")
        self.trainer_consts = [t for t in const_names("trainers.txt") if t != "TRAINER_NONE"]
        self.items = [i for i in const_names("items.txt") if i != "ITEM_NONE"]
        self.rescan()

    def rescan_items(self):
        self.visible = parse_visible_items(read_text(VISIBLE_ITEMS_S))
        self.flags = parse_flags(read_text(VARS_FLAGS_TXT))
        self.hidden = {}          # hidden flag index (script - HIDDEN_BASE) -> entry
        for m in re.finditer(HIDDEN_ENTRY_RE.format(flag=r"\w+"), read_text(HIDDEN_ITEMS_H)):
            if m.group(4) in self.flags["hidden"]:
                idx = self.flags["hidden"].index(m.group(4))
                self.hidden[idx] = {"index": idx, "item": m.group(1), "qty": int(m.group(2)),
                                    "range": int(m.group(3)), "flag": m.group(4)}
        # Every file that mentions each flag, so "unused" really means unused.
        out = subprocess.run(["git", "grep", "--untracked", "-ow", "-E", r"FLAG_[A-Za-z0-9_]+", "--",
                              ".", ":!generated/vars_flags.txt", ":!tools/trainer_placer"],
                             cwd=ROOT, capture_output=True, text=True).stdout
        self.flag_refs = {}
        for line in out.splitlines():
            path, _, flag = line.rpartition(":")
            self.flag_refs.setdefault(flag, set()).add(path)

    def rescan(self):
        self.rescan_items()
        self.vis_placed, self.hid_placed = {}, {}   # entry index -> [events file stem]
        self.placed = {}          # TRAINER_X -> [events file stem]
        class_gfx = {}            # trainer class -> {gfx: count}
        self.trainer_info = {}
        for t in self.trainer_consts:
            p = os.path.join(TRAINER_DIR, t[len("TRAINER_"):].lower() + ".json")
            info = {"name": "", "class": "", "party": [], "has_json": os.path.exists(p)}
            if info["has_json"]:
                j = json.load(open(p, encoding="utf-8"))
                info["name"] = j.get("name", "")
                info["class"] = j.get("class", "").replace("TRAINER_CLASS_", "")
                info["party"] = [f"Lv {m.get('level')} {m.get('species', '').replace('SPECIES_', '').title()}" for m in j.get("party", [])]
            self.trainer_info[t] = info
        for path in glob.glob(os.path.join(EVENTS_DIR, "*.json")):
            stem = os.path.splitext(os.path.basename(path))[0]
            ev = json.load(open(path, encoding="utf-8"))
            for b in ev.get("bg_events", []):
                s = b.get("script")
                if isinstance(s, int) and HIDDEN_BASE <= s < HIDDEN_LIMIT:
                    self.hid_placed.setdefault(s - HIDDEN_BASE, []).append(stem)
            for o in ev.get("object_events", []):
                s = o.get("script")
                if isinstance(s, int) and VISIBLE_BASE <= s < HIDDEN_BASE:
                    self.vis_placed.setdefault(s - VISIBLE_BASE, []).append(stem)
                if isinstance(s, str) and s.startswith("TRAINER_"):
                    self.placed.setdefault(s, []).append(stem)
                    cls = self.trainer_info.get(s, {}).get("class")
                    if cls:
                        d = class_gfx.setdefault(cls, {})
                        d[o["graphics_id"]] = d.get(o["graphics_id"], 0) + 1
        self.class_gfx = {c: max(d, key=d.get) for c, d in class_gfx.items()}
        self.script_trainers = set()
        for path in glob.glob(os.path.join(SCRIPTS_DIR, "*.s")):
            self.script_trainers.update(re.findall(r"\bTRAINER_[A-Z0-9_]+\b", open(path, encoding="utf-8", errors="replace").read()))

    def meta(self):
        trainers = []
        for t in self.trainer_consts:
            info = self.trainer_info[t]
            trainers.append({
                "const": t, **info,
                "placed_on": self.placed.get(t, []),
                "used_by_script": t in self.script_trainers,
                "default_gfx": self.class_gfx.get(info["class"]),
            })
        maps = sorted(
            [{"header": h, **v} for h, v in self.headers.items()
             if v["events"] and os.path.exists(os.path.join(EVENTS_DIR, v["events"] + ".json"))
             and v["matrix"] and os.path.exists(os.path.join(MATRIX_DIR, v["matrix"] + ".json"))],
            key=lambda m: m["header"])
        visible = [{**e, "placed_on": self.vis_placed.get(e["index"], [])} for e in self.visible]
        hidden = [{**e, "placed_on": self.hid_placed.get(i, [])} for i, e in sorted(self.hidden.items())]
        return {"maps": maps, "gfx": self.gfx, "movement_types": self.movement_types,
                "trainer_types": self.trainer_types, "trainers": trainers, "behaviors": self.behaviors,
                "items": self.items, "visible_items": visible, "hidden_items": hidden}

    def script_paths(self, header):
        h = self.headers[header]
        return [os.path.join(SCRIPTS_DIR, n + ".s") for n in (h["scripts"], h["init_scripts"]) if n]

    def prop_model_names(self):
        return [l.strip() for l in open(PROP_ORDER, encoding="utf-8") if l.strip()]

    def save_props(self, header, chunk_props, tiles):
        """Rewrite the props list and/or terrain words of this map's chunks.

        chunk_props: {"MAP_NNN": [[model, px, py, pz, rx, ry, rz, sx, sy, sz, u1, u2], ...]}
        tiles: {"gx,gz": u16} terrain words to set (global tile coords).
        Ledge props get a Y from the BDHC floor, and the area's prop set / prop texture set
        are extended with the ledge models and texture if they aren't there yet.
        Returns (notes, {path: bytes}).
        """
        h = self.headers[header]
        matrix = json.load(open(os.path.join(MATRIX_DIR, h["matrix"] + ".json"), encoding="utf-8"))
        names = self.prop_model_names()
        ledge_ids = {names.index(f) for f in LEDGE_MODEL_STYLE if f in names}
        own = {}
        for r, row in enumerate(matrix["maps"]):
            for c, m in enumerate(row):
                hdrs = matrix.get("headers") or []
                if m != "MAP_NONE" and (not hdrs or hdrs[r][c] == header):
                    own[m] = (r, c)
        if not own:
            own = {m: (r, c) for r, row in enumerate(matrix["maps"]) for c, m in enumerate(row) if m != "MAP_NONE"}

        by_map = {}
        for key, value in tiles.items():
            gx, gz = (int(v) for v in key.split(","))
            m = matrix["maps"][gz // CHUNK][gx // CHUNK]
            if m not in own:
                raise ValueError(f"Tile {gx},{gz} isn't part of this map.")
            if not (isinstance(value, int) and 0 <= value <= 0xFFFF):
                raise ValueError(f"Bad tile value at {gx},{gz}.")
            by_map.setdefault(m, {})[(gx % CHUNK, gz % CHUNK)] = value
        for m in chunk_props:
            if m not in own:
                raise ValueError(f"{m} isn't part of this map.")
            if len(chunk_props[m]) > MAX_PROPS_PER_CHUNK:
                raise ValueError(f"{m}: at most {MAX_PROPS_PER_CHUNK} props per map chunk.")

        files, notes, used_ledges = {}, [], set()
        for m in set(by_map) | set(chunk_props):
            path = land_path(m)
            sections = split_land(open(path, "rb").read())
            if m in by_map:
                terrain = bytearray(sections[0])
                for (tx, tz), v in by_map[m].items():
                    struct.pack_into("<H", terrain, (tz * CHUNK + tx) * 2, v)
                sections[0] = bytes(terrain)
            if m in chunk_props:
                out = b""
                for p in chunk_props[m]:
                    if len(p) != 12 or not all(isinstance(v, int) for v in p):
                        raise ValueError(f"{m}: malformed prop entry.")
                    if not 0 <= p[0] < len(names):
                        raise ValueError(f"{m}: unknown prop model {p[0]}.")
                    if p[0] in ledge_ids:
                        # sit the decal on the floor at the middle of the tile it covers
                        p = list(p)
                        cx, cz = ledge_center(p[1], p[3], p[5])
                        p[2] = bdhc_height(sections[3], cx, cz)
                        used_ledges.add(p[0])
                    out += struct.pack(PROP_FMT, *p)
                sections[1] = out
            files[path] = join_land(sections)

        if used_ledges:
            notes += self.ensure_area_props(h["area"], [names[i] for i in sorted(used_ledges)], files)
        return notes, files

    def ensure_area_props(self, area, model_files, files):
        """Make the area's prop set load these models and its prop texture set carry their ledge textures."""
        notes = []
        styles = sorted({LEDGE_MODEL_STYLE[f] for f in model_files})
        area_json = json.load(open(os.path.join(AREA_DIR, area + ".json"), encoding="utf-8"))
        set_path = os.path.join(PROPS_DIR, "model_sets", area_json["mapPropSet"] + ".json")
        prop_set = json.load(open(set_path, encoding="utf-8"))
        added = [f.replace(".", "_") for f in model_files if f.replace(".", "_") not in prop_set["mapPropModels"]]
        if added:
            prop_set["mapPropModels"] += added
            files[set_path] = (json.dumps(prop_set, indent=4) + "\n").encode()
            notes.append(f"Added {', '.join(added)} to {area_json['mapPropSet']}.json.")
        tex_path = os.path.join(PROPS_DIR, "texture_sets", area_json["mapPropSet"].replace("prop_model_set", "prop_texture_set") + ".nsbtx")
        for style in styles:
            s = LEDGE_STYLES[style]
            current = files.get(tex_path) or open(tex_path, "rb").read()
            texs, pals = btx_contents(current)
            with open(os.path.join(ROOT, s["source"]), "rb") as g:
                source = g.read()
            if s["texture"] in [t[0] for t in texs] or s["palette"] in pals:
                # Already there: make sure it's ours and not an unrelated texture that happens to share the name.
                have, ours = parse_tex0(current), parse_tex0(source)
                key_t, key_p = s["texture"].encode().ljust(16, b"\0"), s["palette"].encode().ljust(16, b"\0")
                if key_t not in dict(have["texs"]) or key_p not in dict(have["pals"]) or \
                        texture_bytes(have, dict(have["texs"])[key_t]) != texture_bytes(ours, dict(ours["texs"])[key_t]) or \
                        palette_bytes(have, key_p) != palette_bytes(ours, key_p):
                    raise ValueError(f"{os.path.basename(tex_path)} already has a different '{s['texture']}'/'{s['palette']}'.")
            else:
                files[tex_path] = btx_append(current, source, only={s["texture"], s["palette"]})
                notes.append(f"Added the {style} ledge texture ({s['texture']}) to {os.path.basename(tex_path)}.")
        return notes

    def script_tokens(self, header, prefix):
        refs = set()
        for p in self.script_paths(header):
            if os.path.exists(p):
                refs.update(re.findall(rf"\b{prefix}[A-Z0-9_]+\b", open(p, encoding="utf-8", errors="replace").read()))
        return refs

    def localid_refs(self, header):
        return self.script_tokens(header, "LOCALID_")

    def map_data(self, header):
        h = self.headers[header]
        matrix = json.load(open(os.path.join(MATRIX_DIR, h["matrix"] + ".json"), encoding="utf-8"))
        maps, hdrs = matrix["maps"], matrix.get("headers") or []
        rows, cols = len(maps), len(maps[0]) if maps else 0
        own = set()
        for r in range(rows):
            for c in range(cols):
                if maps[r][c] == "MAP_NONE":
                    continue
                if not hdrs or hdrs[r][c] == header:
                    own.add((r, c))
        if not own:
            own = {(r, c) for r in range(rows) for c in range(cols) if maps[r][c] != "MAP_NONE"}

        events_path = os.path.join(EVENTS_DIR, h["events"] + ".json")
        events, _ = read_json_keep_tail(events_path)

        r0, r1 = min(r for r, _ in own), max(r for r, _ in own)
        c0, c1 = min(c for _, c in own), max(c for _, c in own)
        for o in events.get("object_events", []) + events.get("warp_events", []) + events.get("bg_events", []):
            r0, r1 = min(r0, o["z"] // CHUNK), max(r1, o["z"] // CHUNK)
            c0, c1 = min(c0, o["x"] // CHUNK), max(c1, o["x"] // CHUNK)
        r0, c0 = max(r0, 0), max(c0, 0)
        r1, c1 = min(r1, rows - 1), min(c1, cols - 1)

        w, hgt = (c1 - c0 + 1) * CHUNK, (r1 - r0 + 1) * CHUNK
        beh, col, mine, raw = [0] * (w * hgt), [0] * (w * hgt), [0] * (w * hgt), [0] * (w * hgt)
        chunks, props = [], []
        model_names = self.prop_model_names()
        for r in range(r0, r1 + 1):
            for c in range(c0, c1 + 1):
                land = load_land(maps[r][c])
                if land is None:
                    continue
                is_own = 1 if (r, c) in own else 2
                for tz in range(CHUNK):
                    for tx in range(CHUNK):
                        v = land[tz * CHUNK + tx]
                        i = ((r - r0) * CHUNK + tz) * w + (c - c0) * CHUNK + tx
                        beh[i], col[i], mine[i], raw[i] = v & 0xFF, 1 if v & COLLISION_BIT else 0, is_own, v
                if is_own == 1:
                    sections = split_land(open(land_path(maps[r][c]), "rb").read())
                    chunks.append({"map": maps[r][c], "gx": c * CHUNK, "gz": r * CHUNK})
                    for p in parse_props(sections[1]):
                        name = model_names[p[0]] if 0 <= p[0] < len(model_names) else f"#{p[0]}"
                        px, pz = ledge_center(p[1], p[3], p[5]) if name in LEDGE_MODEL_STYLE else (p[1], p[3])
                        props.append({"map": maps[r][c], "raw": p, "name": name,
                                      "tx": c * CHUNK + int((px / FX_ONE + 256) // 16),
                                      "tz": r * CHUNK + int((pz / FX_ONE + 256) // 16)})
        # "style:piece" -> prop model id / file name, e.g. "cave:l"
        ledge_ids = {f"{style}:{k}": model_names.index(v + ".nsbmd")
                     for style, s in LEDGE_STYLES.items() for k, v in s["models"].items() if v + ".nsbmd" in model_names}
        ledge_names = {f"{style}:{k}": v + ".nsbmd" for style, s in LEDGE_STYLES.items() for k, v in s["models"].items()}
        area = json.load(open(os.path.join(AREA_DIR, h["area"] + ".json"), encoding="utf-8")) if h.get("area") else {}
        return {
            "header": header, "events_file": h["events"], "matrix": h["matrix"],
            "x0": c0 * CHUNK, "z0": r0 * CHUNK, "w": w, "h": hgt,
            "behavior": beh, "collision": col, "own": mine, "raw": raw,
            "chunks": chunks, "props": props, "ledge_ids": ledge_ids,
            "ledge_names": ledge_names,
            # Suggest the cave style where the map's own texture set has the cave ledge texture.
            "ledge_default_style": "cave" if area.get("mapTextureSet") and LEDGE_STYLES["cave"]["texture"] in
                [t[0] for t in btx_contents(os.path.join(ROOT, "res", "field", "maps", "texture_sets", area["mapTextureSet"] + ".nsbtx"))[0]] else "outdoor",
            "max_props_per_chunk": MAX_PROPS_PER_CHUNK,
            "events": events, "localid_refs": sorted(self.localid_refs(header)),
            "local_flag_refs": sorted(self.script_tokens(header, "FLAG_MAP_LOCAL_")),
        }

    def validate_item_fields(self, ev, errors, label, need_range=False, required=False):
        if required and "_item" not in ev:
            errors.append(f"{label}: no item chosen.")
        if "_item" in ev and ev["_item"] not in self.items:
            errors.append(f"{label}: unknown item {ev['_item']!r}.")
        if "_qty" in ev and not (isinstance(ev["_qty"], int) and 1 <= ev["_qty"] <= 99):
            errors.append(f"{label}: quantity must be 1-99.")
        if need_range and "_range" in ev and not (isinstance(ev["_range"], int) and 0 <= ev["_range"] <= 15):
            errors.append(f"{label}: dowsing range must be 0-15.")

    def save(self, header, object_events, bg_events=None, props=None, tiles=None):
        h = self.headers[header]
        stem = h["events"]
        path = os.path.join(EVENTS_DIR, stem + ".json")
        rel_events = os.path.relpath(path, ROOT).replace(os.sep, "/")
        current, tail = read_json_keep_tail(path)
        if bg_events is None:
            bg_events = current.get("bg_events", [])

        ids = [o.get("id") for o in object_events]
        errors = []
        for o in object_events:
            s = o.get("script")
            if s == "NEW_ITEM":
                self.validate_item_fields(o, errors, o.get("id"), required=True)
            elif isinstance(s, int) and VISIBLE_BASE <= s < HIDDEN_BASE and ("_item" in o or "_qty" in o):
                e = self.visible[s - VISIBLE_BASE] if s - VISIBLE_BASE < len(self.visible) else None
                if not e or not e["editable"]:
                    errors.append(f"{o.get('id')}: item script {s} can't be edited here.")
                self.validate_item_fields(o, errors, o.get("id"))
        for n, b in enumerate(bg_events):
            for k in ("x", "z", "y"):
                if not isinstance(b.get(k), int):
                    errors.append(f"Background event {n}: {k} must be a whole number.")
            s = b.get("script")
            if s == "NEW_HIDDEN":
                self.validate_item_fields(b, errors, f"Hidden item at {b.get('x')},{b.get('z')}", True, required=True)
            elif isinstance(s, int) and HIDDEN_BASE <= s < HIDDEN_LIMIT and any(k in b for k in ("_item", "_qty", "_range")):
                if s - HIDDEN_BASE not in self.hidden:
                    errors.append(f"Hidden item script {s} has no entry in hidden_items.h.")
                self.validate_item_fields(b, errors, f"Hidden item at {b.get('x')},{b.get('z')}", True)
        if len(set(ids)) != len(ids):
            errors.append("Two objects share the same id.")
        for o in object_events:
            if not isinstance(o.get("id"), str) or not re.fullmatch(r"LOCALID_[A-Z0-9_]+", o["id"]):
                errors.append(f"Bad id: {o.get('id')!r} (must be LOCALID_ + capitals/digits/underscores).")
            for k in ("x", "z", "y", "initial_dir", "movement_range_x", "movement_range_z"):
                if not isinstance(o.get(k), int):
                    errors.append(f"{o.get('id')}: {k} must be a whole number.")
            for k, allowed in (("graphics_id", self.gfx), ("movement_type", self.movement_types), ("trainer_type", self.trainer_types)):
                if o.get(k) not in allowed:
                    errors.append(f"{o.get('id')}: unknown {k} {o.get(k)!r}.")
            s = o.get("script")
            if isinstance(s, str) and s.startswith("TRAINER_") and s not in self.trainer_info:
                errors.append(f"{o.get('id')}: unknown trainer {s}.")
        removed = {o["id"] for o in current.get("object_events", []) if "id" in o} - set(ids)
        referenced = removed & self.localid_refs(header)
        for rid in sorted(referenced):
            errors.append(f"{rid} is referenced in this map's scripts, so it can't be removed here.")
        if errors:
            return {"ok": False, "errors": errors}

        before = {o.get("script") for o in current.get("object_events", [])}
        try:
            notes, files = self.apply_items(stem, rel_events, object_events, bg_events)
            if props or tiles:
                prop_notes, prop_files = self.save_props(header, props or {}, tiles or {})
                notes += prop_notes
                files.update(prop_files)
        except (ValueError, subprocess.CalledProcessError) as e:
            return {"ok": False, "errors": [str(e)]}
        for fpath in list(files):
            with open(fpath, "rb") as f:
                current_bytes = f.read()
            new_bytes = files[fpath] if isinstance(files[fpath], bytes) else files[fpath].encode("utf-8")
            if new_bytes == current_bytes:
                del files[fpath]
        strip = lambda ev: {k: v for k, v in ev.items() if not k.startswith("_")}
        object_events = [strip(o) for o in object_events]
        bg_events = [strip(b) for b in bg_events]
        after = {o.get("script") for o in object_events}
        for fpath, content in files.items():
            if isinstance(content, bytes):
                with open(fpath, "wb") as f:
                    f.write(content)
            else:
                write_text(fpath, content)
        current["object_events"] = object_events
        current["bg_events"] = bg_events
        write_json_keep_tail(path, current, tail)
        self.rescan()
        is_trainer = lambda s: isinstance(s, str) and s.startswith("TRAINER_")
        return {"ok": True, "notes": notes,
                "changed_files": sorted(os.path.relpath(p, ROOT) for p in files),
                "added_trainers": sorted(s for s in after - before if is_trainer(s)),
                "removed_trainers": sorted(s for s in before - after if is_trainer(s))}

    def apply_items(self, stem, rel_events, object_events, bg_events):
        """Resolve NEW_ITEM / NEW_HIDDEN placeholders and item edits.

        Mutates the events in place and returns (notes, {path: new text}) for
        the script / header / vars_flags files that change. Flags are only ever
        renamed in place, never appended, so the save layout doesn't move.
        """
        notes = []
        vis_src = orig_vis = read_text(VISIBLE_ITEMS_S)
        hid_src = orig_hid = read_text(HIDDEN_ITEMS_H)
        flags_src = orig_flags = read_text(VARS_FLAGS_TXT)
        visible = [dict(e) for e in self.visible]
        all_flags = set(self.flags["all"])
        mapname = stem[len("events_"):].upper() if stem.startswith("events_") else stem.upper()

        used_flags = {o.get("hidden_flag") for o in object_events}
        claimed = set()

        def referenced_elsewhere(flag):
            return any(p != rel_events for p in self.flag_refs.get(flag, ()))

        def rename_flag(old, prefix, item):
            nonlocal flags_src, hid_src
            base = f"{prefix}{mapname}_{item[len('ITEM_'):]}"
            new, n = base, 2
            while new in all_flags and new != old:
                new, n = f"{base}_{n}", n + 1
            if new == old:
                return old
            flags_src = re.sub(rf"\b{old}\b", new, flags_src)
            hid_src = re.sub(rf"\b{old}\b", new, hid_src)
            all_flags.discard(old)
            all_flags.add(new)
            return new

        # ---- item balls ----
        vis_here = {o["script"] - VISIBLE_BASE for o in object_events
                    if isinstance(o.get("script"), int) and VISIBLE_BASE <= o["script"] < HIDDEN_BASE}

        def set_visible(label, item, qty):
            nonlocal vis_src
            a, b = visible_block_span(vis_src, label)
            block = re.sub(r"(SetVar VAR_0x8008, )ITEM_\w+", rf"\g<1>{item}", vis_src[a:b])
            block = re.sub(r"(SetVar VAR_0x8009, )\d+", rf"\g<1>{qty}", block)
            vis_src = vis_src[:a] + block + vis_src[b:]

        for o in object_events:
            s = o.get("script")
            if isinstance(s, int) and VISIBLE_BASE <= s < HIDDEN_BASE and ("_item" in o or "_qty" in o):
                e = visible[s - VISIBLE_BASE]
                e["item"], e["qty"] = o.get("_item", e["item"]), o.get("_qty", e["qty"])
                set_visible(e["label"], e["item"], e["qty"])
                others = [p for p in self.vis_placed.get(e["index"], []) if p != stem]
                notes.append(f"{o['id']}: {e['label']} now gives {e['qty']}x {e['item']}"
                             + (f" (also used on {', '.join(others)})" if others else "") + ".")

        free_entries = [e for e in visible if e["editable"] and e["index"] not in vis_here
                        and not any(p != stem for p in self.vis_placed.get(e["index"], []))]
        flag_pool = [f for f in self.flags["item_region"] if f.startswith("FLAG_UNUSED_")]
        flag_pool += [f for f in self.flags["item_region"]
                      if f.startswith("FLAG_OBTAINED_") and not f.startswith("FLAG_OBTAINED_HIDDEN_")]

        for o in object_events:
            if o.get("script") != "NEW_ITEM":
                continue
            item, qty = o["_item"], o.get("_qty", 1)
            if free_entries:
                e = free_entries.pop(0)
                set_visible(e["label"], item, qty)
                idx, how = e["index"], f"reused unplaced script {e['label']}"
            else:
                label, n = f"VisibleItems_{camel(mapname)}_{camel(item[len('ITEM_'):])}", 2
                while any(v["label"] == label for v in visible):
                    label, n = f"VisibleItems_{camel(mapname)}_{camel(item[len('ITEM_'):])}{n}", n + 1
                idx = len(visible)
                if VISIBLE_BASE + idx >= HIDDEN_BASE:
                    raise ValueError("No room for another item ball script.")
                vis_src = vis_src.replace("    ScriptEntryEnd", f"    ScriptEntry {label}\n    ScriptEntryEnd", 1)
                vis_src = vis_src.replace("VisibleItems_TryGiveItem:\n",
                                          f"{label}:\n    SetVar VAR_0x8008, {item}\n    SetVar VAR_0x8009, {qty}\n"
                                          f"    GoTo VisibleItems_TryGiveItem\n    End\n\nVisibleItems_TryGiveItem:\n", 1)
                visible.append({"index": idx, "label": label, "item": item, "qty": qty, "editable": True})
                how = f"new script {label}"
            flag = next((f for f in flag_pool if f not in claimed and f not in used_flags
                         and not referenced_elsewhere(f)), None)
            if not flag:
                raise ValueError("No unused flag left for a new item ball.")
            claimed.add(flag)
            warn = "" if flag.startswith("FLAG_UNUSED_") else \
                f" Reused flag {flag} from a removed item ball: saves that already picked that one up will see this one as taken."
            flag = rename_flag(flag, "FLAG_OBTAINED_", item)
            o["script"], o["hidden_flag"] = VISIBLE_BASE + idx, flag
            notes.append(f"{o['id']}: {qty}x {item}, script {VISIBLE_BASE + idx} ({how}), flag {flag}.{warn}")

        # ---- hidden items ----
        hid_here = {b["script"] - HIDDEN_BASE for b in bg_events
                    if isinstance(b.get("script"), int) and HIDDEN_BASE <= b["script"] < HIDDEN_LIMIT}
        hidden = {i: dict(e) for i, e in self.hidden.items()}

        def set_hidden(e):
            nonlocal hid_src
            line = f"HIDDEN_ITEM_ENTRY({e['item']}, {e['qty']}, {e['range']}, {e['flag']})"
            hid_src, n = re.subn(HIDDEN_ENTRY_RE.format(flag=re.escape(e["flag"])), line, hid_src, count=1)
            if not n:
                raise ValueError(f"Couldn't find {e['flag']} in hidden_items.h.")

        for b in bg_events:
            s = b.get("script")
            if isinstance(s, int) and HIDDEN_BASE <= s < HIDDEN_LIMIT and any(k in b for k in ("_item", "_qty", "_range")):
                e = hidden[s - HIDDEN_BASE]
                e["item"], e["qty"], e["range"] = b.get("_item", e["item"]), b.get("_qty", e["qty"]), b.get("_range", e["range"])
                set_hidden(e)
                others = [p for p in self.hid_placed.get(e["index"], []) if p != stem]
                notes.append(f"Hidden item {e['flag']} now gives {e['qty']}x {e['item']}"
                             + (f" (also used on {', '.join(others)})" if others else "") + ".")

        unplaced = [e for i, e in sorted(hidden.items()) if i not in hid_here
                    and not any(p != stem for p in self.hid_placed.get(i, []))]
        hid_pool = [("entry", e) for e in unplaced if "UNUSED" in e["flag"]]
        hid_pool += [("flag", (i, f)) for i, f in enumerate(self.flags["hidden"])
                     if f.startswith("FLAG_UNUSED_") and i not in hidden and not self.flag_refs.get(f)]
        hid_pool += [("entry", e) for e in unplaced if "UNUSED" not in e["flag"]]

        for b in bg_events:
            if b.get("script") != "NEW_HIDDEN":
                continue
            item, qty, rng = b["_item"], b.get("_qty", 1), b.get("_range", 2)
            if not hid_pool:
                raise ValueError("No unused hidden-item flag left.")
            kind, val = hid_pool.pop(0)
            if kind == "entry":
                e, warn = val, ("" if "UNUSED" in val["flag"] else
                                f" Reused {val['flag']} from a removed hidden item: saves that already found that one will see this one as taken.")
                e["item"], e["qty"], e["range"] = item, qty, rng
                set_hidden(e)
                e["flag"] = rename_flag(e["flag"], "FLAG_OBTAINED_HIDDEN_", item)
            else:
                idx, flag = val
                flag = rename_flag(flag, "FLAG_OBTAINED_HIDDEN_", item)
                e, warn = {"index": idx, "item": item, "qty": qty, "range": rng, "flag": flag}, ""
                hid_src, n = re.subn(r"(const HiddenItem gHiddenItems\[\] = \{.*?)(\n\};)",
                                     lambda m: f"{m.group(1)}\n    HIDDEN_ITEM_ENTRY({item}, {qty}, {rng}, {flag}),{m.group(2)}",
                                     hid_src, count=1, flags=re.S)
                if not n:
                    raise ValueError("Couldn't find the gHiddenItems table in hidden_items.h.")
                hidden[idx] = e
            b["script"] = HIDDEN_BASE + e["index"]
            b["type"] = BG_EVENT_HIDDEN_ITEM
            b.setdefault("player_facing_dir", "BG_EVENT_DIR_ALL")
            notes.append(f"Hidden {qty}x {item} at {b['x']},{b['z']}: script {b['script']}, flag {e['flag']}.{warn}")

        files = {}
        for p, new, old in ((VISIBLE_ITEMS_S, vis_src, orig_vis), (HIDDEN_ITEMS_H, hid_src, orig_hid),
                            (VARS_FLAGS_TXT, flags_src, orig_flags)):
            if new != old:
                files[p] = new
        return notes, files


INDEX = None


class Handler(BaseHTTPRequestHandler):
    def log_message(self, fmt, *args):
        pass

    def send_json(self, obj, code=200):
        body = json.dumps(obj).encode("utf-8")
        self.send_response(code)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        url = urlparse(self.path)
        try:
            if url.path in ("/", "/index.html"):
                body = open(os.path.join(HERE, "index.html"), "rb").read()
                self.send_response(200)
                self.send_header("Content-Type", "text/html; charset=utf-8")
                self.send_header("Content-Length", str(len(body)))
                self.end_headers()
                self.wfile.write(body)
            elif url.path == "/api/meta":
                self.send_json(INDEX.meta())
            elif url.path == "/api/map":
                header = parse_qs(url.query).get("header", [""])[0]
                if header not in INDEX.headers:
                    self.send_json({"error": f"unknown map {header}"}, 404)
                else:
                    self.send_json(INDEX.map_data(header))
            else:
                self.send_json({"error": "not found"}, 404)
        except Exception as e:  # surface the problem in the UI instead of a dead socket
            self.send_json({"error": f"{type(e).__name__}: {e}"}, 500)

    def do_POST(self):
        url = urlparse(self.path)
        try:
            if url.path != "/api/save":
                self.send_json({"error": "not found"}, 404)
                return
            req = json.loads(self.rfile.read(int(self.headers.get("Content-Length", 0))))
            if req.get("header") not in INDEX.headers:
                self.send_json({"ok": False, "errors": ["Unknown map."]}, 400)
                return
            self.send_json(INDEX.save(req["header"], req["object_events"], req.get("bg_events"),
                                      req.get("props"), req.get("tiles")))
        except Exception as e:
            self.send_json({"ok": False, "errors": [f"{type(e).__name__}: {e}"]}, 500)


def main():
    global INDEX
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=8137)
    args = ap.parse_args()
    INDEX = Index()
    server = ThreadingHTTPServer((args.host, args.port), Handler)
    print(f"Trainer placer running at http://localhost:{args.port}  (Ctrl+C to stop)")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    sys.exit(main())
