# phmode — test build notes

Changes that make every new / changed move, ability and terrain reachable early
for testing.

The **encounter tables, route weather, the Verity Lakefront grass patch, the
Lass battle and the guitarist gift are test-only** — revert them before shipping
and they are deliberately *not* in `CHANGES.md`. The **ability reassignments**
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
- `res/field/maps/data/map_data_004.bin` — Verity Lakefront grass patch
- `include/data/map_headers.h` — `.weather` on Routes 201–204 + both Lake Verity
  headers + Verity Lakefront, and Verity Lakefront `.wildEncountersArchiveID`

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

A 5×5 patch of encounter tiles was added to `map_data_004.bin` (top-right cell)
at world **X 85–89, Z 846–850** — the clearing just above the ledge, east of the
hop-west ledge, a few tiles south of the Lake Verity entrance. Only the tile
*behavior* is set to `TILE_BEHAVIOR_TALL_GRASS`: you get the grass rustle sprite,
the step animation, the sound and wild encounters, but the ground texture is
unchanged (DPPt maps are 3D `BMD0`/NSBMD models shipped as binaries with no
editable source, so there is no cheap way to paint visible grass tufts).
`MAP_HEADER_VERITY_LAKEFRONT.wildEncountersArchiveID` was pointed at
`encounters_route_201`, so the patch spawns the Route 201 roster under Harsh Sun.

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
