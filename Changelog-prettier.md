# What's Different From Regular Pokémon Platinum

This is a plain-language guide to everything this hack changes about the
original game. It's written for players, not programmers, so it skips the
"how it was built" details and just tells you what you'll actually notice
and how things work now. It's organized by topic rather than in the order
things were added, and it's meant to be a living document — a starting
point you can add to as more changes come in.

A quick note on the philosophy behind a lot of these changes: this hack is
built around playing as a **hardcore nuzlocke** — if a Pokémon faints, it's
gone for good, no exceptions, no reviving. A lot of the item and shop
changes below exist specifically to support that ruleset (removing ways to
undo a faint, making items that boost your Pokémon over time work
differently, etc.) even if you're not playing with the nuzlocke rules
turned on yourself.

---

## Everyday Quality-of-Life Improvements

These don't change how the game plays strategically — they just make it
less tedious to play.

- **Text is faster.** Every text speed setting now moves one notch quicker
  than it used to (what used to be "Normal" speed now behaves like the old
  "Fast" setting, and so on).
- **You move faster.** Both walking and running cover ground about twice as
  fast as in the original game.
- **Pokémon and people following you keep up when you run.** In the
  original game, a following Pokémon, Barry, a rival, or a dungeon
  companion would gradually fall further and further behind if you held
  down the run button. They now match your running pace properly (they
  still trail one step behind, as intended — they just don't drift back
  any further than that).
- **Battle Style is locked to "Set."** You can no longer switch to "Switch"
  style, where the game asks if you want to swap Pokémon after your
  opponent's automatically. Every battle now plays with the classic
  "Set" rules from the start.
- **You can register up to 6 key items to the Y button** instead of just
  one. If you have two or more registered, pressing Y brings up a small
  list to choose from instead of only ever using one fixed item.
- **The wild/opposing Pokémon's HP bar now shows exact numbers**, not just
  the bar — the same way your own Pokémon's HP is displayed.
- **HP bars drain about 4x faster** when a Pokémon gets hit, so you're not
  stuck watching a slow crawl on a big hit against a high-HP Pokémon.
- **The Summary screen can show hidden stat details.** On a Pokémon's
  Skills page, hold **L** to see its Effort Values (EVs) in place of the
  normal stat numbers, or hold **R** to see its Individual Values (IVs).
  With neither held, each stat number is now color-coded by nature — the
  stat your nature boosts shows in red, the one it lowers shows in blue.
- **The Battle Tower's IV Judge has a new reaction** for a Pokémon with
  low overall IVs — he now opens with "Holy zipcode!" instead of the
  original "I see, I see..." line. Everything else about his readout is
  the same.
- **Your PC now has an "Inflict Status" option.** From the main PC menu you
  can pick a party Pokémon and directly give it (or cure it of) Sleep,
  Poison, Bad Poison, Burn, Freeze, or Paralysis.
- **HM moves can be forgotten like any other move.** You no longer need to
  visit the Move Deleter just to get rid of Cut, Fly, Surf, or another HM
  move — you can replace them the normal way whenever you learn something
  new, or mid-battle.
- **Honey Trees work instantly.** Slathering a tree with Honey now
  triggers its wild encounter chance right away instead of making you wait
  several hours and come back later. Which Pokémon (if any) shows up is
  still random, exactly as before — only the wait is gone.
- **Eggs can be hatched on the spot.** Selecting an egg from your party
  menu now offers a "Hatch" option that hatches it immediately, instead of
  needing to walk around with it in your party until it hatches naturally.
- **Your starter Pokémon's "Met at" location is now "Rowan's briefcase"**
  instead of "Route 201," matching where you actually received it.
- **Long routes and Mt. Coronet are now treated and labeled as separate
  areas.** Several routes in the original game are really made of two
  separate halves that share one name — that mattered for the "only catch
  one Pokémon per area" nuzlocke rule, so those halves are now named and
  banner-announced separately: Routes 204, 205, and 210 are split into
  "south" and "north" halves; Route 211 into "west" and "east"; Route 212
  into "north" and "south." Mt. Coronet's outdoor sections are now
  "Mt. Coronet (Ext)" and its indoor floors are "Mt. Coronet (Int)." This
  affects the location name banner, your Pokémon's "Met at" info, and
  anywhere else the game shows where you are or where a Pokémon was caught.

---

## Battling

- **You can't use items on your own Pokémon during a battle.** Only Poké
  Balls work from the Bag mid-battle — potions, status healers, X items,
  and everything else are blocked with an "Items can't be used here."
  message. (This is a core part of the nuzlocke-friendly design: no
  healing your way out of a bad matchup.)
- **Battling no longer grants Effort Values (EVs) at all.** Defeating wild
  or trainer Pokémon used to slowly build up your stats behind the scenes;
  now the only way to gain EVs is through vitamin items (see the Items
  section below), so your Pokémon's stat growth is something you
  deliberately choose rather than a side effect of grinding.
- **Hail has been renamed Snow**, and it works a little differently: it no
  longer chips away at non-Ice-type Pokémon's HP each turn (Ice Body still
  heals as normal), and while it's active, Ice-type Pokémon get a 50%
  boost to their Defense — mirroring how Sandstorm already boosts Rock
  types' Special Defense.
