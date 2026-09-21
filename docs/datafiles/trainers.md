# Trainer Data File Format

This document describes the file format for trainer data consumed by the
`trainerproc` build tool. Each trainer in the ROM is represented by a JSON file
`res/trainers/data/<name>.json`, which contains metadata for the trainer and
their party. The fields from each of these files are mapped onto the structs
defined in `include/struct_defs/trainer_data.h`.

## Top-Level Schema

| Field           | Type                          | Description                                                      |
| --------------- | ----------------------------- | ---------------------------------------------------------------- |
| `name`          | `string`                      | The trainer's display name.                                      |
| `class`         | `enum TrainerClass`           | The trainer class, e.g. Youngster, Ace Trainer, Gym Leader.      |
| `items`         | `enum Item[]`                 | Items usable by the trainer in battle (up to 4). May be empty.   |
| `ai_flags`      | `enum AIFlag[]`               | AI behavior flags for battle scripts. May be empty.              |
| `double_battle` | `bool`                        | Whether this is a double battle.                                 |
| `party`         | `object[]`                    | The trainer's party of Pokémon. See below for the member schema. |
| `messages`      | `object[]`                    | Dialogue messages. See the Messages section below.               |

## Party Member Schema

Each element of the `party` array represents a Pokémon owned by the trainer.

| Field        | Type                    | Description                                                          |
| ------------ | ----------------------- | ---------------------------------------------------------------------|
| `species`    | `enum Species`          | The species of the Pokémon.                                          |
| `form`       | `u8`                    | Form index, packed into the high bits of the species field.         |
| `level`      | `u16`                   | The level of the Pokémon.                                            |
| `item`       | `enum Item` or `null`   | The held item, or `null` for none.                                   |
| `moves`      | `enum Move[]` or `null` | Known moves (up to 4), or `null` for default level-up moves.         |
| `nature`     | `enum Nature` or `null` | The Pokémon's nature, or `null` to default to a neutral placeholder. |
| `ability`    | `enum Ability` or `null`| Which ability slot to force, or `null` to use the first slot.       |
| `ball_seal`  | `u16`                   | Ball seal / capsule decoration index.                                |

Every trainer Pokémon always has perfect IVs (31 in every stat) — there is no
per-Pokémon IV field. `nature` and `ability` are both optional, following the
same `null`-means-"no override" convention as `item`:

* `nature`: when `null`, the Pokémon currently defaults to `NATURE_HARDY` (a
  neutral nature with no stat effect) rather than being left to whatever the
  personality-value RNG happens to produce. This is a deliberate placeholder,
  not a permanent "Hardy is canon" choice - trainers are meant to get a
  hand-picked nature eventually, and `NATURE_HARDY` is just what an
  as-yet-uncurated one shows as in the meantime. When set to a real nature,
  the game re-rolls the personality value until it lands on that exact one.
* `ability`: when `null`, the Pokémon uses its species' first ability slot
  (`SPECIES_DATA_ABILITY_1`). When set, it must be one of that species' two
  real abilities (checked against `res/pokemon/<species>/data.json` at build
  time - specifying an ability the species can't actually have is a build
  error) and is applied directly, bypassing the personality-parity-bit
  mechanic vanilla normally uses to pick between ability slots.

The presence of the `item` and `moves` fields determines the exact structure of
the output data, which affects the trainer's party in-game. This is decided
by looking at the *first* Pokémon in the party only (`trainerproc.c` peeks at
`party[0]` and never looks at any other member to make this decision) - every
other member is then read according to whatever `party[0]` decided:

* If `party[0]` has a real `item` (a string, not `null`), every other member
  is required to have one too - a `null` on any later member is a build
  error.
* If `party[0]` has `"item": null`, the tool never reads the `item` field
  from *any* other member at all - even if a later Pokémon specifies a real
  item, it is silently ignored with no error or warning, and that Pokémon
  ends up holding nothing.

**This means a `null`-item lead can't be paired with real items on later
party members - but an itemless Pokémon anywhere in the party, lead
included, still can.** Use the literal string `"ITEM_NONE"` instead of
`null` for that Pokémon: it's a real value of `enum Item` (numeric value 0,
"no item"), so it satisfies the "party[0] has a string" check exactly like
any other item name would, opting the whole party into per-member item data
without actually holding anything. For example, a lead with no item and a
second Pokémon holding a Leftovers is `"item": "ITEM_NONE"` on the lead and
`"item": "ITEM_LEFTOVERS"` on the second - *not* `null` on the lead. The
functional effect is identical to `null` (the Pokémon holds nothing); this
only affects whether the *rest* of the party's `item` fields get read at
all. The same trick works for `moves`... except there is no move that means
"no moves," so a Pokémon that should just learn its default level-up moves
while a party-mate has a custom move-set has no equivalent escape hatch -
give it a real move-set too.

The same first-member rule, independently, applies to `moves` based on
whether `party[0]` has an array or `null`.

To illustrate, this party is valid:

```json
{
    "party": [
        {
            "species": "SPECIES_STARLY",
            "form": 0,
            "level": 7,
            "item": null,
            "moves": [
                "MOVE_QUICK_ATTACK",
                "MOVE_GROWL"
            ],
            "nature": null,
            "ability": null,
            "ball_seal": 0
        },
        {
            "species": "SPECIES_CHIMCHAR",
            "form": 0,
            "level": 9,
            "item": null,
            "moves": [
                "MOVE_SCRATCH",
                "MOVE_LEER"
            ],
            "nature": null,
            "ability": null,
            "ball_seal": 0
        }
    ]
}
```

