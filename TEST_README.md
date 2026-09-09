# phmode — test build notes

Changes that make every new / changed move, ability and terrain reachable early
for testing.

The **encounter tables, route weather, the Lass battle and the guitarist gift
are test-only** — revert them before shipping and they are deliberately *not* in
`CHANGES.md`. The **Verity Lakefront grass patch** (the `map_data_004.bin` model
+ tile behaviour) is now a **real change** and is in `CHANGES.md`; only its
encounter-table / weather wiring stays test-only. The **ability reassignments**
needed to make Defiant and Competitive testable (Farfetch'd / Mankey line /
Purugly → Defiant, Milotic → Competitive) are real gameplay changes and *are*
recorded in `CHANGES.md`.

Files touched (test-only):

- `res/trainers/data/lass_carrie_unused_1.json` — Route 201 Lass test party
- `res/field/scripts/scripts_route_201.s` — Route 201 Lass battle script
- `res/field/scripts/scripts_twinleaf_town.s` — guitarist gift quantity
- `res/field/encounters/encounters_route_{201,202,203}.json`
- `res/field/encounters/encounters_route_204_south.json`
- `res/field/encounters/encounters_lake_verity.json`
- `res/field/encounters/encounters_lake_verity_low_water.json` (kept identical)
- `res/field/encounters/encounters_verity_lakefront.json` — *contents only*; the
  file, its `meson.build` / `encounters.order` entries and the map-header wiring
  are real (see `CHANGES.md`), but the spawn list is a copy of the Route 201 test
  roster
- `include/data/map_headers.h` — `.weather` on Routes 201–204 + both Lake Verity
  headers + Verity Lakefront (the `map_data_004.bin` grass patch and Verity
  Lakefront's `.wildEncountersArchiveID` → `encounters_verity_lakefront` are real
  changes — see `CHANGES.md`)

Level cap is left at its existing value of **60**.

Encounter tables use the standard DPPt slot weights (20 / 20 / 10 / 10 / 10 / 10
/ 5 / 5 / 4 / 4 / 1 / 1). Every Pokémon that is the *only* way to test something
sits in slots 0–7, i.e. **≥ 5%**; slots 8–11 are duplicates for better odds.

**Lake Verity has two map headers.** `MAP_HEADER_LAKE_VERITY` is used walking in
from Verity Lakefront (pre-Galactic) or from Verity Cavern;
`MAP_HEADER_LAKE_VERITY_LOW_WATER` is used by the opening cutscene warp and after
the lakes drain. They read separate encounter archives, so the spawns and
Distortion Terrain are applied to **both**.

## Wild spawns

### Route 201 — new abilities (Sticky Web terrain)

| Species | Rarity | New ability |
| --- | --- | --- |
| Honchkrow | 20% | Moxie |
| Glalie | 20% | Refrigerate |
| Drifblim | 10% | Flare Boost |
| Kecleon | 10% | Protean |
| Cacturne | 10% | Shed Spines |
| Gallade | 10% | Justified / Sharpness |
| Grumpig | 5% | Psychic Surge |
| Toxicroak | 5% | Poison Touch |

All level 50. Electric Surge (Ampharos) and Boomburst (Exploud) moved to Route
203 to keep everything here at ≥ 5%.

### Route 202 — vanilla move / ability *mechanics* that changed (Electric Terrain)

Levels are tuned so the changed move is in the wild moveset.

| Species | Level | Changed thing |
| --- | --- | --- |
| Forretress | 20 | **Sturdy** (guaranteed) + **Rapid Spin** (50 BP, +1 Spe, clears every hazard both sides) |
| Tangrowth | 42 | **Knock Off** (65 BP, ×1.5 vs removable item) + **Power Whip** (100% acc, 25% recoil) |
| Vespiquen | 38 | **Attack Order** (100 BP, hits both foes, no high-crit) |
| Vespiquen | 26 | **Heal Order** (also heals the ally in a double) |
| Vespiquen | 16 | **Defend Order** (+2 Def **and** +2 Sp. Def) |
| Scizor | 50 | **X-Scissor** (high-crit ratio removed) |
| Parasect | 20 | **Leech Life** (80 BP) |
| Abomasnow | 50 | Snow Warning → **Snow** (renamed Hail: no chip damage, +50% Def to Ice types) |