- **Three new battlefield conditions were added, alongside weather:
  Electric Terrain, Psychic Terrain, and Distortion Terrain.**
  - *Electric Terrain* stops grounded Pokémon from falling asleep (Rest
    included), wakes up a sleeping Pokémon the moment it's grounded, and
    boosts Electric-type moves.
  - *Psychic Terrain* blocks priority moves from hitting grounded targets
    and boosts Psychic-type moves used by a grounded attacker.
  - *Distortion Terrain* flips every stat-stage change on the field, the
    same way the Contrary ability does, for every Pokémon on the field
    regardless of whether it's grounded.
  - Weather and terrain now share the same "slot" — setting one clears the
    other, so you can't have both active together. Trick Room is
    unaffected by this and always keeps running on its own independent
    timer.
  - There's also support for a battle location to have Trick Room running
    permanently for the whole fight rather than on the usual five-turn
    timer (only the move Trick Room itself could cancel it early), though
    no location currently uses this.
- **Sticky Web now only affects one side of the field** (the way it works
  in modern Pokémon games) instead of being a shared field-wide effect,
  and it always prints a single, clear "was caught in a sticky web!"
  message instead of layering extra text on top of the Speed-drop message.
- **Defog and Rapid Spin now clear every hazard on both sides of the
  field** — Spikes, Toxic Spikes, Stealth Rock, Sticky Web, and Tailwind —
  plus Gravity, not just the hazards on your own side. This is a
  deliberate departure from how modern games limit Rapid Spin to your own
  side.
- **The battle AI understands terrain**, not just weather — trainers will
  now factor an active terrain into their decision-making the same way
  they already did for weather.
- **Dialga, Palkia, and Giratina have new signature abilities** in place
  of their old ones (see the Pokémon-by-Pokémon list below for details).
- **The Normalize ability now also boosts the power of the Normal-type
  moves it affects**, not just changing their type, matching how it works
  in modern games.
- **Sturdy now works like the modern version.** In addition to blocking
  one-hit-KO moves, a Pokémon with Sturdy that's at full HP will always
  survive an otherwise-fatal hit with 1 HP left (unless the attacker's
  ability ignores it). It shows its own "survived the hit with Sturdy!"
  message rather than the generic "endured the hit!" line.

---

## New and Changed Abilities

A number of abilities that don't exist in the original Platinum have been
added to the game, and a few existing ones have new effects. These are
listed in the Pokémon-by-Pokémon section below wherever they were given to
a specific species, but here's what each one actually does:

- **Sheer Force** — moves with a secondary effect (like a chance to
  flinch or lower a stat) hit 30% harder, but lose that secondary effect
  entirely.
- **Defiant** — if an opponent lowers this Pokémon's stats, its Attack
  sharply rises in response.
- **Competitive** — same idea as Defiant, but it boosts Special Attack
  instead.
- **Prankster** — status moves (non-damaging moves) go first in priority.
- **Contrary** — every stat change this Pokémon would normally receive is
  flipped — a stat drop becomes a boost, and vice versa.
- **Psychic Surge / Electric Surge** — sets up Psychic Terrain or Electric
  Terrain, respectively, the moment this Pokémon enters battle.
- **Shed Spines** — lays a layer of Spikes on the opponent's side the
  moment this Pokémon enters battle.
- **Toxic Boost** — while poisoned, this Pokémon's physical moves hit 50%
  harder.
- **Protean** — this Pokémon's type changes to match whatever move it's
  about to use.
- **Refrigerate** — this Pokémon's Normal-type moves become Ice-type
  instead, and hit a bit harder.
- **Flare Boost** — while burned, this Pokémon's special moves hit 50%
  harder.
- **Moxie** — knocking out an opposing Pokémon raises this Pokémon's
  Attack.
- **Poison Touch** — moves that make physical contact have a chance to
  poison the target.
- **Justified** — getting hit by a Dark-type move raises this Pokémon's
  Attack.
- **Sharpness** — slicing and cutting moves (like Slash, Leaf Blade, and
  Air Slash) hit 50% harder.
