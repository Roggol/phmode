# Wild Encounters By Route

> **Note:** this doc's prose was AI-assisted. The user will rewrite it by
> hand before the game is released. (Keep this note if the file is
> regenerated again.)

Every wild-encounter table in the game (grass/land, Surf, Rock Smash, and the three fishing rods), grouped in the same route-progression order as `res/trainers/TRAINER_LOCATIONS.md` - **not** by when trainers there change tiers, but by when the player can actually reach the area, per that file's route-order pass. A location with no bullet points, or missing entirely, has no wild-encounter table in `res/field/encounters/` (it's a town/building interior, a legendary-only chamber, a transfer-only area like Pal Park, etc.). A location made of several sub-maps (Old Chateau's rooms, Solaceon Ruins' many rooms, Great Marsh's 6 areas, Mt Coronet's floors, etc.) gets one `####` subheading per sub-map/floor, each with its own results - floors with no data of their own are skipped.

Percentages are each species' total catch rate for that method at that location (summed across every slot it occupies) - not a slot-by-slot breakdown. **Grass/Land** entries can vary by time of day: the game's own grass table is written for *Morning*, and only two of its twelve slots (worth 10% each) swap to a different species for *Day* (which covers Twilight too) or *Night* (which covers Late Night too) - see `WildEncounters_ReplaceTimedEncounters` in `src/overlay006/wild_encounters.c`. When those two slots don't actually change species across all three periods, only one unlabeled line is shown; otherwise each differing period gets its own `*Period*:` line (periods that end up identical to each other are combined, e.g. `*Morning/Night*:`). Surf, Rock Smash, and the three fishing rods have no time-of-day variation.

Not covered: the Battle Frontier, Turnback Cave, Stark Mountain, Snowpoint Temple, and other Tier 10 (post-Elite Four) content - `tbd later`, per the route-order pass.

## Tier 1 — cap 14