Not slotted: the HM retypes (**Strength**→Fighting, **Cut**→Grass,
**Rock Climb**→Rock) and **Defog** (now clears all hazards + Tailwind + Gravity,
both sides) — teach these to any test mon.

### Route 203 — new abilities not reachable on Route 201 (Psychic Terrain)

| Species | Rarity | New ability |
| --- | --- | --- |
| Nidoking | 20% | Sheer Force (vs Poison Point) |
| Empoleon | 20% | Competitive (vs Torrent) |
| Sableye | 10% | Prankster (vs Keen Eye) |
| Zangoose | 10% | Toxic Boost (vs Immunity) |
| Spinda | 10% | Contrary (vs Tangled Feet) |
| Primeape | 10% | **Defiant** (vs Anger Point) |
| Ampharos | 5% | Electric Surge (vs Static) |
| Exploud | 5% | *Boomburst* (level-up move, not an ability) |

All level 50. Every ability here is a 50/50 wild roll — catch a couple or KO and
re-encounter.

### Route 204 South — Normalize + the extra Defiant / Competitive holders (Trick Room)

| Species | Rarity | Thing |
| --- | --- | --- |
| Delcatty | 20% | **Normalize** (now also ×1.2 to Normal moves) — 50/50 vs Cute Charm |
| Milotic | 20% | **Competitive** (2nd holder) — 50/50 vs Marvel Scale |
| Purugly | 10% | **Defiant** (2nd holder) + Thick Fat |
| Farfetch'd | 10% | **Defiant** (2nd holder) |
| Mankey (Lv 40) | 10% | **Defiant** (2nd holder) |
| Nidoqueen | 10% | Sheer Force (2nd holder) |

All level 50 unless noted. Route 204 North keeps its vanilla encounters (it also
gets Trick Room).

### Lake Verity — legendaries + new moves (Distortion Terrain)

| Species | Level | What to test |
| --- | --- | --- |
| Luxray | 52 | Wild Charge (learns it at 49) |
| Spinarak | 35 | Sticky Web (learns it at 29) |
| Uxie / Mesprit / Azelf | 55 | Psychic Surge + Expanding Force (51) |
| Dialga | 55 | Time Warp |
| Palkia | 55 | Space Warp |
| Giratina | 55 | Distortion Surge |

Ariados was dropped — Spinarak covers Sticky Web.

### Verity Lakefront — added grass patch (Harsh Sun)

**The grass patch itself is now a real change** — the `map_data_004.bin` model
was re-exported (via DSPRE) with visible tall grass, and it is documented in
`CHANGES.md` under *Map data*. It is a **4×5** block at local tiles X 21–24,
Z 14–18 (world X 85–88, Z 846–850) — the clearing east of the hop-west ledge, a
few tiles south of the Lake Verity entrance. Still under test; walk it to confirm
the collision lines up with the visible grass and that you can still cross
between this quadrant and the rest of the lakefront.

The location now has its **own** encounter table,
`encounters_verity_lakefront` (real change, in `CHANGES.md`), wired to
`MAP_HEADER_VERITY_LAKEFRONT`. **Its contents are still test data** — a straight
copy of the Route 201 test roster (Honchkrow / Glalie / Drifblim / Kecleon /
Cacturne / Gallade / Grumpig / Toxicroak, all level 50). Retune
`res/field/encounters/encounters_verity_lakefront.json` for real before shipping.

Also still test-only: `MAP_HEADER_VERITY_LAKEFRONT.weather` is
`OVERWORLD_WEATHER_HARSH_SUN` (revert to the vanilla value before shipping).

## Route 201 Lass — repeatable double battle

`LOCALID_LASS` (wanders near the Sandgem end of Route 201) starts a **double
battle every time you talk to her** — the script never sets a defeated flag.
Party (`lass_carrie_unused_1.json` → `TRAINER_LASS_CARRIE_UNUSED_1`): **6 ×
Rattata, Bite + Endure**. The first two are **level 60** and the very first one
holds a **Choice Scarf**; the other four are level 10. Needs two healthy
Pokémon or she just gives her old line.

## Route 201 School Kid — restock NPC

