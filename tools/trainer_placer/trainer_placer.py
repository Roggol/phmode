#!/usr/bin/env python3
"""Browser-based editor for placing trainers on field maps.

Edits res/field/events/*.json in place (the same files the build reads), so
changes show up as ordinary git diffs. Run from anywhere:

    python3 tools/trainer_placer/trainer_placer.py [--port 8137] [--host 127.0.0.1]

then open http://localhost:8137 in a browser.
"""

import argparse
import glob
import json
import os
import re
import struct
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

CHUNK = 32
TERRAIN_OFFSET = 0x10
COLLISION_BIT = 0x8000


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
        }
    return headers


def parse_tile_behaviors():
    src = open(os.path.join(ROOT, "include", "constants", "field", "map_tile_behaviors.h"), encoding="utf-8").read()
    names, value = {}, 0
    for m in re.finditer(r"^\s*(TILE_BEHAVIOR_\w+)\s*(?:=\s*(0x[0-9A-Fa-f]+|\d+))?\s*,", src, re.M):
        if m.group(2) is not None:
            value = int(m.group(2), 0)
        names[value] = m.group(1)
        value += 1
    return [names.get(i, "") for i in range(256)]


def load_land(map_name):
    if map_name == "MAP_NONE":
        return None
    idx = int(map_name.split("_")[1])
    path = os.path.join(LAND_DIR, f"map_data_{idx:03d}.bin")
    if not os.path.exists(path):
        return None
    with open(path, "rb") as f:
        f.seek(TERRAIN_OFFSET)
        return struct.unpack(f"<{CHUNK * CHUNK}H", f.read(CHUNK * CHUNK * 2))


def read_json_keep_tail(path):
    raw = open(path, encoding="utf-8").read()
    tail = raw[len(raw.rstrip()):]
    return json.loads(raw), tail


def write_json_keep_tail(path, data, tail):
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(json.dumps(data, indent=4, ensure_ascii=False) + (tail or "\n"))


class Index:
    """Cross-map facts that need a full scan: where each trainer is placed and which ones scripts start."""

    def __init__(self):
        self.headers = parse_map_headers()
        self.behaviors = parse_tile_behaviors()
        self.gfx = const_names("object_events_gfx.txt")
        self.movement_types = const_names("movement_types.txt")
        self.trainer_types = const_names("trainer_types.txt")
        self.trainer_consts = [t for t in const_names("trainers.txt") if t != "TRAINER_NONE"]
        self.rescan()

    def rescan(self):
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
            for o in json.load(open(path, encoding="utf-8")).get("object_events", []):
                s = o.get("script")
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
        return {"maps": maps, "gfx": self.gfx, "movement_types": self.movement_types,
                "trainer_types": self.trainer_types, "trainers": trainers, "behaviors": self.behaviors}

    def script_paths(self, header):
        h = self.headers[header]
        return [os.path.join(SCRIPTS_DIR, n + ".s") for n in (h["scripts"], h["init_scripts"]) if n]

    def localid_refs(self, header):
        refs = set()
        for p in self.script_paths(header):
            if os.path.exists(p):
                refs.update(re.findall(r"\bLOCALID_[A-Z0-9_]+\b", open(p, encoding="utf-8", errors="replace").read()))
        return refs

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
        beh, col, mine = [0] * (w * hgt), [0] * (w * hgt), [0] * (w * hgt)
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
                        beh[i], col[i], mine[i] = v & 0xFF, 1 if v & COLLISION_BIT else 0, is_own
        return {
            "header": header, "events_file": h["events"], "matrix": h["matrix"],
            "x0": c0 * CHUNK, "z0": r0 * CHUNK, "w": w, "h": hgt,
            "behavior": beh, "collision": col, "own": mine,
            "events": events, "localid_refs": sorted(self.localid_refs(header)),
        }

    def save(self, header, object_events):
        h = self.headers[header]
        path = os.path.join(EVENTS_DIR, h["events"] + ".json")
        current, tail = read_json_keep_tail(path)

        ids = [o.get("id") for o in object_events]
        errors = []
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
        after = {o.get("script") for o in object_events}
        current["object_events"] = object_events
        write_json_keep_tail(path, current, tail)
        self.rescan()
        is_trainer = lambda s: isinstance(s, str) and s.startswith("TRAINER_")
        return {"ok": True,
                "added_trainers": sorted(s for s in after - before if is_trainer(s)),
                "removed_trainers": sorted(s for s in before - after if is_trainer(s))}


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
            self.send_json(INDEX.save(req["header"], req["object_events"]))
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