### Twinleaf Town
- **Surf**: Psyduck (Lv. 20-30, 90%), Golduck (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Goldeen (Lv. 15-35, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Seaking (Lv. 30-55, 45%)

### Route 201
- **Grass/Land**:
  - *Morning*: Starly (Lv. 2-3, 50%), Bidoof (Lv. 2-3, 40%), Kricketot (Lv. 3, 10%)
  - *Day*: Starly (Lv. 2-3, 50%), Bidoof (Lv. 2-3, 50%)
  - *Night*: Bidoof (Lv. 2-3, 50%), Starly (Lv. 2-3, 40%), Kricketot (Lv. 3, 10%)
- **Swarm**: Doduo
- **Poke Radar**: Nidoran♀, Nidoran♂

### Verity Lakefront
- **Grass/Land**: Honchkrow (Lv. 50, 24%), Glalie (Lv. 50, 24%), Gallade (Lv. 50, 11%), Drifblim (Lv. 50, 10%), Kecleon (Lv. 50, 10%), Cacturne (Lv. 50, 10%), Grumpig (Lv. 50, 6%), Toxicroak (Lv. 50, 5%)
- **Swarm**: Doduo
- **Poke Radar**: Nidoran♀, Nidoran♂

### Lake Verity

#### Lake Verity
- **Grass/Land**: Starly (Lv. 2-4, 50%), Bidoof (Lv. 2-4, 50%)
- **Surf**: Psyduck (Lv. 20-30, 90%), Golduck (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Goldeen (Lv. 15-20, 40%), Seaking (Lv. 25-35, 5%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Seaking (Lv. 30-55, 45%)
- **Swarm**: Bidoof, Starly
- **Poke Radar**: Wobbuffet

#### Low Water
- **Grass/Land**:
  - *Morning/Day*: Starly (Lv. 2-4, 50%), Bidoof (Lv. 2-4, 50%)
  - *Night*: Bidoof (Lv. 2-4, 60%), Starly (Lv. 2-4, 40%)
- **Surf**: Psyduck (Lv. 20-30, 90%), Golduck (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Goldeen (Lv. 15-20, 40%), Seaking (Lv. 25-35, 5%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Seaking (Lv. 30-55, 45%)
- **Swarm**: Bidoof, Starly
- **Poke Radar**: Wobbuffet

### Route 202
- **Grass/Land**:
  - *Morning*: Bidoof (Lv. 2-4, 40%), Shinx (Lv. 3-4, 30%), Starly (Lv. 2-4, 20%), Kricketot (Lv. 3, 10%)
  - *Day*: Bidoof (Lv. 2-4, 50%), Shinx (Lv. 3-4, 30%), Starly (Lv. 2-4, 20%)
  - *Night*: Bidoof (Lv. 2-4, 50%), Shinx (Lv. 3-4, 30%), Kricketot (Lv. 4, 10%), Starly (Lv. 2-4, 10%)
- **Swarm**: Zigzagoon
- **Poke Radar**: Sentret

### Route 203
- **Grass/Land**:
  - *Morning*: Starly (Lv. 4-7, 35%), Shinx (Lv. 4-5, 25%), Bidoof (Lv. 5-7, 15%), Abra (Lv. 4-5, 15%), Kricketot (Lv. 4, 10%)
  - *Day*: Starly (Lv. 4-7, 35%), Shinx (Lv. 4-5, 25%), Bidoof (Lv. 4-7, 25%), Abra (Lv. 4-5, 15%)
  - *Night*: Starly (Lv. 4-7, 25%), Shinx (Lv. 4-5, 25%), Bidoof (Lv. 5-7, 15%), Abra (Lv. 4-5, 15%), Kricketot (Lv. 5, 10%), Zubat (Lv. 4, 10%)
- **Surf**: Psyduck (Lv. 20-30, 90%), Golduck (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Goldeen (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Seaking (Lv. 30-55, 45%)
- **Swarm**: Cubone
- **Poke Radar**: Abra, Bidoof, Starly

### Route 204 South
- **Grass/Land**:
  - *Morning*: Starly (Lv. 4-6, 25%), Bidoof (Lv. 4-6, 25%), Budew (Lv. 4-5, 15%), Shinx (Lv. 4-5, 15%), Wurmple (Lv. 4, 10%), Kricketot (Lv. 3, 10%)
  - *Day*: Starly (Lv. 4-6, 25%), Bidoof (Lv. 4-6, 25%), Budew (Lv. 3-5, 25%), Shinx (Lv. 4-5, 15%), Wurmple (Lv. 4, 10%)
  - *Night*: Starly (Lv. 4-6, 25%), Bidoof (Lv. 4-6, 25%), Budew (Lv. 4-5, 15%), Shinx (Lv. 4-5, 15%), Kricketot (Lv. 4, 10%), Zubat (Lv. 3, 10%)
- **Surf**: Psyduck (Lv. 20-30, 90%), Golduck (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Goldeen (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Seaking (Lv. 30-55, 45%)
- **Swarm**: Bidoof, Starly
- **Poke Radar**: Bidoof, Budew, Shinx, Starly

### Ravaged Path
- **Grass/Land**: Zubat (Lv. 3-6, 65%), Psyduck (Lv. 4-6, 35%)
- **Surf**: Psyduck (Lv. 20-30, 60%), Zubat (Lv. 20-30, 30%), Golduck (Lv. 20-40, 5%), Golbat (Lv. 20-40, 5%)
- **Rock Smash**: Geodude (Lv. 3-6, 100%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Barboach (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Whiscash (Lv. 30-55, 45%)
- **Swarm**: Psyduck, Zubat
- **Poke Radar**: Zubat

### Oreburgh Gate

#### 1F
- **Grass/Land**: Zubat (Lv. 5-8, 50%), Psyduck (Lv. 5-7, 35%), Geodude (Lv. 5-7, 15%)
- **Rock Smash**: Geodude (Lv. 5-8, 100%)
- **Swarm**: Psyduck, Zubat
- **Poke Radar**: Geodude, Zubat

#### B1F
- **Grass/Land**: Zubat (Lv. 6-9, 45%), Psyduck (Lv. 8-10, 35%), Geodude (Lv. 6-8, 15%), Golbat (Lv. 10, 5%)
- **Surf**: Psyduck (Lv. 20-30, 60%), Zubat (Lv. 20-30, 30%), Golduck (Lv. 20-40, 5%), Golbat (Lv. 20-40, 5%)
- **Rock Smash**: Geodude (Lv. 6-10, 100%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Barboach (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Whiscash (Lv. 30-55, 45%)
- **Swarm**: Psyduck, Zubat
- **Poke Radar**: Geodude, Golbat, Zubat

### Oreburgh Mine

#### B1F
- **Grass/Land**: Geodude (Lv. 4-8, 65%), Zubat (Lv. 5-7, 25%), Onix (Lv. 6-8, 10%)
- **Swarm**: Geodude
- **Poke Radar**: Geodude

#### B2F
- **Grass/Land**: Geodude (Lv. 5-9, 65%), Zubat (Lv. 6-8, 25%), Onix (Lv. 7-9, 10%)
- **Rock Smash**: Geodude (Lv. 5-9, 100%)
- **Swarm**: Geodude
- **Poke Radar**: Geodude

### Route 218
- **Grass/Land**:
  - *Morning/Day*: Floatzel (Lv. 29-31, 30%), Gastrodon (Lv. 28-30, 25%), Mr. Mime (Lv. 29-31, 25%), Chatot (Lv. 28-30, 20%)
  - *Night*: Floatzel (Lv. 29-31, 40%), Gastrodon (Lv. 28-30, 35%), Mr. Mime (Lv. 29-31, 25%)
- **Surf**: Tentacool (Lv. 20-30, 60%), Shellos (Lv. 20-30, 30%), Tentacruel (Lv. 20-40, 9%), Gastrodon (Lv. 20-40, 1%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Finneon (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Lumineon (Lv. 30-55, 45%)
- **Swarm**: Voltorb
- **Poke Radar**: Floatzel, Gastrodon, Mr. Mime

### Route 219
- **Surf**: Tentacool (Lv. 20-30, 60%), Wingull (Lv. 20-30, 30%), Tentacruel (Lv. 20-40, 9%), Pelipper (Lv. 20-40, 1%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Finneon (Lv. 15-20, 40%), Lumineon (Lv. 25-35, 5%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Lumineon (Lv. 30-55, 45%)

## Tier 2 — cap 23

### Route 204 North
- **Grass/Land**:
  - *Morning*: Starly (Lv. 9-11, 25%), Bidoof (Lv. 9-11, 25%), Budew (Lv. 9-10, 15%), Shinx (Lv. 9-10, 15%), Wurmple (Lv. 9, 10%), Kricketot (Lv. 8, 10%)
  - *Day*: Starly (Lv. 9-11, 25%), Bidoof (Lv. 9-11, 25%), Budew (Lv. 8-10, 25%), Shinx (Lv. 9-10, 15%), Wurmple (Lv. 9, 10%)
  - *Night*: Starly (Lv. 9-11, 25%), Bidoof (Lv. 9-11, 25%), Budew (Lv. 9-10, 15%), Shinx (Lv. 9-10, 15%), Kricketot (Lv. 9, 10%), Zubat (Lv. 8, 10%)
- **Surf**: Psyduck (Lv. 20-30, 90%), Golduck (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Goldeen (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Seaking (Lv. 30-55, 45%)
- **Swarm**: Bidoof, Starly
- **Poke Radar**: Sunkern

### Route 205 South
- **Grass/Land**: Shellos (Lv. 9-12, 65%), Buizel (Lv. 10-11, 15%), Bidoof (Lv. 10, 10%), Pachirisu (Lv. 9-11, 10%)
- **Surf**: Shellos (Lv. 20-30, 60%), Tentacool (Lv. 20-30, 30%), Gastrodon (Lv. 20-40, 9%), Tentacruel (Lv. 20-40, 1%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Finneon (Lv. 10-25, 45%)
- **Super Rod**: Lumineon (Lv. 30-55, 45%), Gyarados (Lv. 30-55, 40%), Shellder (Lv. 20-50, 15%)
- **Swarm**: Shellos
- **Poke Radar**: Hoppip

### Route 205 North
- **Grass/Land**:
  - *Morning*: Bidoof (Lv. 12-14, 30%), Budew (Lv. 12-14, 28%), Wurmple (Lv. 13, 10%), Kricketot (Lv. 12, 10%), Silcoon (Lv. 14, 10%), Cascoon (Lv. 14, 10%), Beautifly (Lv. 15, 1%), Dustox (Lv. 15, 1%)
  - *Day*: Budew (Lv. 12-14, 38%), Bidoof (Lv. 12-14, 30%), Wurmple (Lv. 13, 10%), Silcoon (Lv. 14, 10%), Cascoon (Lv. 14, 10%), Beautifly (Lv. 15, 1%), Dustox (Lv. 15, 1%)
  - *Night*: Bidoof (Lv. 12-14, 30%), Budew (Lv. 12-14, 28%), Kricketot (Lv. 13, 10%), Hoothoot (Lv. 12, 10%), Silcoon (Lv. 14, 10%), Cascoon (Lv. 14, 10%), Beautifly (Lv. 15, 1%), Dustox (Lv. 15, 1%)
- **Surf**: Psyduck (Lv. 20-30, 90%), Golduck (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Barboach (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Whiscash (Lv. 30-55, 45%)
- **Swarm**: Bidoof, Budew
- **Poke Radar**: Slowpoke

### Valley Windworks
- **Grass/Land**: Shellos (Lv. 9-12, 45%), Buizel (Lv. 9-11, 25%), Shinx (Lv. 10, 20%), Pachirisu (Lv. 9-11, 10%)
- **Surf**: Shellos (Lv. 20-30, 60%), Tentacool (Lv. 20-30, 30%), Gastrodon (Lv. 20-40, 9%), Tentacruel (Lv. 20-40, 1%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Finneon (Lv. 10-25, 45%)
- **Super Rod**: Lumineon (Lv. 30-55, 45%), Gyarados (Lv. 30-55, 40%), Shellder (Lv. 20-50, 15%)
- **Swarm**: Electrike
- **Poke Radar**: Mareep

### Eterna Forest
- **Grass/Land**:
  - *Morning*: Budew (Lv. 10-11, 30%), Buneary (Lv. 11-13, 24%), Wurmple (Lv. 10, 10%), Kricketot (Lv. 12, 10%), Bidoof (Lv. 12, 10%), Silcoon (Lv. 12, 5%), Cascoon (Lv. 12, 5%), Gastly (Lv. 13, 4%), Beautifly (Lv. 14, 1%), Dustox (Lv. 14, 1%)
  - *Day*: Budew (Lv. 10-12, 40%), Buneary (Lv. 11-13, 24%), Wurmple (Lv. 10, 10%), Bidoof (Lv. 12, 10%), Silcoon (Lv. 12, 5%), Cascoon (Lv. 12, 5%), Gastly (Lv. 13, 4%), Beautifly (Lv. 14, 1%), Dustox (Lv. 14, 1%)
  - *Night*: Budew (Lv. 10-11, 30%), Buneary (Lv. 11-13, 24%), Kricketot (Lv. 10, 10%), Hoothoot (Lv. 12, 10%), Bidoof (Lv. 12, 10%), Silcoon (Lv. 12, 5%), Cascoon (Lv. 12, 5%), Gastly (Lv. 13, 4%), Beautifly (Lv. 14, 1%), Dustox (Lv. 14, 1%)
- **Swarm**: Slakoth
- **Poke Radar**: Nincada

### Eterna City
- **Surf**: Psyduck (Lv. 20-30, 90%), Golduck (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Barboach (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Whiscash (Lv. 30-55, 45%)

### Route 211

#### West
- **Grass/Land**:
  - *Morning/Day*: Meditite (Lv. 13-15, 40%), Bidoof (Lv. 14, 20%), Chingling (Lv. 14-16, 15%), Machop (Lv. 14-15, 15%), Bronzor (Lv. 14-16, 10%)
  - *Night*: Meditite (Lv. 13, 20%), Bidoof (Lv. 14, 20%), Chingling (Lv. 14-16, 15%), Machop (Lv. 14-15, 15%), Zubat (Lv. 14, 10%), Hoothoot (Lv. 15, 10%), Bronzor (Lv. 14-16, 10%)
- **Rock Smash**: Geodude (Lv. 13-16, 100%)
- **Swarm**: Bidoof, Meditite
- **Poke Radar**: Tyrogue

#### East
- **Grass/Land**:
  - *Morning/Day*: Meditite (Lv. 27-29, 40%), Graveler (Lv. 28, 20%), Chingling (Lv. 28-30, 15%), Machoke (Lv. 29-30, 15%), Bronzor (Lv. 29, 10%)
  - *Night*: Meditite (Lv. 27, 20%), Graveler (Lv. 28, 20%), Chingling (Lv. 28-30, 15%), Machoke (Lv. 29-30, 15%), Zubat (Lv. 28, 10%), Noctowl (Lv. 29, 10%), Bronzor (Lv. 29, 10%)
- **Rock Smash**: Geodude (Lv. 27-30, 100%)
- **Swarm**: Graveler, Meditite
- **Poke Radar**: Bronzor, Chingling, Machoke

### Mt Coronet (Int)

#### 1F North Room 1
- **Grass/Land**:
  - *Morning*: Bronzor (Lv. 14, 20%), Geodude (Lv. 15, 20%), Meditite (Lv. 14-16, 20%), Cleffa (Lv. 13, 10%), Machop (Lv. 16, 10%), Chingling (Lv. 13-15, 10%), Nosepass (Lv. 14, 5%), Zubat (Lv. 15, 5%)
  - *Day*: Geodude (Lv. 13-15, 30%), Bronzor (Lv. 14, 20%), Meditite (Lv. 14-16, 20%), Machop (Lv. 16, 10%), Chingling (Lv. 13-15, 10%), Nosepass (Lv. 14, 5%), Zubat (Lv. 15, 5%)
  - *Night*: Bronzor (Lv. 14, 20%), Geodude (Lv. 15, 20%), Zubat (Lv. 14-15, 15%), Cleffa (Lv. 13, 10%), Machop (Lv. 16, 10%), Meditite (Lv. 16, 10%), Chingling (Lv. 13-15, 10%), Nosepass (Lv. 14, 5%)
- **Rock Smash**: Geodude (Lv. 13-16, 100%)
- **Swarm**: Bronzor, Geodude
- **Poke Radar**: Chingling, Machop, Meditite, Zubat

#### 1F North Room 2
- **Grass/Land**:
  - *Morning*: Bronzor (Lv. 33, 20%), Graveler (Lv. 34, 20%), Meditite (Lv. 33-35, 20%), Clefairy (Lv. 32, 10%), Machoke (Lv. 35, 10%), Chingling (Lv. 32-34, 10%), Nosepass (Lv. 33, 5%), Golbat (Lv. 34, 5%)
  - *Day*: Graveler (Lv. 32-34, 30%), Bronzor (Lv. 33, 20%), Meditite (Lv. 33-35, 20%), Machoke (Lv. 35, 10%), Chingling (Lv. 32-34, 10%), Nosepass (Lv. 33, 5%), Golbat (Lv. 34, 5%)
  - *Night*: Bronzor (Lv. 33, 20%), Graveler (Lv. 34, 20%), Golbat (Lv. 33-34, 15%), Clefairy (Lv. 32, 10%), Machoke (Lv. 35, 10%), Meditite (Lv. 35, 10%), Chingling (Lv. 32-34, 10%), Nosepass (Lv. 33, 5%)
- **Swarm**: Bronzor, Graveler
- **Poke Radar**: Chingling, Golbat, Machoke, Meditite

#### 1F South
- **Grass/Land**:
  - *Morning*: Bronzor (Lv. 18, 20%), Geodude (Lv. 19, 20%), Meditite (Lv. 18-20, 20%), Clefairy (Lv. 17, 10%), Machop (Lv. 20, 10%), Chingling (Lv. 17-19, 10%), Nosepass (Lv. 18, 5%), Zubat (Lv. 19, 5%)
  - *Day*: Geodude (Lv. 17-19, 30%), Bronzor (Lv. 18, 20%), Meditite (Lv. 18-20, 20%), Machop (Lv. 20, 10%), Chingling (Lv. 17-19, 10%), Nosepass (Lv. 18, 5%), Zubat (Lv. 19, 5%)
  - *Night*: Bronzor (Lv. 18, 20%), Geodude (Lv. 19, 20%), Zubat (Lv. 18-19, 15%), Clefairy (Lv. 17, 10%), Machop (Lv. 20, 10%), Meditite (Lv. 20, 10%), Chingling (Lv. 17-19, 10%), Nosepass (Lv. 18, 5%)
- **Surf**: Zubat (Lv. 20-30, 90%), Golbat (Lv. 20-40, 10%)
- **Rock Smash**: Geodude (Lv. 17-20, 100%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Barboach (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 20-55, 55%), Whiscash (Lv. 30-55, 45%)
- **Swarm**: Bronzor, Geodude
- **Poke Radar**: Chingling, Machop, Meditite, Zubat

#### 1F Tunnel Room
- **Grass/Land**:
  - *Morning*: Graveler (Lv. 37-38, 40%), Medicham (Lv. 37-39, 20%), Clefairy (Lv. 36, 10%), Machoke (Lv. 39, 10%), Chingling (Lv. 36-38, 10%), Nosepass (Lv. 37, 5%), Golbat (Lv. 38, 5%)
  - *Day*: Graveler (Lv. 36-38, 50%), Medicham (Lv. 37-39, 20%), Machoke (Lv. 39, 10%), Chingling (Lv. 36-38, 10%), Nosepass (Lv. 37, 5%), Golbat (Lv. 38, 5%)
  - *Night*: Graveler (Lv. 37-38, 40%), Golbat (Lv. 37-38, 15%), Clefairy (Lv. 36, 10%), Machoke (Lv. 39, 10%), Medicham (Lv. 39, 10%), Chingling (Lv. 36-38, 10%), Nosepass (Lv. 37, 5%)
- **Rock Smash**: Geodude (Lv. 36-39, 100%)
- **Swarm**: Graveler
- **Poke Radar**: Chingling, Golbat, Machoke, Medicham

#### 2F
- **Grass/Land**:
  - *Morning*: Bronzong (Lv. 37, 20%), Graveler (Lv. 38, 20%), Medicham (Lv. 37-39, 20%), Clefairy (Lv. 36, 10%), Machoke (Lv. 39, 10%), Chingling (Lv. 36-38, 10%), Nosepass (Lv. 37, 5%), Golbat (Lv. 38, 5%)
  - *Day*: Graveler (Lv. 36-38, 30%), Bronzong (Lv. 37, 20%), Medicham (Lv. 37-39, 20%), Machoke (Lv. 39, 10%), Chingling (Lv. 36-38, 10%), Nosepass (Lv. 37, 5%), Golbat (Lv. 38, 5%)
  - *Night*: Bronzong (Lv. 37, 20%), Graveler (Lv. 38, 20%), Golbat (Lv. 37-38, 15%), Clefairy (Lv. 36, 10%), Machoke (Lv. 39, 10%), Medicham (Lv. 39, 10%), Chingling (Lv. 36-38, 10%), Nosepass (Lv. 37, 5%)
- **Swarm**: Bronzong, Graveler
- **Poke Radar**: Chingling, Golbat, Machoke, Medicham

#### 3F
- **Grass/Land**:
  - *Morning*: Medicham (Lv. 37-39, 21%), Bronzong (Lv. 37, 20%), Graveler (Lv. 38, 20%), Clefairy (Lv. 36, 10%), Machoke (Lv. 39, 10%), Chingling (Lv. 36-38, 10%), Nosepass (Lv. 37, 5%), Golbat (Lv. 38, 4%)
  - *Day*: Graveler (Lv. 36-38, 30%), Medicham (Lv. 37-39, 21%), Bronzong (Lv. 37, 20%), Machoke (Lv. 39, 10%), Chingling (Lv. 36-38, 10%), Nosepass (Lv. 37, 5%), Golbat (Lv. 38, 4%)
  - *Night*: Bronzong (Lv. 37, 20%), Graveler (Lv. 38, 20%), Golbat (Lv. 37-38, 14%), Medicham (Lv. 38-39, 11%), Clefairy (Lv. 36, 10%), Machoke (Lv. 39, 10%), Chingling (Lv. 36-38, 10%), Nosepass (Lv. 37, 5%)
- **Swarm**: Bronzong, Graveler
- **Poke Radar**: Chingling, Machoke, Medicham

#### 4F Room 3
- **Grass/Land**:
  - *Morning*: Bronzong (Lv. 37, 20%), Graveler (Lv. 38, 20%), Medicham (Lv. 37-39, 20%), Clefairy (Lv. 36, 10%), Machoke (Lv. 39, 10%), Chingling (Lv. 36-38, 9%), Nosepass (Lv. 37, 5%), Golbat (Lv. 38, 5%), Chimecho (Lv. 39, 1%)
  - *Day*: Graveler (Lv. 36-38, 30%), Bronzong (Lv. 37, 20%), Medicham (Lv. 37-39, 20%), Machoke (Lv. 39, 10%), Chingling (Lv. 36-38, 9%), Nosepass (Lv. 37, 5%), Golbat (Lv. 38, 5%), Chimecho (Lv. 39, 1%)
  - *Night*: Bronzong (Lv. 37, 20%), Graveler (Lv. 38, 20%), Golbat (Lv. 37-38, 15%), Clefairy (Lv. 36, 10%), Machoke (Lv. 39, 10%), Medicham (Lv. 39, 10%), Chingling (Lv. 36-38, 9%), Nosepass (Lv. 37, 5%), Chimecho (Lv. 39, 1%)
- **Swarm**: Bronzong, Graveler
- **Poke Radar**: Chimecho, Golbat, Machoke, Medicham

#### 4F Rooms 1 and 2
- **Grass/Land**:
  - *Morning*: Bronzong (Lv. 37, 20%), Graveler (Lv. 38, 20%), Medicham (Lv. 37-39, 20%), Clefairy (Lv. 36, 10%), Machoke (Lv. 39, 10%), Chingling (Lv. 36-38, 10%), Nosepass (Lv. 37, 5%), Golbat (Lv. 38, 5%)
  - *Day*: Graveler (Lv. 36-38, 30%), Bronzong (Lv. 37, 20%), Medicham (Lv. 37-39, 20%), Machoke (Lv. 39, 10%), Chingling (Lv. 36-38, 10%), Nosepass (Lv. 37, 5%), Golbat (Lv. 38, 5%)
  - *Night*: Bronzong (Lv. 37, 20%), Graveler (Lv. 38, 20%), Golbat (Lv. 37-38, 15%), Clefairy (Lv. 36, 10%), Machoke (Lv. 39, 10%), Medicham (Lv. 39, 10%), Chingling (Lv. 36-38, 10%), Nosepass (Lv. 37, 5%)
- **Surf**: Zubat (Lv. 20-30, 90%), Golbat (Lv. 20-40, 10%)
- **Rock Smash**: Geodude (Lv. 36-39, 100%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Barboach (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 40%), Dratini (Lv. 15-25, 40%), Whiscash (Lv. 20-50, 15%), Dragonair (Lv. 20-55, 5%)
- **Swarm**: Bronzong, Graveler
- **Poke Radar**: Chingling, Golbat, Machoke, Medicham

#### 5F
- **Grass/Land**:
  - *Morning*: Bronzong (Lv. 37, 20%), Graveler (Lv. 38, 20%), Medicham (Lv. 37-39, 20%), Clefairy (Lv. 36, 10%), Machoke (Lv. 39, 10%), Chingling (Lv. 36, 5%), Nosepass (Lv. 37, 5%), Golbat (Lv. 38, 5%), Chimecho (Lv. 39-40, 5%)
  - *Day*: Graveler (Lv. 36-38, 30%), Bronzong (Lv. 37, 20%), Medicham (Lv. 37-39, 20%), Machoke (Lv. 39, 10%), Chingling (Lv. 36, 5%), Nosepass (Lv. 37, 5%), Golbat (Lv. 38, 5%), Chimecho (Lv. 39-40, 5%)
  - *Night*: Bronzong (Lv. 37, 20%), Graveler (Lv. 38, 20%), Golbat (Lv. 37-38, 15%), Clefairy (Lv. 36, 10%), Machoke (Lv. 39, 10%), Medicham (Lv. 39, 10%), Chingling (Lv. 36, 5%), Nosepass (Lv. 37, 5%), Chimecho (Lv. 39-40, 5%)
- **Swarm**: Bronzong, Graveler
- **Poke Radar**: Chimecho, Golbat, Machoke, Medicham

#### 6F
- **Grass/Land**:
  - *Morning*: Bronzong (Lv. 37, 20%), Graveler (Lv. 38, 20%), Medicham (Lv. 37-39, 20%), Clefairy (Lv. 36, 10%), Machoke (Lv. 39, 10%), Chimecho (Lv. 39-41, 10%), Nosepass (Lv. 37, 5%), Golbat (Lv. 38, 5%)
  - *Day*: Graveler (Lv. 36-38, 30%), Bronzong (Lv. 37, 20%), Medicham (Lv. 37-39, 20%), Machoke (Lv. 39, 10%), Chimecho (Lv. 39-41, 10%), Nosepass (Lv. 37, 5%), Golbat (Lv. 38, 5%)
  - *Night*: Bronzong (Lv. 37, 20%), Graveler (Lv. 38, 20%), Golbat (Lv. 37-38, 15%), Clefairy (Lv. 36, 10%), Machoke (Lv. 39, 10%), Medicham (Lv. 39, 10%), Chimecho (Lv. 39-41, 10%), Nosepass (Lv. 37, 5%)
- **Swarm**: Bronzong, Graveler
- **Poke Radar**: Chimecho, Golbat, Machoke, Medicham

#### B1F
- **Grass/Land**:
  - *Morning*: Bronzor (Lv. 33, 20%), Graveler (Lv. 34, 20%), Meditite (Lv. 33-35, 20%), Clefairy (Lv. 32, 10%), Machoke (Lv. 35, 10%), Chingling (Lv. 32-34, 10%), Nosepass (Lv. 33, 5%), Golbat (Lv. 34, 5%)
  - *Day*: Graveler (Lv. 32-34, 30%), Bronzor (Lv. 33, 20%), Meditite (Lv. 33-35, 20%), Machoke (Lv. 35, 10%), Chingling (Lv. 32-34, 10%), Nosepass (Lv. 33, 5%), Golbat (Lv. 34, 5%)
  - *Night*: Bronzor (Lv. 33, 20%), Graveler (Lv. 34, 20%), Golbat (Lv. 33-34, 15%), Clefairy (Lv. 32, 10%), Machoke (Lv. 35, 10%), Meditite (Lv. 35, 10%), Chingling (Lv. 32-34, 10%), Nosepass (Lv. 33, 5%)
- **Surf**: Zubat (Lv. 20-30, 90%), Golbat (Lv. 20-40, 10%)
- **Rock Smash**: Geodude (Lv. 32-35, 100%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Barboach (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Whiscash (Lv. 30-55, 45%)
- **Swarm**: Bronzor, Graveler
- **Poke Radar**: Chingling, Golbat, Machoke, Meditite

## Tier 3 — cap 30

### Old Chateau

#### Main Room
- **Grass/Land**: Gastly (Lv. 14-17, 100%)
- **Swarm**: Gastly
- **Poke Radar**: Gastly

#### Corridor
- **Grass/Land**: Gastly (Lv. 14-17, 100%)
- **Swarm**: Gastly
- **Poke Radar**: Gastly

#### Dining Area
- **Grass/Land**: Gastly (Lv. 14-17, 100%)
- **Swarm**: Gastly
- **Poke Radar**: Gastly

#### Side Rooms
- **Grass/Land**: Gastly (Lv. 14-17, 100%)
- **Swarm**: Gastly
- **Poke Radar**: Gastly

#### Back West Room
- **Grass/Land**: Gastly (Lv. 14-17, 100%)
- **Swarm**: Gastly
- **Poke Radar**: Gastly

#### Back Middle-West Room
- **Grass/Land**: Gastly (Lv. 14-17, 100%)
- **Swarm**: Gastly
- **Poke Radar**: Gastly

#### Back Middle Room
- **Grass/Land**: Gastly (Lv. 14-17, 100%)
- **Swarm**: Gastly
- **Poke Radar**: Gastly

#### Back Middle-East Room
- **Grass/Land**: Gastly (Lv. 14-17, 100%)
- **Swarm**: Gastly
- **Poke Radar**: Gastly

#### Back East Room
- **Grass/Land**: Gastly (Lv. 14-17, 100%)
- **Swarm**: Gastly
- **Poke Radar**: Gastly

### Route 206
- **Grass/Land**:
  - *Morning*: Geodude (Lv. 16-18, 30%), Ponyta (Lv. 16, 20%), Gligar (Lv. 16-18, 20%), Machop (Lv. 17-19, 20%), Kricketune (Lv. 17, 10%)
  - *Day*: Ponyta (Lv. 16-17, 30%), Geodude (Lv. 16-18, 30%), Gligar (Lv. 16-18, 20%), Machop (Lv. 17-19, 20%)
  - *Night*: Geodude (Lv. 16-18, 30%), Ponyta (Lv. 16, 20%), Machop (Lv. 17-19, 20%), Kricketune (Lv. 18, 10%), Zubat (Lv. 17, 10%), Gligar (Lv. 16, 10%)
- **Swarm**: Larvitar
- **Poke Radar**: Baltoy

### Wayward Cave

#### 1F
- **Grass/Land**: Geodude (Lv. 17-20, 40%), Bronzor (Lv. 18-20, 30%), Zubat (Lv. 17-19, 15%), Onix (Lv. 18-20, 15%)
- **Rock Smash**: Geodude (Lv. 17-20, 100%)
- **Swarm**: Bronzor, Geodude
- **Poke Radar**: Bronzor, Geodude, Onix

#### B1F
- **Grass/Land**: Bronzor (Lv. 18-20, 30%), Geodude (Lv. 18-20, 25%), Gible (Lv. 17-20, 20%), Onix (Lv. 18-20, 15%), Zubat (Lv. 19, 10%)
- **Swarm**: Bronzor, Geodude
- **Poke Radar**: Bronzor, Gible, Onix

### Route 207
- **Grass/Land**:
  - *Morning*: Machop (Lv. 6-8, 35%), Geodude (Lv. 5-7, 30%), Ponyta (Lv. 5-7, 25%), Kricketot (Lv. 5, 10%)
  - *Day*: Machop (Lv. 5-8, 45%), Geodude (Lv. 5-7, 30%), Ponyta (Lv. 5-7, 25%)
  - *Night*: Machop (Lv. 6-8, 35%), Geodude (Lv. 5-7, 30%), Ponyta (Lv. 5-7, 15%), Kricketot (Lv. 6, 10%), Zubat (Lv. 5, 10%)
- **Swarm**: Phanpy
- **Poke Radar**: Stantler

### Route 208
- **Grass/Land**:
  - *Morning/Day*: Budew (Lv. 18-19, 30%), Bidoof (Lv. 18, 20%), Bibarel (Lv. 18-20, 20%), Ralts (Lv. 17-18, 15%), Roselia (Lv. 19-20, 15%)
  - *Night*: Budew (Lv. 18, 20%), Bidoof (Lv. 18, 20%), Bibarel (Lv. 18-20, 20%), Ralts (Lv. 17-18, 15%), Roselia (Lv. 19-20, 15%), Zubat (Lv. 19, 10%)
- **Surf**: Psyduck (Lv. 20, 90%), Golduck (Lv. 20-40, 10%)
- **Rock Smash**: Geodude (Lv. 17-20, 100%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Goldeen (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Seaking (Lv. 30-55, 45%)
- **Swarm**: Dunsparce
- **Poke Radar**: Smeargle

## Tier 4 — cap 37

### Route 209
- **Grass/Land**:
  - *Morning/Day*: Bibarel (Lv. 18-19, 30%), Roselia (Lv. 19-20, 25%), Staravia (Lv. 18-19, 20%), Ralts (Lv. 17-19, 20%), Chansey (Lv. 17-19, 5%)
  - *Night*: Bibarel (Lv. 18-19, 30%), Roselia (Lv. 19-20, 25%), Zubat (Lv. 19, 10%), Duskull (Lv. 17, 10%), Staravia (Lv. 18, 10%), Ralts (Lv. 18-19, 10%), Chansey (Lv. 17-19, 5%)
- **Surf**: Psyduck (Lv. 20-30, 90%), Golduck (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Goldeen (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Seaking (Lv. 30-55, 45%)
- **Swarm**: Snubbull
- **Poke Radar**: Kirlia

### Lost Tower

#### 1F
- **Grass/Land**:
  - *Morning/Day*: Gastly (Lv. 17-20, 65%), Zubat (Lv. 17-19, 35%)
  - *Night*: Gastly (Lv. 17-20, 45%), Zubat (Lv. 17-19, 35%), Duskull (Lv. 17-19, 20%)
- **Swarm**: Gastly, Zubat
- **Poke Radar**: Gastly, Zubat

#### 2F
- **Grass/Land**:
  - *Morning/Day*: Gastly (Lv. 17-20, 65%), Zubat (Lv. 17-19, 35%)
  - *Night*: Gastly (Lv. 17-20, 45%), Zubat (Lv. 17-19, 35%), Duskull (Lv. 17-19, 20%)
- **Swarm**: Gastly, Zubat
- **Poke Radar**: Gastly, Zubat

#### 3F
- **Grass/Land**:
  - *Morning/Day*: Gastly (Lv. 18-21, 64%), Zubat (Lv. 18-20, 35%), Golbat (Lv. 21, 1%)
  - *Night*: Gastly (Lv. 18-21, 44%), Zubat (Lv. 18-20, 35%), Duskull (Lv. 18-20, 20%), Golbat (Lv. 21, 1%)
- **Swarm**: Gastly, Zubat
- **Poke Radar**: Gastly, Zubat

#### 4F
- **Grass/Land**:
  - *Morning/Day*: Gastly (Lv. 18-21, 60%), Zubat (Lv. 18-20, 35%), Golbat (Lv. 21, 5%)
  - *Night*: Gastly (Lv. 18-21, 40%), Zubat (Lv. 18-20, 35%), Duskull (Lv. 18-20, 20%), Golbat (Lv. 21, 5%)
- **Swarm**: Gastly, Zubat
- **Poke Radar**: Gastly, Zubat

#### 5F
- **Grass/Land**:
  - *Morning/Day*: Gastly (Lv. 19-22, 55%), Zubat (Lv. 19-21, 35%), Golbat (Lv. 22, 10%)
  - *Night*: Gastly (Lv. 19-22, 35%), Zubat (Lv. 19-21, 35%), Duskull (Lv. 19-21, 20%), Golbat (Lv. 22, 10%)
- **Swarm**: Gastly, Zubat
- **Poke Radar**: Gastly, Zubat

### Route 210 South
- **Grass/Land**:
  - *Morning*: Ponyta (Lv. 19-21, 25%), Staravia (Lv. 19, 20%), Geodude (Lv. 18, 20%), Scyther (Lv. 19-21, 15%), Roselia (Lv. 20-21, 15%), Chansey (Lv. 19-21, 5%)
  - *Day*: Ponyta (Lv. 19-21, 35%), Staravia (Lv. 19, 20%), Geodude (Lv. 18, 20%), Roselia (Lv. 20-21, 15%), Scyther (Lv. 19, 5%), Chansey (Lv. 19-21, 5%)
  - *Night*: Staravia (Lv. 19, 20%), Geodude (Lv. 18, 20%), Roselia (Lv. 20-21, 15%), Ponyta (Lv. 19-21, 15%), Noctowl (Lv. 21, 10%), Hoothoot (Lv. 20, 10%), Scyther (Lv. 19, 5%), Chansey (Lv. 19-21, 5%)
- **Swarm**: Geodude, Staravia
- **Poke Radar**: Miltank, Tauros

### Solaceon Ruins

#### Room 1 NW Dead End
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 1 SE Dead End
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 2
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 2 NE Dead End
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 2 SE Dead End
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 3
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 3 NW Dead End
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 3 SW Dead End
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 4
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 4 SE Dead End
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 5
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 5 SE Dead End
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 5 SW Dead End
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 6
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 6 NW Dead End
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 6 SE Dead End
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

#### Room 7
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

### Route 215
- **Grass/Land**:
  - *Morning*: Staravia (Lv. 19-21, 30%), Marill (Lv. 20-22, 25%), Scyther (Lv. 20-22, 15%), Lickitung (Lv. 20, 10%), Abra (Lv. 19, 10%), Kadabra (Lv. 21-22, 10%)
  - *Day*: Staravia (Lv. 19-22, 40%), Marill (Lv. 20-22, 25%), Lickitung (Lv. 20, 10%), Abra (Lv. 19, 10%), Kadabra (Lv. 21-22, 10%), Scyther (Lv. 20, 5%)
  - *Night*: Marill (Lv. 20-22, 45%), Staravia (Lv. 19, 20%), Lickitung (Lv. 20, 10%), Abra (Lv. 19, 10%), Kadabra (Lv. 21-22, 10%), Scyther (Lv. 20, 5%)
- **Swarm**: Drowzee
- **Poke Radar**: Abra, Kadabra, Lickitung, Marill

## Tier 5 — cap 45

### Route 212 North
- **Grass/Land**:
  - *Morning*: Roselia (Lv. 22-24, 35%), Marill (Lv. 21-23, 25%), Staravia (Lv. 21-23, 20%), Kirlia (Lv. 22-24, 20%)
  - *Day*: Roselia (Lv. 22-24, 35%), Marill (Lv. 21-23, 25%), Staravia (Lv. 21-23, 20%), Ralts (Lv. 22, 10%), Kirlia (Lv. 24, 10%)
  - *Night*: Marill (Lv. 21-23, 45%), Roselia (Lv. 22-24, 35%), Staravia (Lv. 21, 10%), Kirlia (Lv. 24, 10%)
- **Surf**: Psyduck (Lv. 20-30, 90%), Golduck (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Goldeen (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Seaking (Lv. 30-55, 45%)
- **Swarm**: Marill, Roselia
- **Poke Radar**: Smeargle

### Route 212 South
- **Grass/Land**: Shellos (Lv. 23-26, 45%), Quagsire (Lv. 24-26, 30%), Buizel (Lv. 23-25, 15%), Croagunk (Lv. 24-25, 10%)
- **Surf**: Shellos (Lv. 20-30, 60%), Tentacool (Lv. 20-30, 30%), Gastrodon (Lv. 20-40, 9%), Tentacruel (Lv. 20-40, 1%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Remoraid (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Octillery (Lv. 30-55, 45%)
- **Swarm**: Quagsire, Shellos
- **Poke Radar**: Grimer

### Trophy Garden
- **Grass/Land**:
  - *Morning*: Pichu (Lv. 21-22, 30%), Roselia (Lv. 22-23, 30%), Staravia (Lv. 22-24, 20%), Kricketune (Lv. 23, 10%), Pikachu (Lv. 22-24, 10%)
  - *Day*: Pichu (Lv. 21-22, 30%), Roselia (Lv. 22-23, 30%), Staravia (Lv. 22-24, 30%), Pikachu (Lv. 22-24, 10%)
  - *Night*: Pichu (Lv. 21-22, 30%), Roselia (Lv. 22-23, 30%), Kricketune (Lv. 22-23, 20%), Staravia (Lv. 24, 10%), Pikachu (Lv. 22-24, 10%)
- **Swarm**: Pichu, Roselia
- **Poke Radar**: Pichu, Pikachu, Roselia, Staravia

### Route 214
- **Grass/Land**:
  - *Morning/Day*: Graveler (Lv. 22-24, 35%), Rhyhorn (Lv. 21-24, 30%), Geodude (Lv. 21, 20%), Houndour (Lv. 23-24, 15%)
  - *Night*: Graveler (Lv. 23-24, 25%), Houndour (Lv. 22-24, 25%), Geodude (Lv. 21, 20%), Rhyhorn (Lv. 21-24, 20%), Zubat (Lv. 22, 10%)
- **Surf**: Psyduck (Lv. 20-30, 90%), Golduck (Lv. 20-40, 10%)
- **Rock Smash**: Geodude (Lv. 21-24, 100%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Goldeen (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Seaking (Lv. 30-55, 45%)
- **Swarm**: Spoink
- **Poke Radar**: Poochyena

### Ruin Maniac Cave / Maniac Tunnel

#### Ruin Maniac Cave (Long)
- **Grass/Land**: Geodude (Lv. 22-24, 90%), Hippopotas (Lv. 23-25, 10%)
- **Swarm**: Geodude
- **Poke Radar**: Geodude, Hippopotas

#### Ruin Maniac Cave (Short)
- **Grass/Land**: Geodude (Lv. 21-23, 95%), Hippopotas (Lv. 22-24, 5%)
- **Swarm**: Geodude
- **Poke Radar**: Geodude, Hippopotas

#### Maniac Tunnel
- **Grass/Land**: Geodude (Lv. 23-25, 80%), Hippopotas (Lv. 24-26, 20%)
- **Swarm**: Geodude
- **Poke Radar**: Geodude, Hippopotas

#### Maniac Tunnel Room
- **Grass/Land**: Unown (Lv. 20-30, 100%)
- **Swarm**: Unown
- **Poke Radar**: Unown

### Valor Lakefront
- **Grass/Land**:
  - *Morning*: Bibarel (Lv. 25-27, 30%), Girafarig (Lv. 26-28, 25%), Staravia (Lv. 26-28, 25%), Kricketune (Lv. 27, 10%), Houndour (Lv. 28, 10%)
  - *Day*: Staravia (Lv. 26-28, 35%), Bibarel (Lv. 25-27, 30%), Girafarig (Lv. 26-28, 25%), Houndour (Lv. 28, 10%)
  - *Night*: Bibarel (Lv. 25-27, 30%), Girafarig (Lv. 26-28, 25%), Houndour (Lv. 27-28, 20%), Staravia (Lv. 27-28, 15%), Kricketune (Lv. 26, 10%)
- **Swarm**: Bibarel, Girafarig
- **Poke Radar**: Nidorina, Nidorino

### Route 213
- **Grass/Land**:
  - *Morning/Day*: Shellos (Lv. 24-26, 35%), Buizel (Lv. 23-25, 25%), Chatot (Lv. 23-25, 20%), Wingull (Lv. 24-26, 20%)
  - *Night*: Shellos (Lv. 23-26, 45%), Buizel (Lv. 23-25, 35%), Wingull (Lv. 24-26, 20%)
- **Surf**: Tentacool (Lv. 20-30, 60%), Wingull (Lv. 20-30, 30%), Tentacruel (Lv. 20-40, 5%), Shellos (Lv. 20-30, 4%), Gastrodon (Lv. 20-40, 1%)
- **Rock Smash**: Geodude (Lv. 23-26, 100%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Remoraid (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Octillery (Lv. 30-55, 45%)
- **Swarm**: Buizel, Shellos
- **Poke Radar**: Swellow

### Pastoria City
- **Surf**: Tentacool (Lv. 20-30, 60%), Shellos (Lv. 20-30, 30%), Tentacruel (Lv. 20-40, 5%), Wingull (Lv. 20-30, 4%), Gastrodon (Lv. 20-40, 1%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Remoraid (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Octillery (Lv. 30-55, 45%)

### Great Marsh

#### Area 1
- **Grass/Land**:
  - *Morning/Day*: Wooper (Lv. 28-30, 40%), Bibarel (Lv. 28, 20%), Tropius (Lv. 28-30, 20%), Quagsire (Lv. 30, 10%), Yanma (Lv. 30-31, 5%), Tangela (Lv. 30-31, 5%)
  - *Night*: Wooper (Lv. 28-30, 40%), Bibarel (Lv. 28, 20%), Noctowl (Lv. 28-30, 20%), Quagsire (Lv. 30, 10%), Yanma (Lv. 30-31, 5%), Tangela (Lv. 30-31, 5%)
- **Surf**: Wooper (Lv. 20-30, 90%), Quagsire (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Barboach (Lv. 10-25, 45%)
- **Super Rod**: Carvanha (Lv. 30-55, 45%), Gyarados (Lv. 30-55, 40%), Whiscash (Lv. 20-50, 15%)
- **Swarm**: Bibarel, Wooper
- **Poke Radar**: Quagsire, Tangela, Wooper, Yanma

#### Area 2
- **Grass/Land**:
  - *Morning/Day*: Wooper (Lv. 28-30, 40%), Bibarel (Lv. 28, 20%), Tropius (Lv. 28-30, 20%), Quagsire (Lv. 30, 10%), Yanma (Lv. 30-31, 5%), Tangela (Lv. 30-31, 5%)
  - *Night*: Wooper (Lv. 28-30, 40%), Bibarel (Lv. 28, 20%), Noctowl (Lv. 28-30, 20%), Quagsire (Lv. 30, 10%), Yanma (Lv. 30-31, 5%), Tangela (Lv. 30-31, 5%)
- **Surf**: Wooper (Lv. 20-30, 90%), Quagsire (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Barboach (Lv. 10-25, 45%)
- **Super Rod**: Carvanha (Lv. 30-55, 45%), Gyarados (Lv. 30-55, 40%), Whiscash (Lv. 20-50, 15%)
- **Swarm**: Bibarel, Wooper
- **Poke Radar**: Quagsire, Tangela, Wooper, Yanma

#### Area 3
- **Grass/Land**:
  - *Morning/Day*: Wooper (Lv. 27-29, 30%), Bibarel (Lv. 27-28, 30%), Tangela (Lv. 27-30, 25%), Quagsire (Lv. 29, 10%), Yanma (Lv. 29-30, 5%)
  - *Night*: Wooper (Lv. 27-29, 30%), Bibarel (Lv. 27, 20%), Tangela (Lv. 28-30, 15%), Noctowl (Lv. 28, 10%), Hoothoot (Lv. 27, 10%), Quagsire (Lv. 29, 10%), Yanma (Lv. 29-30, 5%)
- **Surf**: Wooper (Lv. 20-30, 90%), Quagsire (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Barboach (Lv. 10-25, 45%)
- **Super Rod**: Carvanha (Lv. 30-55, 45%), Gyarados (Lv. 30-55, 40%), Whiscash (Lv. 20-50, 15%)
- **Swarm**: Bibarel, Wooper
- **Poke Radar**: Quagsire, Tangela, Yanma

#### Area 4
- **Grass/Land**:
  - *Morning/Day*: Wooper (Lv. 27-29, 30%), Bibarel (Lv. 27-28, 30%), Tangela (Lv. 27-30, 25%), Quagsire (Lv. 29, 10%), Yanma (Lv. 29-30, 5%)
  - *Night*: Wooper (Lv. 27-29, 30%), Bibarel (Lv. 27, 20%), Tangela (Lv. 28-30, 15%), Noctowl (Lv. 28, 10%), Hoothoot (Lv. 27, 10%), Quagsire (Lv. 29, 10%), Yanma (Lv. 29-30, 5%)
- **Surf**: Wooper (Lv. 20-30, 90%), Quagsire (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Barboach (Lv. 10-25, 45%)
- **Super Rod**: Carvanha (Lv. 30-55, 45%), Gyarados (Lv. 30-55, 40%), Whiscash (Lv. 20-50, 15%)
- **Swarm**: Bibarel, Wooper
- **Poke Radar**: Quagsire, Tangela, Yanma

#### Area 5
- **Grass/Land**:
  - *Morning/Day*: Wooper (Lv. 26-28, 30%), Bibarel (Lv. 26-27, 30%), Yanma (Lv. 26-29, 25%), Quagsire (Lv. 28, 10%), Tangela (Lv. 28-29, 5%)
  - *Night*: Wooper (Lv. 26-28, 30%), Bibarel (Lv. 26, 20%), Hoothoot (Lv. 26-27, 20%), Yanma (Lv. 27-29, 15%), Quagsire (Lv. 28, 10%), Tangela (Lv. 28-29, 5%)
- **Surf**: Wooper (Lv. 20-30, 90%), Quagsire (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Barboach (Lv. 10-25, 45%)
- **Super Rod**: Carvanha (Lv. 30-55, 45%), Gyarados (Lv. 30-55, 40%), Whiscash (Lv. 20-50, 15%)
- **Swarm**: Bibarel, Wooper
- **Poke Radar**: Quagsire, Tangela, Yanma

#### Area 6
- **Grass/Land**:
  - *Morning/Day*: Wooper (Lv. 26-28, 30%), Bibarel (Lv. 26-27, 30%), Yanma (Lv. 26-29, 25%), Quagsire (Lv. 28, 10%), Tangela (Lv. 28-29, 5%)
  - *Night*: Wooper (Lv. 26-28, 30%), Bibarel (Lv. 26, 20%), Hoothoot (Lv. 26-27, 20%), Yanma (Lv. 27-29, 15%), Quagsire (Lv. 28, 10%), Tangela (Lv. 28-29, 5%)
- **Surf**: Wooper (Lv. 20-30, 90%), Quagsire (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Barboach (Lv. 10-25, 45%)
- **Super Rod**: Carvanha (Lv. 30-55, 45%), Gyarados (Lv. 30-55, 40%), Whiscash (Lv. 20-50, 15%)
- **Swarm**: Bibarel, Wooper
- **Poke Radar**: Quagsire, Tangela, Yanma

## Tier 6 — cap 51

### Fuego Ironworks
- **Grass/Land**:
  - *Morning/Day*: Magmar (Lv. 28-29, 30%), Magnemite (Lv. 28-30, 30%), Floatzel (Lv. 29-31, 25%), Gastrodon (Lv. 30-31, 15%)
  - *Night*: Magnemite (Lv. 28-30, 30%), Gastrodon (Lv. 29-31, 25%), Floatzel (Lv. 29-31, 25%), Magmar (Lv. 28, 20%)
- **Surf**: Tentacool (Lv. 20-30, 60%), Shellos (Lv. 20-30, 30%), Tentacruel (Lv. 20-40, 9%), Gastrodon (Lv. 20-40, 1%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Finneon (Lv. 10-25, 45%)
- **Super Rod**: Lumineon (Lv. 30-55, 45%), Gyarados (Lv. 30-55, 40%), Shellder (Lv. 20-50, 15%)
- **Swarm**: Magmar, Magnemite
- **Poke Radar**: Aron

### Route 220
- **Surf**: Tentacool (Lv. 20-30, 60%), Wingull (Lv. 20-30, 30%), Tentacruel (Lv. 20-40, 9%), Pelipper (Lv. 20-40, 1%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Finneon (Lv. 15-20, 40%), Lumineon (Lv. 25-35, 5%)
- **Super Rod**: Lumineon (Lv. 30-55, 45%), Gyarados (Lv. 30-55, 40%), Chinchou (Lv. 20-50, 15%)

### Route 221
- **Grass/Land**:
  - *Morning/Day*: Floatzel (Lv. 29-31, 25%), Girafarig (Lv. 28-30, 25%), Roselia (Lv. 28-30, 25%), Sudowoodo (Lv. 29-31, 25%)
  - *Night*: Floatzel (Lv. 28-31, 35%), Girafarig (Lv. 28-30, 25%), Sudowoodo (Lv. 29-31, 25%), Roselia (Lv. 29-30, 15%)
- **Surf**: Tentacool (Lv. 20-30, 60%), Wingull (Lv. 20-30, 30%), Tentacruel (Lv. 20-40, 9%), Pelipper (Lv. 20-40, 1%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Finneon (Lv. 15-20, 40%), Lumineon (Lv. 25-35, 5%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Lumineon (Lv. 30-55, 45%)
- **Swarm**: Farfetch’D
- **Poke Radar**: Nidorina, Nidorino

### Celestic Town
- **Surf**: Psyduck (Lv. 20-30, 90%), Golduck (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Goldeen (Lv. 15-35, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 40%), Corphish (Lv. 30-40, 40%), Crawdaunt (Lv. 40-55, 15%), Seaking (Lv. 40-55, 5%)

### Canalave City
- **Surf**: Tentacool (Lv. 20-30, 60%), Shellos (Lv. 20-30, 30%), Tentacruel (Lv. 20-40, 9%), Gastrodon (Lv. 20-40, 1%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Finneon (Lv. 10-25, 45%)
- **Super Rod**: Lumineon (Lv. 30-55, 45%), Gyarados (Lv. 30-55, 40%), Staryu (Lv. 20-50, 15%)

### Iron Island

#### Entrance
- **Surf**: Wingull (Lv. 20-30, 60%), Tentacool (Lv. 20-30, 30%), Pelipper (Lv. 20-40, 9%), Tentacruel (Lv. 20-40, 1%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Finneon (Lv. 10-25, 45%)
- **Super Rod**: Lumineon (Lv. 30-55, 45%), Gyarados (Lv. 30-55, 40%), Qwilfish (Lv. 20-50, 15%)

#### 1F
- **Grass/Land**: Geodude (Lv. 30-32, 35%), Graveler (Lv. 31-33, 35%), Zubat (Lv. 30, 10%), Golbat (Lv. 32, 10%), Onix (Lv. 31-33, 10%)
- **Swarm**: Geodude, Graveler
- **Poke Radar**: Geodude, Graveler

#### B1F Left Room
- **Grass/Land**: Graveler (Lv. 30-33, 70%), Golbat (Lv. 30-32, 20%), Onix (Lv. 31-33, 10%)
- **Swarm**: Graveler
- **Poke Radar**: Graveler

#### B1F Right Room
- **Grass/Land**: Graveler (Lv. 30-33, 70%), Golbat (Lv. 30-32, 20%), Onix (Lv. 31-33, 10%)
- **Swarm**: Graveler
- **Poke Radar**: Graveler

#### B2F Left Room
- **Grass/Land**: Graveler (Lv. 31-34, 50%), Onix (Lv. 32, 20%), Golbat (Lv. 31-33, 20%), Steelix (Lv. 33-35, 10%)
- **Swarm**: Graveler, Onix
- **Poke Radar**: Graveler

#### B2F Right Room
- **Grass/Land**: Graveler (Lv. 31-34, 50%), Onix (Lv. 32, 20%), Golbat (Lv. 31-33, 20%), Steelix (Lv. 33-35, 10%)
- **Swarm**: Graveler, Onix
- **Poke Radar**: Graveler

#### B3F
- **Grass/Land**: Graveler (Lv. 31-34, 50%), Steelix (Lv. 32-35, 30%), Golbat (Lv. 31-33, 20%)
- **Swarm**: Graveler, Onix
- **Poke Radar**: Graveler

## Tier 7 — cap 57

### Lake Valor
- **Grass/Land**: Staravia (Lv. 38-40, 35%), Bibarel (Lv. 39-41, 35%), Golduck (Lv. 40-41, 15%), Psyduck (Lv. 38-39, 15%)
- **Surf**: Psyduck (Lv. 20-30, 90%), Golduck (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Goldeen (Lv. 15-20, 40%), Seaking (Lv. 25-35, 5%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Seaking (Lv. 30-55, 45%)
- **Swarm**: Bibarel, Staravia
- **Poke Radar**: Wobbuffet

### Route 216
- **Grass/Land**:
  - *Morning/Day*: Snover (Lv. 32-35, 40%), Sneasel (Lv. 33-35, 35%), Meditite (Lv. 32-33, 20%), Graveler (Lv. 35, 5%)
  - *Night*: Sneasel (Lv. 33-35, 35%), Snover (Lv. 32-35, 30%), Zubat (Lv. 32, 10%), Snorunt (Lv. 33, 10%), Meditite (Lv. 33, 10%), Graveler (Lv. 35, 5%)
- **Swarm**: Sneasel, Snover
- **Poke Radar**: Graveler, Meditite, Sneasel, Snover

### Route 217
- **Grass/Land**:
  - *Morning/Day*: Snover (Lv. 32-35, 40%), Swinub (Lv. 32-34, 35%), Sneasel (Lv. 33-35, 25%)
  - *Night*: Swinub (Lv. 32-34, 35%), Snover (Lv. 32-35, 30%), Snorunt (Lv. 33, 20%), Sneasel (Lv. 34-35, 15%)
- **Swarm**: Delibird
- **Poke Radar**: Piloswine

### Acuity Lakefront
- **Grass/Land**:
  - *Morning/Day*: Snover (Lv. 32-35, 40%), Swinub (Lv. 32-34, 35%), Sneasel (Lv. 33-35, 25%)
  - *Night*: Swinub (Lv. 32-34, 35%), Snover (Lv. 33-35, 30%), Snorunt (Lv. 32-33, 20%), Sneasel (Lv. 35, 15%)
- **Swarm**: Snover, Swinub
- **Poke Radar**: Sneasel, Snover, Swinub

### Lake Acuity
- **Grass/Land**:
  - *Morning/Day*: Snover (Lv. 38-41, 45%), Bibarel (Lv. 39-40, 25%), Golduck (Lv. 39-40, 15%), Sneasel (Lv. 41, 10%), Psyduck (Lv. 38, 5%)
  - *Night*: Snover (Lv. 38-41, 25%), Bibarel (Lv. 39-40, 25%), Snorunt (Lv. 39-40, 20%), Golduck (Lv. 39-40, 15%), Sneasel (Lv. 41, 10%), Psyduck (Lv. 38, 5%)
- **Surf**: Psyduck (Lv. 20-30, 90%), Golduck (Lv. 20-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Goldeen (Lv. 15-20, 40%), Seaking (Lv. 25-35, 5%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Seaking (Lv. 30-55, 45%)
- **Swarm**: Bibarel, Snover
- **Poke Radar**: Bibarel, Golduck, Sneasel, Snover

## Tier 8 — cap 62

### Mt Coronet (Ext)

#### North
- **Grass/Land**:
  - *Morning/Day*: Abomasnow (Lv. 38-39, 30%), Snover (Lv. 36, 20%), Medicham (Lv. 38-40, 20%), Machoke (Lv. 40, 10%), Chingling (Lv. 37-39, 10%), Nosepass (Lv. 38, 5%), Absol (Lv. 38-40, 5%)
  - *Night*: Snover (Lv. 36, 20%), Abomasnow (Lv. 38, 20%), Golbat (Lv. 38, 10%), Noctowl (Lv. 39, 10%), Machoke (Lv. 40, 10%), Medicham (Lv. 40, 10%), Chingling (Lv. 37-39, 10%), Nosepass (Lv. 38, 5%), Absol (Lv. 38-40, 5%)
- **Rock Smash**: Geodude (Lv. 36-40, 100%)
- **Swarm**: Abomasnow, Snover
- **Poke Radar**: Loudred

#### South
- **Grass/Land**:
  - *Morning/Day*: Abomasnow (Lv. 38-39, 30%), Snover (Lv. 36, 20%), Medicham (Lv. 38-40, 20%), Machoke (Lv. 40, 10%), Chingling (Lv. 37-39, 10%), Nosepass (Lv. 38, 5%), Absol (Lv. 38-40, 5%)
  - *Night*: Snover (Lv. 36, 20%), Abomasnow (Lv. 38, 20%), Golbat (Lv. 38, 10%), Noctowl (Lv. 39, 10%), Machoke (Lv. 40, 10%), Medicham (Lv. 40, 10%), Chingling (Lv. 37-39, 10%), Nosepass (Lv. 38, 5%), Absol (Lv. 38-40, 5%)
- **Rock Smash**: Geodude (Lv. 36-40, 100%)
- **Swarm**: Abomasnow, Snover
- **Poke Radar**: Loudred

### Sendoff Spring
- **Grass/Land**:
  - *Morning/Day*: Graveler (Lv. 37-39, 35%), Bibarel (Lv. 37-38, 30%), Staravia (Lv. 38-40, 20%), Chingling (Lv. 37-39, 10%), Dusclops (Lv. 40, 5%)
  - *Night*: Bibarel (Lv. 37-38, 30%), Graveler (Lv. 37-38, 25%), Dusclops (Lv. 39-40, 15%), Golbat (Lv. 38, 10%), Staravia (Lv. 40, 10%), Chingling (Lv. 37-39, 10%)
- **Surf**: Golduck (Lv. 20-40, 100%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Goldeen (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Seaking (Lv. 30-55, 45%)
- **Swarm**: Bibarel, Graveler
- **Poke Radar**: Bibarel, Chingling, Dusclops, Staravia

### Route 222
- **Grass/Land**:
  - *Morning/Day*: Electabuzz (Lv. 39-41, 30%), Floatzel (Lv. 40, 20%), Chatot (Lv. 38, 10%), Wingull (Lv. 38, 10%), Magnemite (Lv. 39, 10%), Luxio (Lv. 38-40, 10%), Pelipper (Lv. 40, 5%), Magneton (Lv. 41, 5%)
  - *Night*: Floatzel (Lv. 38-41, 40%), Electabuzz (Lv. 39, 20%), Wingull (Lv. 38, 10%), Magnemite (Lv. 39, 10%), Luxio (Lv. 38-40, 10%), Pelipper (Lv. 40, 5%), Magneton (Lv. 41, 5%)
- **Surf**: Tentacool (Lv. 30-40, 60%), Wingull (Lv. 30-40, 30%), Tentacruel (Lv. 30-50, 9%), Pelipper (Lv. 30-50, 1%)
- **Rock Smash**: Geodude (Lv. 38-41, 100%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Remoraid (Lv. 10-25, 45%)
- **Super Rod**: Gyarados (Lv. 30-55, 55%), Octillery (Lv. 30-55, 45%)
- **Swarm**: Skitty
- **Poke Radar**: Flaaffy

### Sunyshore City
- **Surf**: Tentacool (Lv. 30-40, 60%), Wingull (Lv. 30-40, 30%), Tentacruel (Lv. 30-50, 9%), Pelipper (Lv. 30-50, 1%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Remoraid (Lv. 10-25, 45%)
- **Super Rod**: Octillery (Lv. 30-55, 45%), Gyarados (Lv. 30-55, 40%), Staryu (Lv. 20-50, 15%)

## Tier 9 — cap 67

### Route 223
- **Surf**: Tentacruel (Lv. 30-50, 60%), Pelipper (Lv. 30-50, 30%), Mantyke (Lv. 30-40, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Remoraid (Lv. 15-25, 40%), Octillery (Lv. 25-35, 5%)
- **Super Rod**: Gyarados (Lv. 30-55, 40%), Wailmer (Lv. 30-40, 40%), Octillery (Lv. 20-50, 15%), Wailord (Lv. 40-55, 5%)

### Victory Road

#### 1F
- **Grass/Land**:
  - *Morning/Day*: Graveler (Lv. 40-42, 30%), Rhyhorn (Lv. 41, 20%), Onix (Lv. 41-42, 20%), Rhydon (Lv. 41-43, 15%), Golbat (Lv. 43, 5%), Steelix (Lv. 42, 5%), Gabite (Lv. 41, 5%)
  - *Night*: Graveler (Lv. 40, 20%), Rhyhorn (Lv. 41, 20%), Onix (Lv. 41-42, 20%), Golbat (Lv. 42-43, 15%), Rhydon (Lv. 41-43, 15%), Steelix (Lv. 42, 5%), Gabite (Lv. 41, 5%)
- **Swarm**: Graveler, Rhyhorn
- **Poke Radar**: Gabite, Onix, Rhydon, Steelix

#### 1F Room 1
- **Grass/Land**:
  - *Morning/Day*: Floatzel (Lv. 47-49, 35%), Graveler (Lv. 47-49, 30%), Onix (Lv. 48-50, 20%), Golbat (Lv. 50, 5%), Steelix (Lv. 50, 5%), Gabite (Lv. 50, 5%)
  - *Night*: Floatzel (Lv. 47-49, 35%), Graveler (Lv. 47, 20%), Onix (Lv. 48-50, 20%), Golbat (Lv. 49-50, 15%), Steelix (Lv. 50, 5%), Gabite (Lv. 50, 5%)
- **Swarm**: Floatzel, Graveler
- **Poke Radar**: Floatzel, Gabite, Onix, Steelix

#### 1F Room 2
- **Grass/Land**:
  - *Morning/Day*: Floatzel (Lv. 48-50, 30%), Azumarill (Lv. 47-49, 30%), Graveler (Lv. 47-49, 15%), Dewgong (Lv. 48-50, 15%), Golbat (Lv. 50, 5%), Gabite (Lv. 49, 5%)
  - *Night*: Floatzel (Lv. 48-50, 30%), Azumarill (Lv. 47-49, 30%), Golbat (Lv. 47-50, 15%), Dewgong (Lv. 48-50, 15%), Graveler (Lv. 49, 5%), Gabite (Lv. 49, 5%)
- **Surf**: Floatzel (Lv. 35-55, 60%), Dewgong (Lv. 35-55, 30%), Lapras (Lv. 35-55, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 100%)
- **Super Rod**: Gyarados (Lv. 30-55, 100%)
- **Swarm**: Azumarill, Floatzel
- **Poke Radar**: Dewgong, Floatzel, Gabite

#### 1F Room 3
- **Grass/Land**:
  - *Morning/Day*: Floatzel (Lv. 47-49, 35%), Graveler (Lv. 47-49, 30%), Onix (Lv. 48-50, 20%), Golbat (Lv. 49, 5%), Steelix (Lv. 50, 5%), Gabite (Lv. 50, 5%)
  - *Night*: Floatzel (Lv. 47-49, 35%), Graveler (Lv. 47, 20%), Onix (Lv. 48-50, 20%), Golbat (Lv. 49, 15%), Steelix (Lv. 50, 5%), Gabite (Lv. 50, 5%)
- **Swarm**: Floatzel, Graveler
- **Poke Radar**: Floatzel, Gabite, Onix, Steelix

#### 2F
- **Grass/Land**:
  - *Morning/Day*: Steelix (Lv. 42-44, 40%), Magneton (Lv. 41-43, 30%), Graveler (Lv. 41-43, 15%), Golbat (Lv. 44, 5%), Onix (Lv. 42, 5%), Gabite (Lv. 43, 5%)
  - *Night*: Steelix (Lv. 42-44, 40%), Magneton (Lv. 41-43, 30%), Golbat (Lv. 41-44, 15%), Graveler (Lv. 43, 5%), Onix (Lv. 42, 5%), Gabite (Lv. 43, 5%)
- **Rock Smash**: Geodude (Lv. 41-44, 100%)
- **Swarm**: Magneton, Steelix
- **Poke Radar**: Gabite, Magneton, Onix, Steelix

#### B1F
- **Grass/Land**:
  - *Morning/Day*: Floatzel (Lv. 42-44, 30%), Azumarill (Lv. 41-43, 30%), Graveler (Lv. 41-43, 15%), Onix (Lv. 42, 10%), Golbat (Lv. 44, 5%), Steelix (Lv. 44, 5%), Gabite (Lv. 43, 5%)
  - *Night*: Floatzel (Lv. 42-44, 30%), Azumarill (Lv. 41-43, 30%), Golbat (Lv. 41-44, 15%), Onix (Lv. 42, 10%), Graveler (Lv. 43, 5%), Steelix (Lv. 44, 5%), Gabite (Lv. 43, 5%)
- **Surf**: Floatzel (Lv. 30-50, 90%), Golbat (Lv. 30-50, 10%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 100%)
- **Super Rod**: Gyarados (Lv. 30-55, 100%)
- **Swarm**: Azumarill, Floatzel
- **Poke Radar**: Floatzel, Gabite, Onix, Steelix

### Pokemon League
- **Surf**: Wingull (Lv. 30-40, 60%), Pelipper (Lv. 30-50, 35%), Tentacruel (Lv. 30-50, 5%)
- **Old Rod**: Magikarp (Lv. 3-15, 100%)
- **Good Rod**: Magikarp (Lv. 10-25, 55%), Remoraid (Lv. 10-25, 45%)
- **Super Rod**: Octillery (Lv. 30-55, 45%), Gyarados (Lv. 30-55, 40%), Luvdisc (Lv. 20-30, 15%)

## Tier 10 (Post-Elite Four)

Everything else - tbd later.