`LOCALID_SCHOOL_KID_M` (the little boy just east of the Lass) says "Enjoy your
testing!" and hands over **100 Rare Candies, 10 Return TMs (TM27), 10 X-Scissor
TMs (TM81) and 50 Max Repels** every time you talk to him — `Route201_SchoolKidM`
in `scripts_route_201.s`, no flag gate.

## Route weather / terrain

Set via each map header's `.weather`. No overworld visual (values ≥ 31 render as
clear); the effect applies in battle on switch-in.

| Map | Weather |
| --- | --- |
| Route 201 | Sticky Web (player's side webbed on send-out) |
| Route 202 | Electric Terrain |
| Route 203 | Psychic Terrain |
| Route 204 (South + North) | Trick Room |
| Lake Verity | Distortion Terrain |
| Verity Lakefront | Harsh Sun |

## Guitarist gift

The Twinleaf Town guitarist hands over **999 Rare Candies** (plus the 99 Master
Balls already in that gift). One-time, gated by
`FLAG_RECEIVED_TWINLEAF_TOWN_GUITARIST_RARE_CANDY`.

## Caveat — ability slot roll

Any species with two real abilities is a 50/50 wild roll for the one being
tested. Guaranteed instead: **Forretress** (Sturdy — sole ability), **Cacturne**
(Shed Spines), **Gallade** (one of Justified / Sharpness), and the six
**legendaries** (empty / identical second slot). Everything else — catch a
couple or KO and re-encounter.

---

## Changed mechanics in this uncommitted batch

Real gameplay changes (not test-only) made since the last commit. All are in
`CHANGES.md` too; this is the short "what to check" list.

### Will-O-Wisp accuracy 75 → 85
`res/moves/will_o_wisp/data.json`. Follow-up to the committed accuracy pass.

### Honey trees give an instant encounter
Slather Honey on a tree → the Honey Tree battle starts immediately instead of
after leaving and coming back 6+ hours later. A "no encounter" roll is promoted
to group A, so a slather is never wasted; the tree still goes bare afterwards and
can be re-slathered right away (offered automatically if you have more Honey).
*Which* Pokémon appears is still the normal random roll.
`src/overlay005/honey_tree.c`, `res/field/scripts/scripts_common.s`.

### "HATCH" option on eggs in the party menu
START → Pokémon → select an egg → **HATCH** hatches it on the spot (the normal
"Oh?" hatch cutscene + nickname prompt), no walking. Only on the field party
menu, not the bag/daycare/selection party screens.
`src/applications/party_menu/*`, `src/start_menu.c`, `res/text/party_menu.json`.

### Repel Toggle (new Key Item)
- Given by the professor's assistant on **Route 202**, right after the catching
  tutorial and the five Poké Balls, with the line *"Take this, I think it will
  be helpful for your journey."* Nurse Joy hands it over as a fallback for saves
  already past that tutorial (check-in heal after the tutorial → obtained).
- Uses the Repel icon, registerable to Y. Using it (bag or Y) toggles it on/off
  with a message each time; the Bag description ends with `(on)` / `(off)`.
- While **on**: weak wild Pokémon never appear and the "REPEL's effect wore off"
  prompt never fires, with no step limit. Turning it off resumes encounters
  immediately. Standard Repel rule still applies — only blocks Pokémon lower
  level than your lead.
- `FLAG_REPEL_TOGGLE_ON` (renamed unused flag); `src/item_use_functions.c`,
  `src/overlay006/repel_step_update.c`, `src/applications/bag/windows.c`,
  `res/items/data/repel_toggle.json`, `res/text/bag.json`.

### Register up to 6 key items to the Y button
- Bag → key item → **Register** adds it to the Y list (up to 6); a registered
  item shows **Deselect**. Every registered item shows the Y icon in the list.
- Pressing **Y** in the field: 0 registered does nothing, 1 uses it directly
  (as before), 2+ opens a small drop-down (registered items + CANCEL) — pick one
  and it's used.
- Stored in `VAR_REGISTERED_KEY_ITEM_0..5` (renamed unused vars), so the save
  layout is unchanged and existing saves keep working (they just start with an
  empty list). New module `src/registered_items.c`; picker in
  `src/item_use_functions.c` / `src/overlay005/field_control.c`.
- Worth a close look: the drop-down window rendering (position / frame) is
  untested visually — register e.g. Bicycle + Repel Toggle + Vs. Seeker and
  press Y.