- **Time Warp** (Dialga's signature ability) — while this Pokémon is on
  the field, two-turn moves like Solar Beam and Fly happen in a single
  turn instead of needing a charge-up turn, and recharge moves like Hyper
  Beam skip their recharge turn.
- **Space Warp** (Palkia's signature ability) — sets Gravity for the rest
  of the battle the moment this Pokémon enters.
- **Distortion Surge** (Giratina's signature ability) — sets Distortion
  Terrain the moment this Pokémon enters battle.

---

## New and Changed Moves

- **Strength is now Fighting-type, Cut is now Grass-type, and Rock Climb
  is now Rock-type** — matching their names/themes. Nothing else about
  them changed.
- **X-Scissor now has a high critical-hit ratio.**
- **Boomburst is a new move** — a powerful Normal-type special attack
  that hits every Pokémon on the field at once.
- **Knock Off is much stronger and works differently.** It now deals
  extra damage whenever the target is holding an item it can actually
  knock away, and it hits significantly harder overall than it used to.
- **Thief, Switcheroo, and Trick have all been removed and replaced with
  Knock Off.** Every Pokémon that used to learn one of those three moves
  now learns Knock Off instead, at the same level it used to learn the
  older move (see the Pokémon-by-Pokémon section for exactly which levels).
  The TM that used to teach Thief now teaches Knock Off.
- **Leech Life hits much harder** than it used to.
- **Attack Order now hits every adjacent opponent** instead of a single
  target, though it lost its old high critical-hit ratio.
- **Defend Order now raises Defense and Special Defense by two stages
  each** instead of just one.
- **Heal Order now also heals your ally** for half its max HP in a double
  battle, on top of healing the user as before.
- **Power Whip is much more accurate** but now has recoil — the user takes
  damage equal to a quarter of the damage it deals.
- **Rapid Spin hits harder and also raises the user's Speed** on top of
  clearing hazards.
- **Sticky Web is a new move** — a Bug-type move that lowers the Speed of
  any grounded Pokémon that switches into its effect.
- **Wild Charge is a new move** — a strong Electric-type physical attack
  that costs the user some recoil damage.
- **Expanding Force is a new move** — a Psychic-type special attack that
  hits harder, and hits both opponents in a double battle, while Psychic
  Terrain is active.
- **Many pure stat-boosting moves now have very low PP**, so setting up
  stats repeatedly in one battle isn't as easy as it used to be. Most of
  these moves (Bulk Up, Calm Mind, Agility, Iron Defense, Amnesia, Belly
  Drum, and similar moves) now have just 3 PP; Swords Dance, Nasty Plot,
  and Dragon Dance have only 1 PP. Moves that deal damage *and* happen to
  raise a stat (like Ancient Power) are unaffected, and so are stat-
  *lowering* moves.
- **A batch of historically unreliable moves are more accurate now.**
  Moves like Thunder, Blizzard, Focus Blast, Hydro Pump, Stone Edge, Fire
  Spin, Bounce, Rock Climb, Rock Tomb, and many others have all had their
  accuracy raised — none of their other stats changed.

---

## Pokémon-by-Pokémon Changes

Every Pokémon with a change gets its own entry below, even if several
Pokémon received the exact same change — nothing is merged.

**How to read these entries:**
- A stat line like `Atk: 45 -> 70` means that stat went from 45 to 70.
  Whenever one stat goes up, another on the same Pokémon goes down by the
  same amount, so its overall stat total never changes.
- An ability line like `Ability slot 1: Sand Veil -> Rough Skin` means
  that ability slot's ability changed; `Ability slot 2: None -> Chlorophyll`
  means a previously-empty second ability slot now has that ability.
  Ability slots not listed are unchanged.
- A learnset line like `Level 45: Roar -> Boomburst` means that level used
  to teach Roar and now teaches Boomburst instead. `Level 66: Earthquake`
  (no arrow) means that level now teaches a move it didn't teach before.
  `Level 58 -> 25: Grudge` means the same move's learnset level changed
  (from 58 to 25) without the move itself changing.
- An evolution line like `Evolution level: 40 -> 30` means the Pokémon now
  evolves at level 30 instead of 40.

### Bulbasaur
- Ability slot 2: None -> Chlorophyll

### Ivysaur
- Ability slot 2: None -> Chlorophyll

### Venusaur
- Ability slot 2: None -> Chlorophyll

### Charmander
- Ability slot 2: None -> Solar Power

### Charmeleon
- Ability slot 2: None -> Solar Power

### Charizard
- Ability slot 2: None -> Solar Power

### Squirtle
- Ability slot 2: None -> Rain Dish

### Wartortle
- Ability slot 2: None -> Rain Dish

### Blastoise
- Ability slot 2: None -> Rain Dish

### Beedrill
- Level 19: Rage -> X-Scissor
- Level 25: Toxic Spikes -> Cross Poison

### Sandshrew
- Ability slot 1: Sand Veil -> Rough Skin

### Sandslash
- Ability slot 1: Sand Veil -> Rough Skin

### Nidoqueen
- Ability slot 2: Rivalry -> Sheer Force

### Nidoking
- Ability slot 2: Rivalry -> Sheer Force

### Vulpix
- Ability slot 2: None -> Drought

### Ninetales
- Ability slot 2: None -> Drought

### Mankey
- Ability slot 1: Vital Spirit -> Defiant

### Primeape
- Ability slot 1: Vital Spirit -> Defiant

### Parasect
- HP: 60 -> 90
- SpA: 60 -> 30

### Ponyta
- Evolution level (into Rapidash): 40 -> 30

### Slowpoke
- Evolution level (into Slowbro): 37 -> 30
- Its King's Rock evolution into Slowking is unchanged in level/trigger,
  but like every trade evolution in the game, it now happens by using the
  item on it instead of trading (see Link Cable, in the Items section).

### Farfetch'd
- Ability slot 1: Keen Eye -> Defiant

### Onix
- Atk: 45 -> 70
- Spe: 70 -> 45
- Its evolution into Steelix now happens by using a Metal Coat on it
  directly instead of trading while holding one (see Link Cable).

### Tangela
- Level 40: Natural Gift -> Power Whip
- Level 54: Power Whip -> Natural Gift

### Seaking
- Atk: 92 -> 100
- Spe: 68 -> 60

### Jolteon
- Level 43: Thunder Fang -> Thunderbolt
- Level 50: Last Resort -> Discharge (moved up from level 78)

### Flareon
- Level 36: Fire Spin -> Fire Fang
- Level 43: Fire Fang -> Fire Spin
- Level 71: Fire Blast -> Flare Blitz

### Omanyte
- Evolution level (into Omastar): 40 -> 30

### Kabuto
- Evolution level (into Kabutops): 40 -> 30

### Chikorita
- Ability slot 2: None -> Leaf Guard

### Bayleef
- Ability slot 2: None -> Leaf Guard

### Meganium
- Ability slot 2: None -> Leaf Guard

### Cyndaquil
- Ability slot 2: None -> Flash Fire

### Quilava
- Ability slot 2: None -> Flash Fire

### Typhlosion
- Ability slot 2: None -> Flash Fire

### Totodile
- Ability slot 2: None -> Sheer Force

### Croconaw
- Ability slot 2: None -> Sheer Force

### Feraligatr
- Ability slot 2: None -> Sheer Force

### Furret
- Atk: 76 -> 91
- Spe: 90 -> 100
- SpA: 45 -> 20

### Noctowl
- Atk: 50 -> 26
- SpA: 76 -> 100

### Ledian
- SpA: 55 -> 20
- SpD: 110 -> 90

### Spinarak
- Level 29: Spider Web -> Sticky Web

### Ariados
- Atk: 90 -> 100
- SpA: 60 -> 50
- Level 32: Spider Web -> Sticky Web

### Togepi
- Level 17: Extrasensory (new)

### Togetic
- Level 17: Extrasensory (new)

### Xatu
- Atk: 75 -> 60
- SpA: 95 -> 110

### Ampharos
- Ability slot 2: None -> Electric Surge

### Politoed
- Ability slot 2: Damp -> Drizzle

### Jumpluff
- Level 30: Tailwind (new)

### Sunflora
- Atk: 75 -> 65
- SpA: 105 -> 115

### Yanma
- Level 30: Pursuit -> Ancient Power
- Level 33: Ancient Power -> Pursuit
- Level 43 -> 36: Wing Attack
- Level 57 -> 43: Bug Buzz

### Unown
- Atk: 72 -> 44
- SpA: 72 -> 100

### Girafarig
- HP: 70 -> 90
- Atk: 80 -> 60
- Def: 65 -> 90
- Spe: 85 -> 65
- SpA: 90 -> 60
- SpD: 65 -> 90

### Dunsparce
- Def: 70 -> 90
- Spe: 45 -> 25

### Qwilfish
- HP: 65 -> 90
- Spe: 85 -> 60

### Magcargo
- Ability slot 1: Magma Armor -> Solid Rock

### Swinub
- Ability slot 1: Oblivious -> Thick Fat

### Piloswine
- Ability slot 1: Oblivious -> Thick Fat

### Corsola
- HP: 55 -> 90
- SpA: 65 -> 30

### Octillery
- Atk: 105 -> 85
- SpA: 105 -> 125

### Delibird
- Atk: 55 -> 25
- SpA: 65 -> 95
- Ability slot 2: Hustle -> Snow Warning
- Level 10: Swift (new)
- Level 15: Ice Shard (new)
- Level 20: Icy Wind (new)
- Level 30: Fake Out (new)
- Level 40: Blizzard (new)
  (Its whole learnset was reworked around Ice-type coverage, so several
  of its original moves at other levels were removed to make room.)

### Mantine
- Atk: 40 -> 20
- SpA: 80 -> 100

### Houndoom
- Level 66: Dark Pulse (new)

### Stantler
- Atk: 95 -> 110
- SpA: 85 -> 70

### Treecko
- Ability slot 2: None -> Unburden

### Grovyle
- Ability slot 2: None -> Unburden

### Sceptile
- Ability slot 2: None -> Unburden

### Torchic
- Ability slot 2: None -> Speed Boost

### Combusken
- Ability slot 2: None -> Speed Boost

### Blaziken
- Ability slot 2: None -> Speed Boost

### Mudkip
- Ability slot 2: None -> Damp

### Marshtomp
- Ability slot 2: None -> Damp

### Swampert
- Ability slot 2: None -> Damp

### Mightyena
- Spe: 70 -> 100
- SpA: 60 -> 30

### Linoone
- Atk: 70 -> 90
- SpA: 50 -> 30

### Beautifly
- Atk: 70 -> 50
- SpD: 50 -> 70

### Dustox
- Atk: 50 -> 30
- SpA: 50 -> 70

### Shiftry
- Atk: 100 -> 120
- SpA: 90 -> 70

### Masquerain
- Atk: 60 -> 30
- Spe: 60 -> 110
- SpD: 82 -> 62

### Breloom
- Level 50: Spore (new)

### Exploud
- Level 45: Boomburst (new)
- Its level-up Roar was removed to make room in its 20-move learnset limit
  (it's still available to teach via TM05).

### Sableye
- Ability slot 2: Stall -> Prankster
- Level 30: Gravity (new)
- Level 57: Mean Look (removed)

### Mawile
- Atk: 85 -> 105
- Spe: 50 -> 30

### Plusle
- SpA: 85 -> 105
- SpD: 75 -> 55

### Minun
- Atk: 40 -> 20
- Def: 50 -> 70

### Volbeat
- Atk: 73 -> 58
- Spe: 85 -> 100
- Ability slot 2: Swarm -> Prankster

### Illumise
- Atk: 47 -> 32
- SpA: 73 -> 88
- Ability slot 1: Oblivious -> Prankster

### Swalot
- Def: 83 -> 98
- Spe: 55 -> 25
- SpD: 83 -> 98

### Sharpedo
- Def: 40 -> 85
- SpA: 95 -> 50

### Wailord
- Def: 45 -> 55
- Spe: 60 -> 40
- SpD: 45 -> 55
- Ability slot 2: Oblivious -> Filter

### Camerupt
- HP: 70 -> 90
- Spe: 40 -> 20
- Ability slot 1: Magma Armor -> Anger Point

### Torkoal
- Ability slot 2: None -> Drought

### Grumpig
- Ability slot 2: Own Tempo -> Psychic Surge

### Spinda
- Ability slot 1: Own Tempo -> Contrary

### Flygon
- Atk: 100 -> 120
- SpA: 80 -> 60

### Cacturne
- Ability slot 1: Sand Veil -> Shed Spines

### Zangoose
- Ability slot 2: None -> Toxic Boost

### Crawdaunt
- Level 30: Swift -> Aqua Jet
- Level 34: Taunt -> Sucker Punch

### Baltoy
- Level 31: Power Trick -> Recover

### Claydol
- Level 31: Power Trick -> Recover

### Lileep
- Ability slot 2: None -> Storm Drain

### Cradily
- Ability slot 2: None -> Storm Drain

### Anorith
- Ability slot 2: None -> Swift Swim

### Armaldo
- Ability slot 2: None -> Swift Swim

### Milotic
- Ability slot 2: None -> Competitive

### Castform
- Atk: 70 -> 40
- Def: 70 -> 55
- Spe: 70 -> 100
- SpA: 70 -> 100
- SpD: 70 -> 55

### Kecleon
- Atk: 90 -> 100
- Spe: 40 -> 30
- Ability slot 2: None -> Protean

### Banette
- Ability slot 1: Insomnia -> Prankster
- Level 58 -> 25: Grudge
- Level 66 -> 58: Trick -> Knock Off
- Level 66: Destiny Bond (new)

### Chimecho
- Typing: Psychic -> Psychic / Steel
- Level 33: Double-Edge -> Protect
- Level 41: Safeguard -> Wish

### Glalie
- Ability slot 1: Inner Focus -> Refrigerate

### Walrein
- Atk: 80 -> 95
- SpA: 95 -> 80

### Luvdisc
- Level 17: Lucky Chant -> Captivate
- Level 40: Captivate -> Lucky Chant

### Turtwig
- Ability slot 2: None -> Shell Armor

### Grotle
- Ability slot 2: None -> Shell Armor

### Torterra
- Ability slot 2: None -> Shell Armor

### Chimchar
- Ability slot 2: None -> Iron Fist

### Monferno
- Ability slot 2: None -> Iron Fist

### Infernape
- Ability slot 2: None -> Iron Fist

### Piplup
- Ability slot 2: None -> Competitive

### Prinplup
- Ability slot 2: None -> Competitive

### Empoleon
- Ability slot 2: None -> Competitive

### Kricketune
- Ability slot 1: Swarm -> Technician
- Ability slot 2: None -> Technician
- Level 30: X-Scissor -> Aerial Ace
- Level 34: Screech -> Bug Bite

### Shinx
- Ability slot 1: Rivalry -> Guts

### Luxio
- Ability slot 1: Rivalry -> Guts

### Luxray
- Ability slot 1: Rivalry -> Guts
- Level 49: Scary Face -> Wild Charge

### Cranidos
- Ability slot 2: None -> Sheer Force

### Rampardos
- Ability slot 2: None -> Sheer Force

### Shieldon
- Ability slot 2: None -> Soundproof

### Bastiodon
- Ability slot 2: None -> Soundproof

### Wormadam (Plant Cloak)
- Atk: 59 -> 99
- SpA: 79 -> 39
- Level 29: Growth -> Leech Life
- Level 47: Leaf Storm -> Seed Bomb
- Level 50: Sucker Punch (new)

### Wormadam (Sandy Cloak)
- Atk: 79 -> 99
- SpA: 59 -> 39
- Level 29: Harden -> Leech Life
- Level 47: Fissure -> Earthquake
- Level 50: Sucker Punch (new)

### Wormadam (Trash Cloak)
- Atk: 69 -> 79
- SpA: 69 -> 79
- Spe: 36 -> 16
- Level 29: Metal Sound -> Leech Life
- Level 50: Sucker Punch (new)
  (Trash Cloak keeps Iron Head at level 47 — no change there.)

### Mothim
- Atk: 94 -> 74
- SpA: 94 -> 114

### Combee
- Gender ratio: 12.5% female -> 75% female

### Pachirisu
- Ability slot 2: Pickup -> Volt Absorb

### Cherrim
- HP: 70 -> 90
- Spe: 85 -> 65

### Gastrodon
- Ability slot 1: Sticky Hold -> Storm Drain
  (its second ability was already Storm Drain, so this is now its only
  ability)

### Drifloon
- Ability slot 1: Aftermath -> Flare Boost

### Drifblim
- Ability slot 1: Aftermath -> Flare Boost

### Buneary
- Ability slot 1: Run Away -> Limber
- Ability slot 2: Klutz -> Scrappy

### Lopunny
- Ability slot 1: Cute Charm -> Limber
- Ability slot 2: Klutz -> Scrappy

### Honchkrow
- Ability slot 1: Insomnia -> Moxie

### Glameow
- Evolution level (into Purugly): 38 -> 17

### Purugly
- Ability slot 1: Thick Fat -> Defiant
- Ability slot 2: Own Tempo -> Thick Fat

### Stunky
- Evolution level (into Skuntank): 34 -> 24

### Chatot
- Atk: 65 -> 55
- Spe: 91 -> 101

### Garchomp
- Ability slot 1: Sand Veil -> Rough Skin

### Croagunk
- Ability slot 1: Anticipation -> Poison Touch

### Toxicroak
- Ability slot 1: Anticipation -> Poison Touch

### Carnivine
- Atk: 100 -> 130
- SpA: 90 -> 60

### Finneon
- Evolution level (into Lumineon): 31 -> 20

### Lumineon
- Level 26: Captivate -> Tailwind

### Lickilicky
- Ability slot 1: Own Tempo -> Cloud Nine
- Ability slot 2: Oblivious -> Filter

### Tangrowth
- Level 40: Natural Gift -> Power Whip
- Level 54: Power Whip -> Natural Gift

### Togekiss
- Level 1: Ancient Power (new)
- Level 1: Last Resort (new)
- Level 1: Wish (new)
- Level 1: Follow Me (new)
- Level 1: Encore (new)
- Level 1: Yawn (new)
- Level 1: Extrasensory (new)

### Yanmega
- Level 30: Pursuit -> Ancient Power
- Level 33: Ancient Power -> Pursuit
- Level 43 -> 36: Slash -> Wing Attack
- Level 57 -> 43: Bug Buzz

### Leafeon
- Level 36: Magical Leaf -> Seed Bomb

### Glaceon
- Level 43: Ice Fang -> Ice Beam

### Gliscor
- Ability slot 2: Sand Veil -> Poison Heal

### Mamoswine
- Ability slot 1: Oblivious -> Thick Fat

### Gallade
- Ability slot 1: Steadfast -> Justified
- Ability slot 2: Steadfast -> Sharpness

### Dusknoir
- HP: 45 -> 75
- SpA: 65 -> 35

### Froslass
- Atk: 80 -> 50
- SpA: 80 -> 110
- Ability slot 1: Snow Cloak -> Snow Warning
- Ability slot 2: Snow Cloak -> Snow Warning

### Rotom
- Level 1: Trick -> Knock Off
  (typing stays Electric / Ghost)

### Rotom (Heat)
- Typing: Electric / Ghost -> Electric / Fire
- Level 1: Trick -> Knock Off

### Rotom (Wash)
- Typing: Electric / Ghost -> Electric / Water
- Level 1: Trick -> Knock Off

### Rotom (Frost)
- Typing: Electric / Ghost -> Electric / Ice
- Level 1: Trick -> Knock Off

### Rotom (Fan)
- Typing: Electric / Ghost -> Electric / Flying
- Level 1: Trick -> Knock Off

### Rotom (Mow)
- Typing: Electric / Ghost -> Electric / Grass
- Level 1: Trick -> Knock Off

### Uxie
- Ability slot 1: Levitate -> Psychic Surge
- Ability slot 2: None -> Psychic Surge
- Level 51: Extrasensory -> Expanding Force

### Mesprit
- Ability slot 1: Levitate -> Psychic Surge
- Ability slot 2: None -> Psychic Surge
- Level 51: Extrasensory -> Expanding Force

### Azelf
- Ability slot 1: Levitate -> Psychic Surge
- Ability slot 2: None -> Psychic Surge
- Level 51: Extrasensory -> Expanding Force

### Arceus
- Level 1: Spacial Rend (new)
- Level 1: Roar of Time (new)
- Level 1: Shadow Force (new)

---

## Leveling and the Level Cap

To keep pacing under control, there's a **hard level cap** — your Pokémon
simply cannot gain any more Exp. once they hit it, no matter how many
battles you win. The cap starts fairly low and is raised at certain story
milestones as you progress, opening up as you earn more badges. This
applies everywhere Exp. is normally gained, including from battling and
from leaving a Pokémon at the Day Care.

If you use a Rare Candy, it jumps the Pokémon straight to the current
level cap in one go (rather than one level at a time), and it correctly
offers every move that Pokémon would have learned along the way, so you
won't miss out on level-up moves just because you skipped several levels
at once.

---

## Items

### The Underground is closed
The Underground can no longer be entered at all, through any means. Trying
to use the Explorer Kit tells you: *"Sorry, the underground is undergoing
maintenance."*

### Shops: things you can only buy once, and purchase limits
Shops used to sell everything with unlimited stock. Now:

- **Every TM, plus Wide Lens, Zoom Lens, Metronome, and Shed Shell, can
  only be bought once, ever** — from any shop that sells them (this
  includes the department store, the Battle Frontier's BP shop, and the
  Prize Corner). Once you've bought one, its price is replaced with
  **"Sold out!"** everywhere it's sold, and you can't buy a second one.
- **Protein, Iron, Calcium, Zinc, Carbos, HP Up, and PP Up are each
  limited to 10 total purchases, for the whole game**, added up across
  every shop that sells them. Once you hit 10 of a given one, it also
  shows "Sold out!" (Finding or winning extras outside of a shop purchase
  doesn't count against this limit — it's only about what you buy.) Note:
  PP Up currently isn't sold in any shop in this hack (it's still
  obtainable as a fixed pickup in several overworld spots), so this cap
  is ready to go but has nothing to apply to yet unless that changes.
- **The shop that used to sell in-battle stat boosters (X Speed, X Attack,
  Guard Spec., Dire Hit, and similar) now sells only Heart Scales, priced
  at 10,000**, since those stat-boosting items can no longer be used in
  battle at all (see the Battling section above). No shop in the game
  sells battle-only stat boosters anymore.
- **Revive, Max Revive, and Revival Herb can no longer be bought from any
  shop.** (See below for why they're also gone from the overworld.)
- **Silk Scarf is no longer sold at the Prize Corner**, since your rival
  now hands you one directly early in the game instead.

### Revives are gone from the game entirely
Since a real nuzlocke run treats a fainted Pokémon as gone for good,
Revive, Max Revive, and Revival Herb no longer serve any purpose — so
they've been removed not just from shops but from every hidden and
overworld item spot that used to contain one. Here's exactly what each
spot gives now, with what it used to give in brackets:

**Full Restore** can now be found at:
- Route 217 (was Max Revive)
- Mt. Coronet, 4F Rooms 1 & 2 (was Max Revive)
- Route 213 (was Max Revive)
- Floaroma Meadow (was Max Revive)
- Route 221 (was Max Revive)

**Max Potion** can now be found at:
- Grand Lake / Route 213 Northeast House (was Max Revive)
- Route 217 (was Revival Herb)
- Mt. Coronet Outside South (was Revival Herb)
- Mt. Coronet Outside North (was Revival Herb)

**Hyper Potion** can now be found at:
- Route 209 (was Revive)
- Floaroma Meadow (was Revive)
- Mt. Coronet 1F South (was Revive)

**Full Heal** can now be found at:
- Route 215 (was Revive)
- Route 207 (was Revive)

**Rare Candy** can now be found at:
- Victory Road 1F (was Max Revive)
- Stark Mountain Room 2 (was Max Revive)
- Wayward Cave 1F, as a ground item (was Revive)
- Route 207, as a ground item (was Revive)
- Route 212 South, as a ground item (was Revive)
- Route 209's Lost Tower 3F, as a ground item (was Revive)
- Mt. Coronet B1F, as a ground item (was Revive)
- Route 225, as a ground item (was Revive)
- Team Galactic's Eterna Building 4F, as a ground item (was Revive)
- Iron Island B1F Right Room, as a ground item (was Revive)
- Route 216, as a ground item (was Revive)

**Heart Scale** can now be found at:
- Route 213, as a ground item (was Max Revive)
- Galactic HQ 2F, as a ground item (was Revive)
- Mt. Coronet 2F, as a ground item (was Max Revive)
- Stark Mountain Room 2, as a ground item (was Max Revive)

### Poké Doll replaced with Shed Shell
The Poké Doll's only use — fleeing a wild battle — is meaningless now that
items can't be used in battle at all, so it's been replaced with a
**Shed Shell**, a held item that lets its holder always switch out safely.
It's now found at:
- Galactic HQ 2F, as a hidden item (was Poké Doll)
- The Veilstone Dept Store's 1F Left counter (was Poké Doll) — this shop
  copy can only be bought once, like the other one-time shop items above.

### Overworld "X" battle items replaced with treasure
Every ground item that used to give a battle-only "X item" now gives a
treasure item instead. Earlier spots generally give lower-value treasure
and later spots give better treasure:

**Tiny Mushroom** can now be found at:
- Route 203 (was X Defense)
- Jubilife City's Trainers' School (was X Attack)

**Pearl** can now be found at:
- Oreburgh City (was Dire Hit)
- Route 205 South (was X Sp. Def)

**Big Mushroom** can now be found at:
- Route 205 North (was Guard Spec.)

**Stardust** can now be found at:
- Team Galactic's Eterna Building, 2F (was X Speed)

**Big Pearl** can now be found at:
- Team Galactic's Eterna Building, 3F (was X Sp. Atk)

**Star Piece** can now be found at:
- Route 218 (was X Accuracy)

### Overworld Shards replaced with EV vitamins
Every ground item or hidden item that used to give a Shard now gives one
of the six EV-boosting vitamins instead, cycled through fairly evenly:

**Protein** can now be found at:
- Route 207 / Wayward Cave 1F, as a hidden item (was Red Shard)
- Mt. Coronet 2F, as a hidden item (was Red Shard)
- Wayward Cave 1F, as a hidden item (was Blue Shard)
- Eterna Forest, as a ground item (was Green Shard)
- Route 214, as a ground item (was Red Shard)
- Fuego Ironworks Building, as a ground item (was Blue Shard)
- Route 217, as a ground item (was Blue Shard)

**Iron** can now be found at:
- Wayward Cave 1F, as a hidden item (was Yellow Shard)
- Mt. Coronet Outside South, as a hidden item (was Blue Shard)
- Oreburgh City, as a ground item (was Yellow Shard)
- Fuego Ironworks Building, as a ground item (was Red Shard)
- Galactic HQ 2F, as a ground item (was Green Shard)

**Calcium** can now be found at:
- Mt. Coronet 1F North Room 2, as a hidden item (was Yellow Shard)
- Mt. Coronet 4F Rooms 1 & 2, as a hidden item (was Yellow Shard)
- Great Marsh Area 2, as a ground item (was Blue Shard)
- Team Galactic's Eterna Building 4F, as a ground item (was Blue Shard)
- Fuego Ironworks Building, as a ground item (was Green Shard)
- Route 225, as a ground item (was Green Shard)

**Zinc** can now be found at:
- Mt. Coronet 1F North Room 1, as a hidden item (was Red Shard)
- Mt. Coronet 1F Tunnel Room, as a hidden item (was Green Shard)
- Great Marsh Area 4, as a ground item (was Green Shard)
- Route 212 North, as a ground item (was Green Shard)
- Fuego Ironworks Building, as a ground item (was Yellow Shard)
- Route 227, as a ground item (was Yellow Shard)

**Carbos** can now be found at:
- Mt. Coronet B1F, as a hidden item (was Green Shard)
- Mt. Coronet 1F Tunnel Room, as a second hidden item (was Green Shard)
- Great Marsh Area 6, as a ground item (was Red Shard)
- Route 212 South, as a ground item (was Blue Shard)
- Iron Island B2F Left Room, as a ground item (was Red Shard)
- Route 230, as a ground item (was Blue Shard)

**HP Up** can now be found at:
- Mt. Coronet B1F, as a hidden item (was Blue Shard)
- Wayward Cave 1F, as a hidden item (was Green Shard)
- Route 213, as a ground item (was Yellow Shard)
- Route 210 North, as a ground item (was Red Shard)
- Iron Island B1F Left Room, as a ground item (was Yellow Shard)
- Survival Area, as a ground item (was Red Shard)

A few NPCs who used to specifically *hand you* a Shard as a reward now
give you a Heart Scale instead — this includes a Hiker in the Solaceon
Ruins who thanks you for lending him Defog, and a daily Ace Trainer in
the Great Marsh. Similarly, the Fuego Ironworks worker who used to trade
a Star Piece for a set of Shards now trades a Star Piece for a Heart Scale
instead (the bulk "trade 10 at once" option still works the same way).
These are separate NPC trades/gifts, not ground items, so they aren't
included in the list above.

### EV vitamins are stronger, but limited per Pokémon
Since battling no longer grants any Effort Values, vitamins (Protein,
Iron, Calcium, Zinc, Carbos, and HP Up) are now the *only* way to build up
your Pokémon's stats, and they've been made much stronger to compensate:

- **Each vitamin now gives 50 EVs per use** (up from the original 10).
- Because of the game's normal 100-EV-per-stat ceiling, this naturally
  means **only 2 uses per stat actually do anything** — a 3rd use on an
  already-maxed stat has no effect.
- **A Pokémon can only benefit from vitamins 4 times total**, across all
  six types combined. Once a Pokémon has had 4 successful vitamin uses, no
  more vitamins will affect it, even on a stat that hasn't been touched
  yet. (Shop purchase limits, described above, are separate from this —
  they cap how many vitamins you can *buy*, not how many you can use on
  one Pokémon.)

### Link Cable — trade evolutions no longer require a trade
A new item, the **Link Cable**, lets you evolve any Pokémon that normally
needs to be traded, just by using the item on it instead. This covers
every trade evolution in the game — Kadabra, Machoke, Graveler, and
Haunter now evolve when you use a Link Cable on them, and Pokémon that
needed to be traded *while holding* a specific item (like Onix holding a
Metal Coat, or Poliwhirl holding a King's Rock) now evolve when you simply
use that same held item on them directly from the Bag, the same way you'd
use an Evolution Stone.

### PPHM — a portable full heal
A new key item, the **PPHM (Portable Pokémon Healing Machine)**, fully
heals your whole party's HP, status, and PP on the spot, just like a
Pokémon Center — but usable anywhere, anytime. Nurse Joy gives it to you
automatically the first time she heals your party.

### Repel Toggle
A new key item that keeps a Repel effect running indefinitely with no
step limit — turn it on and weak wild Pokémon simply won't appear until
you turn it back off. It can be registered to a button and toggled with
a single press, and its description always shows whether it's currently
on or off. Your rival gives you this early on, right after you receive
your first Poké Balls.

### PP Up is no longer handed out for free
Since stat-boosting moves now have very low PP overall, PP Up isn't as
valuable as it used to be — the two spots in the game that used to give
one away for free (the Pokémon Center lottery and a common high-tier
"lucky pickup" reward) now give a Heart Scale instead. PP Up can still be
bought or found in its usual fixed spots, and — as noted above — is now
also limited to 10 lifetime shop purchases if it's ever added to a shop.

### Move tutors teach moves for free
The shard-based move tutors (the ones in the Survival Area, Route 212, and
Snowpoint) no longer charge any Shards to teach their moves — everything
they offer is free. The Battle Frontier's Battle-Point tutors and the
regular Move Reminder are unaffected.

### The Vs. Seeker, trainer rematches, and Pokémon Center daily trainers are all gone
The Vs. Seeker item has been removed from the game, and along with it, the
entire trainer-rematch system it powered — no trainer in the game can be
challenged for a second battle anymore. Your rival now gives you a Silk
Scarf instead of a Vs. Seeker, early in the game. The unrelated
"daily rotating trainers" you could battle at Pokémon Centers have also
been removed entirely.

### Gift Pokémon and eggs go to your PC box if your party is full
Instead of turning you away and telling you to come back later, a gift
Pokémon or a gifted egg will now automatically be sent to an open PC box
if your party is full at the time. You'll be told it was sent to your PC.
An egg sitting in a box doesn't make progress toward hatching until it's
moved into your active party.

---

## World and Location Changes

### The house on Route 221 gives its rewards all at once
The NPC in the house on Route 221 used to run a daily minigame where you'd
show him a Pokémon of a specific level to earn one of three items (Black
Belt, Expert Belt, or Focus Sash) in rotation, forever. That's gone now —
talking to him gives you **all three items right away, in a single visit**,
regardless of the day, and never again after that (if your Bag is full
partway through, he'll pick up where he left off on your next visit).

### Oreburgh Gym has a new quiz-rock puzzle
Two rock obstacles now block one of the paths through Oreburgh Gym (the
side without any trainers on it). Interacting with a rock doesn't smash it
right away — instead, it asks you a multiple-choice question:

- The first rock asks what item your rival gave you on Route 207 (the
  correct answer is Silk Scarf).
- The second rock asks what item your rival gave you on Route 202 (the
  correct answer is the Repel Toggle).

Answer correctly and the rock crumbles away, clearing the path for good.
Answer incorrectly and a Level 25 Graveler bursts out and attacks you,
using only Magnitude and Rock Tomb — the rock stays in place afterward, so
you can try again. The Gym's front-desk guide warns you about this ahead
of time, and also mentions that Roark likes to use speed-lowering moves
and set up Stealth Rock.

### A new patch of grass near Lake Verity
A small patch of tall grass has been added to the clearing near the Lake
Verity entrance, giving that area its own dedicated set of wild Pokémon
encounters (separate from Route 201's). The exact Pokémon that appear
there are still being tuned and may change.

### Smashing rocks can now start a wild Pokémon battle
In the original game, using Rock Smash on a rock never led to an actual
encounter — it just cleared the rock. Now, every time you smash a rock,
there's a 50% chance it kicks off a wild Pokémon battle, similar to how
Rock Smash worked in older Pokémon games. This applies to Rock Smash rocks
both outdoors and inside caves. The specific Pokémon that can appear this
way are still being finalized.

### Elite Four: a webbed battle room
Aaron's Elite Four battle room now starts every fight with Sticky Web
already in effect on your side, so anything you send out that touches the
ground gets its Speed lowered right away.
