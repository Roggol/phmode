# phmode — test build notes

Temporary changes that make the new abilities, moves and terrains reachable
early for testing. **Not** gameplay changes — none of this is recorded in
`CHANGES.md`, and it should be reverted before shipping.

Files touched:

- `res/field/encounters/encounters_route_201.json`
- `res/field/encounters/encounters_lake_verity.json`
- `include/data/map_headers.h` (Route 201/202/203, Lake Verity, Verity Lakefront `.weather`)
- `res/field/scripts/scripts_twinleaf_town.s` (guitarist gift quantity)

Level cap is left at its existing value of **60**.

## Wild spawns

All entries are ordinary tall-grass encounters at a high encounter rate. Levels
are chosen so the new level-up move is already in the wild moveset.

### Route 201 — new abilities + Boomburst

| Species | Level | What to test |
| --- | --- | --- |
| Honchkrow | 50 | Moxie |
| Drifloon | 50 | Flare Boost |
| Drifblim | 50 | Flare Boost |
| Kecleon | 50 | Protean |
| Cacturne | 50 | Shed Spines |
| Gallade | 50 | Justified / Sharpness |
| Croagunk | 45 | Poison Touch |
| Toxicroak | 50 | Poison Touch |
| Ampharos | 50 | Electric Surge |
| Grumpig | 50 | Psychic Surge |
| Glalie | 50 | Refrigerate |
| Exploud | 50 | Boomburst (learns it at 45) |

### Lake Verity — legendaries + Sticky Web / Wild Charge / Expanding Force

| Species | Level | What to test |
| --- | --- | --- |
| Luxray | 52 | Wild Charge (learns it at 49) |
| Spinarak | 35 | Sticky Web (learns it at 29) |
| Ariados | 38 | Sticky Web (learns it at 32) |
| Uxie | 55 | Psychic Surge + Expanding Force (51) |
| Mesprit | 55 | Psychic Surge + Expanding Force (51) |
| Azelf | 55 | Psychic Surge + Expanding Force (51) |
| Dialga | 55 | Time Warp |
| Palkia | 55 | Space Warp |
| Giratina | 55 | Distortion Surge |

Route 202/203 and Verity Lakefront were not needed for spawns.

## Route weather / terrain

Applied via each map header's `.weather`. There is no overworld visual (values
≥ 31 render as clear); the effect applies in battle on switch-in.

| Map | Weather |
| --- | --- |
| Route 201 | Sticky Web (player's side webbed on send-out) |
| Route 202 | Electric Terrain |
| Route 203 | Psychic Terrain |
| Lake Verity | Distortion Terrain |
| Verity Lakefront | Harsh Sun |

## Guitarist gift

The Twinleaf Town guitarist now hands over **999 Rare Candies** (plus the 99
Master Balls that were already part of that gift). Still a one-time gift, gated
by `FLAG_RECEIVED_TWINLEAF_TOWN_GUITARIST_RARE_CANDY`.

## Caveat — ability slot roll

Species with two real abilities roll 50/50 for the new one in the wild:
**Honchkrow, Drifloon, Drifblim, Kecleon, Croagunk, Toxicroak, Ampharos,
Grumpig, Glalie**. Catch a few or knock out and re-encounter.

Guaranteed to have the new ability: **Cacturne**, **Gallade** (one of the two
new ones), and the six **legendaries** (their second slot is empty / identical).
**Exploud** has no new ability — it is here only for Boomburst.
