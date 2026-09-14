# AI behavior tests

A small, standalone test suite for the trainer AI's *decision-making*
(which item to use, which ability/type interactions it should recognize,
and so on). It builds and runs natively on your PC with a plain C compiler
in well under a second — no ROM build, no emulator, no meson.

## This is not the ROM's own code

`src/battle/trainer_ai/trainer_ai.c` and `script.s` pull in `<nitro.h>` and a
large slice of the battle engine (`battle_context.h`, `pokemon.h`, `party.h`,
generated data tables...). Actually compiling those files for a PC target
would mean writing and maintaining stub headers for most of that engine —
a large, fragile undertaking that breaks every time an upstream struct
changes.

Instead, `ai_model.c`/`ai_model.h` is a **small, hand-written model** of the
specific decision rules being tested, using simplified structs. It is kept in
sync **by hand** with the real logic in `trainer_ai.c`/`script.s`. This means:

- Tests here run instantly and never need a ROM rebuild.
- If you change a rule in the real AI (in `script.s` or `trainer_ai.c`), the
  matching function in `ai_model.c` needs the same change made by hand, or
  the model and the real game will silently drift apart. There's no
  automatic link between them.
- Treat a passing test here as "this specific rule behaves the way we
  intended," not as proof the ROM itself currently does the same thing —
  always double check against the real source when in doubt.

## Running the tests

```sh
./build.sh
```

Or manually:

```sh
cc -std=c11 -Wall -Wextra -O0 -g -o ai_tests_run testing.c ai_model.c main.c scenarios/*.c
./ai_tests_run
```

Output looks like:

```
RUN  dry_skin_is_immune_to_water_moves
PASS dry_skin_is_immune_to_water_moves
RUN  buggy_pre_fix_version_wipes_out_every_later_item_too
PASS buggy_pre_fix_version_wipes_out_every_later_item_too
...
14 passed, 0 failed, 14 total
```

A non-zero exit code means at least one test failed — useful if you ever want
to wire this into a pre-commit hook or CI job (nothing currently does).

## Adding a new scenario

Pick whichever file in `scenarios/` best matches what you're testing (or add
a new `.c` file there — `build.sh` picks up every file in that folder
automatically, nothing else needs updating), and add:

```c
TEST(my_new_scenario)
{
    BattleMon mon = { .curHP = 10, .maxHP = 100 };
    AIItem items[] = {
        { "Full Restore", ITEM_KIND_FULL_RESTORE, 0, 0 },
    };

    AIItemDecision decision = AI_ShouldUseItem(&mon, items, 1);

    ASSERT_TRUE(decision.shouldUse);
}
```

That's it — `TEST(name)` self-registers via a constructor attribute, so there
is no central list of tests to keep updated. `ASSERT_TRUE`, `ASSERT_FALSE`,
and `ASSERT_EQ` are the three assertions available (see `testing.h`); a
failing assertion prints the file/line and keeps running the rest of that
test rather than aborting, so you see every problem in one run.

## Layout

| File | Purpose |
| --- | --- |
| `ai_model.h` / `ai_model.c` | The hand-written model of the AI rules under test. |
| `testing.h` / `testing.c` | The self-registering test framework (no external dependency). |
| `main.c` | Entry point; runs every registered test and returns a pass/fail exit code. |
| `scenarios/item_use_common.c` | Everyday item-use decisions (low HP, status cures, not wasting a big potion on chip damage). |
| `scenarios/item_use_bugs.c` | Regression tests for the "AI dumps its entire item pool at once" bug (fixed in `trainer_ai.c`'s `TrainerAI_ShouldUseItem`) — includes a faithful reproduction of the original bug so you can see the difference directly. |
| `scenarios/ability_interactions.c` | Dry Skin's Water immunity and the Lagging Tail/Metal Burst speed check — both fixed base-game bugs in `script.s`. |
| `scenarios/move_scoring.c` | Move-scoring rules that depend on speed/HP, e.g. Thunder Wave's strong bonus specifically when the AI is slower than its target; also Sucker Punch's bonus when the target's last move was a damaging one. |
| `scenarios/terrain_mechanics.c` | Groundedness, how Electric/Psychic/Distortion Terrain's real mechanical effects (not just their presence) factor into move scoring (including Distortion's rewards for a move's own stat-lowering drawback and penalties for lowering the opponent's stats, gated by Sheer Force), the Sturdy/Focus Sash "survive at 1 HP" exception to the AI's kill prediction, the shared entry-hazard scoring pattern (Stealth Rock, Sticky Web), which move effects a Substitute blocks (and to what degree), and the switch-decision hazard awareness (a small nudge to switch out for a Defog/Rapid Spin user, and a stronger preference for one when picking a switch-in) once hazards are up on our own side. |
| `build.sh` | Compiles everything above and runs the resulting binary. |

## What this does *not* cover

This only models the specific rules above — it's a starting point, not full
coverage of the trainer AI. The real AI's move-scoring logic in `script.s` is
a large custom bytecode interpreted by `trainer_ai.c`, covering hundreds of
move-specific and ability-specific rules; only the ones directly relevant to
the bugs fixed so far are modeled here. Add more `AI_*` functions to
`ai_model.c` (mirroring more of `script.s`'s logic) as you want to cover more
of it.