However, this party is *not* valid - it's a build error, since Starly (the
*first* party member) specifies a held item, so every other member is
required to as well, and Chimchar does not:

```json
{
    "party": [
        {
            "species": "SPECIES_STARLY",
            "form": 0,
            "level": 7,
            "item": "ITEM_ORAN_BERRY",
            "moves": [
                "MOVE_QUICK_ATTACK",
                "MOVE_GROWL"
            ],
            "nature": null,
            "ability": null,
            "ball_seal": 0
        },
        {
            "species": "SPECIES_CHIMCHAR",
            "form": 0,
            "level": 9,
            "item": null,
            "moves": [
                "MOVE_SCRATCH",
                "MOVE_LEER"
            ],
            "nature": null,
            "ability": null,
            "ball_seal": 0
        }
    ],
}
```

The reverse ordering does *not* raise an error, which is easy to get wrong
when editing a party by hand. Here, Starly (first) has `"item": null`, so the
tool never even looks at Chimchar's `item` field - the build succeeds, but
Chimchar's Oran Berry is silently dropped and it ends up holding nothing:

```json
{
    "party": [
        {
            "species": "SPECIES_STARLY",
            "form": 0,
            "level": 7,
            "item": null,
            "moves": [
                "MOVE_QUICK_ATTACK",
                "MOVE_GROWL"
            ],
            "nature": null,
            "ability": null,
            "ball_seal": 0
        },
        {
            "species": "SPECIES_CHIMCHAR",
            "form": 0,
            "level": 9,
            "item": "ITEM_ORAN_BERRY",
            "moves": [
                "MOVE_SCRATCH",
                "MOVE_LEER"
            ],
            "nature": null,
            "ability": null,
            "ball_seal": 0
        }
    ],
}
```

The same first-member-decides rule applies to `moves` independently of
`item`. This party is invalid, since Starly (first) declares a custom
move-set, so Chimchar is required to as well, but specifies `null` instead:

```json
{
    "party": [
        {
            "species": "SPECIES_STARLY",
            "form": 0,
            "level": 7,
            "item": null,
            "moves": [
                "MOVE_QUICK_ATTACK",
                "MOVE_GROWL"
            ],
            "nature": null,
            "ability": null,
            "ball_seal": 0
        },
        {
            "species": "SPECIES_CHIMCHAR",
            "form": 0,
            "level": 9,
            "item": null,
            "moves": null,
            "nature": null,
            "ability": null,
            "ball_seal": 0
        }
    ],
}
```

## Messages

The `messages` array defines dialogue that the trainer may speak at various
points during a battle. Each entry has a `type` field drawn from
`enum TrainerMessageType` (e.g. `TRMSG_PRE_BATTLE`, `TRMSG_DEFEAT`,
`TRMSG_POST_BATTLE`, `TRMSG_REMATCH`) and an `en_US` field containing the
message text. As with the text-files stored in `res/text`, the content may be
a single string or an array of strings as a convenience for multi-line messages.
Entries with the `garbage` key in place of `en_US` represent padding present in
the retail ROM.

## Outputs

The tool produces the following outputs:

| Output                      | Description                                                    |
| --------------------------- | -------------------------------------------------------------- |
| `trdata.narc`               | `TrainerHeader` entries for every trainer, packed by index ID. |
| `trpoke.narc`               | Party data for every trainer, packed by index ID.              |
| `trainer_scripts.h`         | Generated header mapping trainer IDs to `ScriptEntry` macros.  |
| `npc_trainer_names.json`    | Text bank for trainer display names.                           |
| `npc_trainer_messages.json` | Text bank for in-battle trainer dialogue.                      |
| `trtbl.narc`                | Message lookup table mapping `(trainerID, messageType)` pairs. |
| `trtblofs.narc`             | Offset table into `trtbl.narc` for each trainer index.         |

## Example

```json
{
    "name": "Catherine",
    "class": "TRAINER_CLASS_ACE_TRAINER_FEMALE",
    "items": [],
    "ai_flags": [
        "AI_FLAG_BASIC",
        "AI_FLAG_EVAL_ATTACK",
        "AI_FLAG_EXPERT"
    ],
    "double_battle": false,
    "party": [
        {
            "species": "SPECIES_HAUNTER",
            "form": 0,
            "level": 23,
            "item": null,
            "moves": [
                "MOVE_NIGHT_SHADE",
                "MOVE_CONFUSE_RAY",
                "MOVE_SUCKER_PUNCH",
                "MOVE_CURSE"
            ],
            "nature": null,
            "ability": null,
            "ball_seal": 0
        },
        {
            "species": "SPECIES_MISDREAVUS",
            "form": 0,
            "level": 24,
            "item": null,
            "moves": [
                "MOVE_PSYBEAM",
                "MOVE_PAIN_SPLIT",
                "MOVE_CONFUSE_RAY",
                "MOVE_SPITE"
            ],
            "nature": null,
            "ability": null,
            "ball_seal": 0
        }
    ],
    "messages": [
        {
            "type": "TRMSG_PRE_BATTLE",
            "en_US": [
                "What does it feel like, taking one step\n",
                "after another into darkness?\r"
            ]
        },
        {
            "type": "TRMSG_DEFEAT",
            "en_US": [
                "You’re the kind of person who can keep\n",
                "going forward even into the unknown.\n"
            ]
        },
        {
            "type": "TRMSG_POST_BATTLE",
            "en_US": [
                "I think, sometimes, one has to make\n",
                "mistakes to learn what is right.\n"
            ]
        }
    ]
}
```
