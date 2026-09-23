# phmode — changes from the base game

This file tracks every intentional deviation this romhack makes from the
`pokeplatinum` (Pokémon Platinum, US rev 1) decompilation it is built on. It is
grouped by area rather than by commit. When you change gameplay, add an entry
here in the same commit.

Upstream baseline: `pokeplatinum` `main` at commit `064f2900e` ("tools: Sync
nitrorom from upstream (#1276)"). Everything below is on top of that.

---

## Intro

### Rowan's opening lines
`res/text/rowan_intro.json` — two lines in the very first cutscene (before
you even name your character), read by `src/applications/rowan_intro/
rowan_intro_app.c`:
* `RowanIntro_Text_HelloThere`: "Welcome to the world of Pokémon!" →
  "Welcome to Platinum: Hard Mode."
* `RowanIntro_Text_MyNameRowan`: "However, everyone just calls me the
  Pokémon Professor." → "I hear you're up for a challenge."

### Rowan's intro drastically shortened
The optional "Control Info"/"Adventure Info" tutorial menu, and the whole
"here's a Poké Ball, touch it" demo (Rowan explaining that Pokémon exist,
having the player touch a Poké Ball on the touchscreen, a Buneary popping out
in a flash/jump animation, "we live alongside Pokémon as friends") are cut
entirely from the intro cutscene - it now goes straight from "My name is
Rowan... I hear you're up for a challenge." to "Now, why don't you tell me a
little bit about yourself?".

* `res/text/rowan_intro.json` - removed the now-unused
  `RowanIntro_Text_ControlInfo0-3`, `_ControlInfoDsIcon`,
  `_ControlInfoUnderstood`, `_ControlInfoUseTouchscreen`,
  `_InfoAnythingElse`, `_AdventureInfo0-5`, `_WidelyInhabited`,
  `_HavePokeBall`, `_PokeBallUseTouchscreen`, `_LiveAlongsidePokemon`,
  `_ChoiceControlInfo`, `_ChoiceAdventureInfo`, `_ChoiceNoInfo` entries.
* `src/applications/rowan_intro/rowan_intro_app.c` - `RI_STATE_DIALOGUE_ROWAN_INTRO`
  now transitions straight to `RI_STATE_DIALOGUE_ABOUT_YOURSELF`. Removed the
  ~50 now-unreachable state-machine cases in between
  (`RI_STATE_MOVE_ROWAN_RIGHT_FOR_INFO` through `RI_STATE_BETWEEN_DIALOGUE_DELAY`,
  covering the info choice menu, both info tutorials, and the whole Poké
  Ball/Buneary demo), plus everything exclusively used by them:
  `RowanIntro_DisplayTextBlock`, `RowanIntro_WasPokeballOpened`,
  `RowanIntro_LoadBunearySprite`, `RowanIntro_LoadPokeballTilemap`,
  `RowanIntro_AnimateBuneary`/`_BlendSpritePalette`,
  `RowanIntro_SetBunearyLayerPriority`, the `sControlInfoTextWindow`/
  `sAdventureInfoTextWindow`/`sInfoChoiceBoxWindowTemplate` window templates,
  `sInfoChoiceInfos`, `sBunearyLoadTilemapRectRawData`, the `CC_INFO` case in
  `RowanIntro_ChoiceBox` (the `CC_YESNO`/`CC_RIVAL_NAMES` cases it's shared
  with are untouched), and the now-fully-unused `bunearyAnimState`/
  `displayTextBlockState`/`bunearyAnimCarryover`/`bunearyParabolaCoeff`/
  `bunearyAnimUpdateCounter`/`bunearyPaletteBlendUpdateCounter`/
  `bunearyPalette`/`bunearyBlendedPalette` fields on `RowanIntro` (and their
  alloc/free calls). None of this touched anything with a gameplay side
  effect - the whole sequence was purely a visual/audio demo with no item
  grant or flag set, confirmed by reading through it before deleting it.

## Quality-of-life

### Text speed
`src/game_options.c` — `Options_TextFrameDelay` remaps every Text Speed option
one tier faster than vanilla:

| Option shown | Vanilla frame delay | Now |
| --- | --- | --- |
| Slow | `TEXT_SPEED_SLOW` | `TEXT_SPEED_NORMAL` |
| Normal | `TEXT_SPEED_NORMAL` | `TEXT_SPEED_QUICK` |
| Fast | `TEXT_SPEED_FAST` | `TEXT_SPEED_FAST` (unchanged) |

"Fast" was briefly set to `TEXT_SPEED_INSTANT` (0). That is reverted: instant
makes `Text_AddPrinter` render synchronously and return `MAX_TEXT_PRINTERS`
instead of a printer ID, which the options menu then indexes out of bounds and
crashes. Do not set any option to `TEXT_SPEED_INSTANT`.

### Movement speed
`src/unk_020655F4.c` — walking and running both cover a tile in half the usual
number of frames, so the overworld moves at roughly 2×:

* Walk (`MovementAction_WalkNormal*_Step0`): `FX32_CONST(4)` × 4 frames with
  `MAP_OBJ_UNK_A0_04` (was `FX32_CONST(2)` × 8 with `A0_03`).
* Run (`MovementAction_Run*_Step0`): `FX32_CONST(8)` × 2 frames with
  `MAP_OBJ_UNK_A0_09` (was `FX32_CONST(4)` × 4).

`src/overlay005/ov5_021EB1A0.c` — `ov5_021EBEFC` (the player's run animation
handler) advances the stride cycle at `FX32_ONE * 2` so the animation keeps pace
with the faster run step instead of gliding.

### Following NPCs keep up when you run

`src/unk_02069BE0.c` — the player is `MOVEMENT_ACTION_RUN` (2 frames/tile), but
the follow-the-player code translated that to a slower walk action for the
follower, so Barry / rivals / dungeon companions fell a tile further behind on
every running step.

* `sub_02069D50` (`MOVEMENT_TYPE_FOLLOW_PLAYER`) — remapped the player's
  `MOVEMENT_ACTION_RUN_*` to `MOVEMENT_ACTION_WALK_FASTER_*` (also 2 frames/tile)
  instead of `..._WALK_FAST_*` (4 frames/tile).
* `sub_0206A034` (`MOVEMENT_TYPE_FOLLOW_PARTNER_TRAINER`, e.g. the escorted
  double-battle partners) — this stepped at `WALK_NORMAL` unconditionally; now
  uses `WALK_FASTER` while `PlayerAvatar_IsRunning` is true.
* `sub_0206A3BC` (the `MOVEMENT_TYPE_055`–`063` follow path) — the
  `PLAYER_ACTION_SPEED_FAST` case likewise steps at `WALK_FASTER` when the player
  is actually running.

The follower still trails one tile behind by design; it just no longer drifts
further back the longer you hold the run button.

### General pacing pass: trainer approach, battle-start transitions, and several artificial delays

A broad pass aimed at the animations/delays the player sees most often -
every trainer encounter, every battle's opening, and every battle win -
rather than one-off cutscenes.

**Trainer approach speed.** `src/trainer_encounter.c`:
* `ApproachingTrainerTask_StepTowardsPlayer` now walks the trainer toward the
  player using `MOVEMENT_ACTION_WALK_FAST_NORTH` instead of
  `..._WALK_NORMAL_NORTH` - a brisker walk, same pose, no switch to a running
  animation. `MOVEMENT_ACTION_WALK_FASTER_NORTH` (the tier used for the
  player's own bike speed and for followers keeping up with a running player,
  see "Following NPCs keep up when you run" above) was tried first but read
  as too fast for a trainer walking up to you, so this uses the slower of the
  two "fast walk" tiers instead.
* `ApproachingTrainerTask_DelayCheckNextToPlayer` (a pure pause before the
  trainer starts walking - the "spotted you" exclamation bubble has already
  finished playing by this point) cut from 30 ticks to 10.
* `ApproachingTrainerTask_DelayNextToPlayer` (a pure pause once the trainer
  arrives next to the player, before they turn to face each other) cut from
  8 ticks to 3.

**Battle-start transition, field side.** `src/overlay005/encounter_effect.c`'s
`EncounterEffect_FlashTask` - the screen-flash fade shared by every single
encounter type in the game (wild, trainer, gym leader, Elite Four, Champion,
legendary, everything routes through `EncounterEffect_Flash`) - had its fade
durations cut (8→5 and 3→2 frames), speeding up the very start of every
encounter with one change instead of touching each encounter type
individually.

On top of that, `src/overlay005/encounter_effect_core.c`'s interpolation
frame counts (pokeball scale/spin, screen-slice zoom, camera pull-in) were
cut by roughly a third across the wild Grass/Cave encounter effects
(`GRASS_HIGHER_LEVEL_INTERPOLATION_FRAMES` etc., 6→4 and 12→8) and all six
regular trainer encounter effects (`EncounterEffect_Trainer_{Grass,Water,Cave}_
{LowerLevel,HigherLevel}`), each a same-shape-but-faster version of its
existing pokeball-spin/zoom-in animation. One clear outlier was also fixed
regardless of the general pass: `EncounterEffect_Trainer_Cave_HigherLevel`'s
final camera zoom (`unk_270`) was 64 frames - over a second, and 6-10x longer
than the equivalent step in every sibling effect - cut to 24. Wild Water and
the various one-off effects (gym leaders, Elite Four, Champion, legendary,
Galactic grunts/bosses, Battle Frontier, double battles) were deliberately
left alone: they're rare enough, and different/more bespoke enough in
structure, that the risk of a subtle visual glitch outweighed the benefit for
this pass.

**Battle-start transition, in-battle side.**
`res/battle/scripts/subscripts/subscript_start_encounter.s` (runs at the
start of every wild, trainer, Safari, and Pal Park encounter) had every
`WaitTime` cut by roughly a quarter (122→92, 96→72, 112→84). These pair with
the sprite pop-in/ball-throw animations from `PlayEncounterAnimation`/
`PokemonSlideIn`, which aren't queryable from script code the way most other
waits in this codebase are, so this is a moderate trim rather than an
aggressive one, to stay clear of visibly cutting the animation off early. The
same reasoning and cut applies to the two other high-frequency send-out
waits: `subscript_switch_pokemon.s` and `subscript_replace_fainted.s`
(72→54 each, for every voluntary switch and every fainted-mon replacement).

**Artificial (non-animation) delays removed or shortened:**
* `src/battle/battle_script.c`, `GET_EXP_MSG_DELAY` - a flat 7-frame pause
  after "gained N Exp. Points!" finishes printing, before the EXP gauge even
  starts filling, on every single battle win - cut to 1 (the minimum
  non-zero value its decrement-to-zero check can safely use).
* `src/battle/battle_script.c`, `CATCH_MON_DELAY` - several catch-sequence
  pauses cut, some pure dead time and some layered on top of the ball
  animation itself: the pre-shake pause (23→8), the pause after the 3rd
  successful shake before "Gotcha!" (12→4), the pause between shakes
  layered on the shake animation (12→6), the pause after "Gotcha!" before
  the catch is finalized (30→10, also layered on the ball animation), and
  the pause after "added to the Pokédex" before the palette fade (30→2,
  pure dead time).
* `src/battle/battle_display.c`, `Task_WaitForAlertMessagePrint` - a fixed,
  non-skippable 40-frame pause after an alert message (e.g. "That move
  can't be used", "Items can't be used here.") finishes printing, before
  auto-acknowledging it - cut to 20. Happens on every invalid
  move/item/target selection.

**Faster HP and EXP gauges.** `src/battle/healthbox.c`:
* `HEALTHBOX_HP_GAUGE_DRAIN_RATE` (already raised once from 1 to 4 - see
  its own comment) raised again to 8. This is arguably the single
  most-repeated animation in the game, since it plays on every damaging hit
  in every turn of every battle.
* The EXP gauge fill (`HealthBox_DrawGauge`) computed exactly enough EXP
  consumed per frame to move the gauge 1 pixel per frame, taking as many
  frames as pixels it needs to travel (up to 96) regardless of how much EXP
  was actually gained. That per-frame rate is now doubled, so it moves ~2
  pixels per frame instead, halving how long every post-battle EXP fill
  takes.

Left uninvestigated for a future pass: the generic `waittime`/
`waitbuttonabtime` script opcodes are used throughout the individual
move-effect scripts under `res/battle/scripts/`, each with its own
hardcoded frame count baked into that specific move's script - a full
inventory of those call sites (as opposed to the shared subscripts covered
above) was out of scope here.

### Battle style locked to "Set"
* `src/game_options.c` — `Options_Init` defaults `battleStyle` to
  `OPTIONS_BATTLE_STYLE_SET` on a new game.
* `src/applications/options_menu.c` — `ProcessMainInput` ignores left/right on
  the Battle Style row, so the player cannot switch it back to "Switch".

### Register up to 6 key items to Y
The Y button holds up to **six** registered key items instead of one. With one
registered, pressing Y uses it as before; with two or more, Y opens a small
drop-down of the registered items (plus CANCEL) and uses the one you pick. The
save layout is untouched — this is fully compatible with existing saves.

* `generated/vars_flags.txt` — six unused game vars renamed to
  `VAR_REGISTERED_KEY_ITEM_0..5`; each holds one registered item ID (0 = empty).
* `src/registered_items.c` / `include/registered_items.h` — a small module over
  those vars: `RegisteredKeyItems_GetSlot` / `Count` / `IsRegistered` /
  `Register` (appends to the first free slot; no-op if full or already there) /
  `Unregister` (removes and keeps the list packed). The `Bag` struct's now-unused
  `u32 registeredItem` field is left in place so the bag save block is
  byte-for-byte identical.
* `src/applications/bag/main.c` — the key-items list marks every registered item
  with the Y icon; the item menu offers **Register** only while fewer than 6 are
  set and **Deselect** for an already-registered one.
* `src/item_use_functions.c` — `sub_02069238` branches on
  `RegisteredKeyItems_Count`: 0 does nothing, 1 dispatches directly
  (`RegisteredItem_UseInField`, factored out of the old body), 2+ opens
  `RegisteredItemsMenu_Task` (a `Menu` in a field-task window). The chosen item
  is stashed in `sPendingRegisteredItem` and dispatched the next frame by
  `RegisteredItem_ConsumePendingUse`, called at the top of `FieldInput_Process`
  and `FieldInput_Process_BattleTower` (`src/overlay005/field_control.c`) — the
  picker must fully close before the item-use function can create its own task.
* `res/text/bag.json` — `Bag_Text_RegisteredMenuCancel` ("CANCEL").
* `platinum.us/main.lsf` — `src_registered_items.c.o` added to `Static main`. A
  new `src/*.c` file must be listed here or `makelcf` gives it no section; the
  linker then places it outside the resident ARM9 static image, and the first
  overlay that calls into it (the bag app, via the key-items list draw callback)
  jumps to an unmapped address and crashes. This is why opening the Key Items
  pocket froze the game.

### Enemy health box shows HP numbers
The single-battle opponent's health box now prints the raw `current / max` HP
below its gauge, the same as the player's, inside an enlarged box frame.

* `src/battle/healthbox.c` — `HealthBox_DrawInfo` no longer strips the
  current-HP / max-HP number flags for `HEALTHBOX_TYPE_ENEMY_SOLO` (only the EXP
  bar). The double-battle enemy slots (`HEALTHBOX_TYPE_ENEMY_SLOT_1` / `_2`,
  which have no room for digits) are unchanged.
* The enemy-solo digit slots were repositioned to a clean tile row directly
  under the gauge: `sCurrentHPNumberVRAMTransfer` index 1 is now split across
  the two 64×64 sprite halves (`{0x5C0,0x40}` + `{0xD00,0x20}`),
  `sMaxHPNumberVRAMTransfer` index 1 is `0xD40`, and the "/" divider — which,
  unlike the player's solo box, is *not* baked into the enemy graphic — is
  blitted from `HEALTHBOX_PART_SLASH` at `sHPDisplaySlashVRAMTransfer` index 1
  (`0xD20`) each time the max HP is drawn.
* `res/graphics/battle/healthbox/enemy.png` — the box body is extended ~8 px
  downward (extra gauge-fill rows plus the bottom border moved down) so the new
  number row sits neatly within the frame. The pointed tail stays at the HP-bar
  row.

### HP gauge drains faster
`src/battle/healthbox.c` — `HealthBox_DrawGauge` passes a new
`HEALTHBOX_HP_GAUGE_DRAIN_RATE` (4) into `UpdateGauge` in place of a hardcoded
`1`. Once a Pokémon's max HP exceeds the gauge's pixel width (`HEALTHBOX_HP_CELL_COUNT
* HEALTHBOX_NAME_BLOCK_COUNT_X` = 48, i.e. almost every real Pokémon), `UpdateGauge`
drains this many raw HP points off the animated gauge per frame instead of 1, so
a big hit's health-bar animation finishes in a quarter of the time. Pokémon with
low enough max HP to fall under the pixel-interpolation branch (`max < corrected`)
were already fast and are unaffected — this only speeds up the common case.

### Summary screen: EV / IV view and nature colours
`src/applications/pokemon_summary_screen/` (`main.c`, `window.c`, `main.h`) — on
the Skills page:

* Hold **L** to show effort values in place of the six stats (drawn blue).
* Hold **R** to show individual values (drawn red).
* With neither held, each stat number is tinted by the Pokémon's nature: red for
  the boosted stat, blue for the hindered stat, default colour for a neutral
  nature.

### Battle Tower Judge: "Holy zipcode!"

`res/text/battle_tower.json` — the IV Judge's "decent all around" verdict, which
he gives when a Pokémon's total IVs are ≤ 90 (i.e. average ≤ 15), now opens with
**"Holy zipcode!"** instead of "I see, I see...". Same threshold and same rest of
the readout — just a louder reaction to a bad roll. Text-only, no script change
(the `GoToIfLe VAR_0x8001, 90` branch already lines up with a 15 average).

### PC: "Inflict status" option
The main PC boot menu ("Someone's PC / [Player]'s PC / Switch off …") gains an
**INFLICT STATUS** entry: pick a party Pokémon, then choose Cure / Sleep /
Poison / Badly poison / Burn / Freeze / Paralyze from a list, and the condition
is written to that Pokémon.

* `res/field/scripts/scripts_common.s` — `CommonScript_AddMenuEntryInflictStatus`
  (added to both `CommonScript_PCMenu` and `CommonScript_PCMenuHallOfFame`) and
  `CommonScript_PCInflictStatus`, which reuses `SelectMoveTutorPokemon` /
  `GetSelectedPartySlot` for the party picker and a single-column `ShowMenu` for
  the condition. The condition list is built with `AddMenuEntry` (u16 string ID)
  rather than `AddMenuEntryImm` / `AddListMenuEntry`, whose script commands
  truncate the string ID to a byte and so can't reach the new
  `menu_entries.json` strings past index 255.
* New script command `InflictPartyMonStatus partySlot, choice`
  (`asm/macros/scrcmd.inc`, `include/data/scripts/scrcmd.h`,
  `include/scrcmd_party.h`, `src/scrcmd_party.c`) sets `MON_DATA_STATUS`
  (`MON_CONDITION_*`) on the chosen party member.
* Menu strings in `res/text/menu_entries.json`.

### HM moves are forgettable
The "HM moves can't be forgotten!" restriction is removed, so Cut, Fly, Surf,
etc. can be replaced from the "which move should be forgotten?" prompt like any
other move — no trip to the Move Deleter required.

* `src/applications/pokemon_summary_screen/main.c` — the HM guard in the
  move-select A-press handler (`HandleInput` for `SUMMARY_STATE_SELECT_MOVE`) is
  deleted; an HM slot is selected like any other. This covers level-up learning,
  TM/HM learning, and the Move Deleter, which all route through this screen.
* `src/battle_sub_menus/battle_party.c` — `CheckSelectedMoveIsHM` always returns
  `FALSE`, so a move learned mid-battle can replace an HM move too.
* `Item_IsHMMove` itself is unchanged — it is still used to keep HM *items* from
  being consumed when taught and to pick the "Booted up an HM" bag message.

### Honey trees trigger the encounter instantly
Slathering a Honey Tree now starts the Honey Tree battle right away instead of
having to leave and come back 6+ hours later.

* `src/overlay005/honey_tree.c` — `HoneyTree_SlatherTree` marks the tree
  encounter-ready immediately (`minutesRemaining = 18 * 60`, the threshold
  `SixHoursSinceSlathered` already treats as "ready"), and a "no encounter" group
  roll is promoted to group A so honey is never wasted. `HoneyTree_Unslather`
  (called from `CreateWildMon_HoneyTree` when the Pokémon spawns) still clears the
  tree afterwards, so it goes bare again and can be re-slathered.
* `res/field/scripts/scripts_common.s` — `CommonScript_SlatherHoneyTree` re-reads
  the tree status after slathering and jumps straight to
  `CommonScript_HoneyTreeEncounter`. The slot/group/shake rolls and the
  Munchlax-tree logic are unchanged, so *which* Pokémon appears is still random;
  only the wait is gone.

### "Hatch" option on eggs in the party menu
Selecting an egg in the field party menu (START → Pokémon) now offers **HATCH**
alongside Summary / Switch / Cancel, which hatches it on the spot instead of
walking it out.

* `include/applications/party_menu/defs.h` — new `PARTY_MENU_STR_HATCH` (inserted
  before `PARTY_MENU_STR_MOVE0`, kept aligned with the matching new
  `ACTION_HATCH` before `ACTION_CUT` in `context_menu.c`) and new exit code
  `PARTY_MENU_EXIT_CODE_HATCH_EGG`.
* `res/text/party_menu.json` — new `PartyMenu_Text_Hatch` ("HATCH"), appended;
  loaded in `PartyMenu_LoadContextMenuStrings` (`windows.c`).
* `src/applications/party_menu/main.c` — `GetContextMenuEntriesForPartyMon` adds
  the Hatch entry to the egg branch.
* `src/applications/party_menu/context_menu.c` — `PartyMenu_SelectHatch` zeroes
  the egg's remaining egg-cycles (stored in friendship, the same field the
  daycare step check decrements) so `Party_GetFirstEgg` selects it, then exits
  with `PARTY_MENU_EXIT_CODE_HATCH_EGG`.
* `src/start_menu.c` — `StartMenu_ExitPartyMenu` handles that code by jumping the
  field task to `StartMenu_HatchEggTask`, which runs `CommonScript_HatchEgg`
  (`COMMON_SCRIPTS` 31) — the exact "Oh?" → hatch cutscene the daycare step
  trigger uses.

Only the START-menu party screen is affected; the selection/bag/daycare party
modes are unchanged.

### "Rename" option in the party menu and PC boxes
Selecting a non-egg Pokémon in the field party menu (START → Pokémon) or in a
PC Storage Box now offers a **RENAME** option that opens the same nickname-entry
keyboard the Name Rater NPC uses, without a trip to Route 213.

* **Party menu** (`include/applications/party_menu/defs.h`,
  `src/applications/party_menu/{context_menu.c,main.c,windows.c}`) — new
  `PARTY_MENU_STR_RENAME` (inserted before `PARTY_MENU_STR_MOVE0`, kept aligned
  with a new `ACTION_RENAME` before `ACTION_CUT` in `context_menu.c`, the same
  pairing convention `PARTY_MENU_STR_HATCH`/`ACTION_HATCH` already established)
  and a new exit code `PARTY_MENU_EXIT_CODE_RENAME`, appended after
  `PARTY_MENU_EXIT_CODE_HATCH_EGG`. `GetContextMenuEntriesForPartyMon` adds the
  entry for every non-egg party mon, right after Switch. `PartyMenu_SelectRename`
  records the slot and exits with the new code. New string `PartyMenu_Text_Rename`
  ("RENAME") in `res/text/party_menu.json`.
* **Bug fix**: `StartMenu_ExitPartyMenu` (`src/start_menu.c`) originally handled
  this by calling `sub_0203DFE8` — the same low-level helper
  `ScrCmd_OpenPokemonNamingScreen` (the Name Rater's script command) uses —
  which opens the naming screen by pushing a new frame onto the FieldTask call
  stack (`FieldTask_InitCall`) rather than as a tracked child process. That's the
  right tool for a script's own task to resume linearly afterward, but the start
  menu's `StartMenu_ApplicationRun` re-invokes `menu->callback` on its own once
  `FieldSystem_IsRunningApplication` goes false, and this path never updated
  `menu->callback`/`menu->state` away from `StartMenu_ExitPartyMenu` — so once the
  naming screen closed and control popped back, `StartMenu_ExitPartyMenu` fired a
  second time against an already-freed `menu->taskData`, corrupting the heap
  (symptom: a blank screen right after confirming the new nickname). Fixed by
  opening the naming screen the same way Summary/Pokedex do —
  `FieldSystem_StartChildProcess(fieldSystem, &gNamingScreenAppTemplate, nameArgs)`
  — and adding a dedicated `StartMenu_ExitRenameMon` callback (set via
  `StartMenu_SetCallback`) that applies the result once the screen actually
  closes, then returns to the field the same way `StartMenu_ExitPokedex` does.
  The party slot is threaded through via `menu->additionalTaskContext`, the same
  mechanism the Fly-destination and TM/level-up move-overwrite flows already use
  for an extra value alongside `menu->taskData`.
* **PC boxes** (`include/applications/pc_boxes/{struct_box_menu.h,box_app_manager.h}`,
  `src/applications/pc_boxes/{box_menu.c,box_app_manager.c}`) — new
  `BOX_MENU_RENAME_MON`, appended immediately before `BOX_MENU_FIRST_MARKING` so
  every existing `BoxMenuItem`'s text-bank offset (`24 + value`, see
  `ov19_021DB2FC.c`) stays put; a matching string is appended to
  `res/text/pokemon_storage_system.json` (index 80,
  `PokemonStorageSystem_Text_RenameMon`, "RENAME"). `BoxMenu_FillTopLevelMenuItems`
  adds the entry for every non-egg mon in the default (move-mons) mode, right
  after Item — exactly `MAX_MENU_ITEMS` (8) entries in the worst case, so no
  overflow. `BoxAppMan_RenameMonAction` (modeled on the existing
  `BoxAppMan_RenameBoxAction`, which already renames the *box itself* the same
  way) allocates a fresh `NAMING_SCREEN_TYPE_POKEMON` `NamingScreenArgs` (a new
  `BoxApplicationManager::monRenameArgs` field, since the existing
  `namingScreenArgs` field is permanently configured for box names) for whichever
  mon is currently under the cursor (`boxApp.pcMonPreview.mon`, which already
  transparently points at the right `BoxPokemon` whether that mon is sitting in
  a box or in the party column), applies the result via
  `BoxPokemon_SetValue(..., MON_DATA_NICKNAME_AND_FLAG, ...)` when the naming
  screen reports `NAMING_SCREEN_CODE_OK` (a cancelled entry changes nothing),
  and refreshes the on-screen preview nickname before returning to the box.

### Starter is "Met at Rowan's briefcase"

The starter's summary/memo now reads **"Met at Rowan's briefcase"** instead of
"Route 201".

* `res/text/location_names.json` — new `LocationNames_Text_RowansBriefcase`
  ("Rowan's briefcase"), appended → id 126, so no existing location id shifts.
  Met-location values below 2000 index this bank directly; 126 is stored in the
  Pt/HGSS data block with the DP block flagged "fateful", exactly how vanilla
  Platinum already stores its own locations 112–125 (Iron Ruins, etc.).
* `include/data/scripts/scrcmd.h` — new script command
  `SCRCMD_GIVEPOKEMONWITHMETLOCATION` / `ScrCmd_GivePokemonWithMetLocation`,
  **appended** so every existing opcode keeps its number. It is `GivePokemon`
  plus an explicit met-location argument instead of `MapHeader_GetMapLabelTextID`
  of the current map. Impl in `src/scrcmd_party.c`, macro in
  `asm/macros/scrcmd.inc`.
* `res/field/scripts/scripts_route_201.s` — the starter hand-out uses
  `GivePokemonWithMetLocation … LocationNames_Text_RowansBriefcase …`; the file
  now also includes `res/text/bank/location_names.h`.

Reusable for any other gift Pokémon that should not inherit the map's location.

### Split routes and Mt. Coronet are distinct locations

For the nuzlocke "one catch per area" rule, the areas that vanilla splits into
several map headers but labels identically are now named per-segment. This is a
plain `.mapLabelTextID` change in `include/data/map_headers.h`, so it flows
through *everything* consistently — the name banner (which now pops when you
cross the boundary), the "Met at" location, the journal, the save screen.

* Routes **204 / 205 / 210** → `Route 20x (south)` and `Route 20x (north)`;
  Route **211** → `Route 211 (west)` / `Route 211 (east)`; Route **212** →
  `Route 212 (north)` / `Route 212 (south)`.
* **Mt. Coronet** → `Mt. Coronet (Ext)` for the two outside segments
  (`MT_CORONET_OUTSIDE_NORTH/SOUTH`) and `Mt. Coronet (Int)` for the twelve
  interior headers (1F/2F/3F/4F/5F/6F/B1F rooms + the Iceberg Ruins access
  room). All interior floors share one label, so there is no *new* banner
  between them — a cave already pops its name banner on every floor warp in
  vanilla, that just now reads "Mt. Coronet (Int)". Stepping between inside and
  outside now shows "(Int)" ↔ "(Ext)".
* `res/text/location_names.json` — 12 new entries appended (`…Route204South`
  through `…MtCoronetExt`, ids 127–138), so no existing location id shifts.

For the routes, the seamless-crossing banner fires on
`MapHeader_GetMapLabelTextID` mismatch between the old and new header
(`FieldMap_ChangeZone`), so walking across a route's internal boundary now pops
the "(south)"/"(north)" banner where vanilla stayed silent.

---

## Battle changes

### Bug fix (game-breaking, vanilla decomp bug): Hypnosis (and Sleep Powder/Spore) failing could scramble a Pokémon's status and HP, triggering every residual effect at once
Reported: an opponent's Hoothoot used Hypnosis, it "failed", and immediately
afterward the player's own (fully healthy, non-asleep) Pokémon took a chunk of
damage and *every* residual status effect started firing every turn - Toxic,
Nightmare, Curse, Bind, Bad Dreams, Uproar, Thrash, etc. all "active"
simultaneously on a low-level Pokémon that couldn't possibly have all of
them - with no way to select a move, until it fainted.

Root cause, found via live `EmulatorLog` tracing in no$gba: `subscript_fall_asleep`
(`res/battle/scripts/subscripts/subscript_fall_asleep.s`) is the shared
target-side "does this Pokemon actually fall asleep" script used by Hypnosis,
Sleep Powder, and Spore. With phmode's Grass-immune-to-powder-moves checks
(added in two places in that script), it compiles to 1716 bytes (429 words) -
29 words past `BATTLE_SCRIPT_SIZE_MAX` (400 words/1600 bytes at the time).
`BattleSystem_LoadScript`/`BattleSystem_CallScript` (`src/battle/battle_lib.c`)
read a compiled subscript from its NARC member straight into
`battleCtx->battleScript[BATTLE_SCRIPT_SIZE_MAX]` with `NARC_ReadWholeMemberByIndexPair`
and only a `GF_ASSERT` guarding the length - and `GF_ASSERT` compiles to a
no-op unless built with `PM_KEEP_ASSERTS`, which normal builds are not. In
`BattleContext` (`include/battle/battle_context.h`), `battleMons[MAX_BATTLERS]`
sits immediately after `battleScript[]`, so the 29-word overflow silently
wrote the tail of the compiled script's bytecode into `battleMons[0]`,
clobbering its `status`, `statusVolatile`, and `curHP` fields with
reinterpreted opcode bytes. That's what made every residual condition read as
active at once (`statusVolatile` ended up a near-all-1s value) and why HP had
already dropped before any of those conditions had a chance to fire "for
real" - the corruption itself did the damage. The "Hypnosis fails" trigger
wasn't circumstantial after all; it's the one path that actually loads this
specific oversized script.

* `include/constants/battle.h` - `BATTLE_SCRIPT_SIZE_MAX` raised from 400 to
  500 words, giving headroom (subscript_badly_poison is already the next
  closest at 1428/1600 bytes and will keep growing as more phmode-specific
  branches get added to residual-effect scripts).
* `src/battle/battle_lib.c`, `BattleSystem_CallScript` - its bounds check now
  uses the `BATTLE_SCRIPT_SIZE_MAX` macro instead of a separate hardcoded
  `400 * 4`, so the two can't drift out of sync again.

While investigating this, a second, real but independent latent bug was found
and fixed in the same area: `BattleControllerPlayer_CheckMonConditions` (and
its siblings `CheckFieldConditions`/`CheckSideConditions`) in
`src/battle/battle_controller_player.c` is a resumable state machine with two
counters that must move together - which *condition type* is being checked
(`monConditionCheckState`) and which *battler* is currently being checked
(`monConditionCheckTemp`). Only the type-state half was ever reset in
`BattleContext_Init` (`src/battle/battle_lib.c`); the battler-index half was
only ever reset by the check function's own natural completion. Since
`BattleContext_Init` also runs from `BattleControllerPlayer_MoveEnd` - reachable
mid-battler during a residual-effects sweep, because applying residual damage
shares the same damage-and-faint-check plumbing as a real move hit - hitting
that path could reset the condition-type state back to the start while
leaving the battler index wherever it was, restarting the condition list for
that battler indefinitely. This did not turn out to be the cause of the
reported symptom (it requires interrupting a sweep already in progress, not a
fresh one), but it's a genuine bug in its own right and the fix is kept:
`BattleContext_Init` now also resets `fieldConditionCheckTemp`/
`monConditionCheckTemp`/`sideConditionCheckTemp` alongside their respective
`*CheckState` fields.

### No items on your own Pokémon in battle
`src/battle/battle_controller_player.c` — in the item-select command state, any
bag item whose category is not `ITEM_BATTLE_CATEGORY_POKE_BALLS` is rejected with
the "Items can't be used here." message. Poké Balls still work; potions, status
heals, X items, etc. do not.

### Hail is now Snow
* `res/moves/hail/data.json` — the move is renamed **Snow** with a new
  description; still `CLASS_STATUS` / `TYPE_ICE`, stats otherwise unchanged.
* `src/battle/battle_script.c` — `BtlCmd_EndOfTurnWeatherEffect` no longer deals
  end-of-turn chip damage to non-Ice types. It still heals Ice Body.
* `src/battle/battle_lib.c` — `BattleSystem_CalcMoveDamage` gives Ice-type
  Pokémon +50% Defense while the hail/snow field condition is active (mirrors
  sandstorm's Rock-type SpDef boost).
* `res/text/battle_strings.json`, `res/text/unk_0329.json`,
  `res/text/unk_0662.json` — weather strings reworded from "hail" to "snow".

### No EVs from battle
`src/battle/battle_script.c` — `BattleScript_CalcEffortValues` returns
immediately. Defeating a Pokémon grants no effort values at all, regardless of
species, held item (Macho Brace / power items), or Pokérus. Delete the early
`return;` to restore vanilla EV gain.

### New battle terrains (as overworld weather)
Three custom field conditions were added, each driven by an `OVERWORLD_WEATHER_*`
value set in a map header (or by a weather script) and surfacing in battle as a
`FIELD_CONDITION_*` bit. Constants live in
`include/constants/overworld_weather.h` and `include/constants/battle/condition.h`.
Battle anim/subscript wiring is in `res/battle/scripts/` and `src/battle/`.

* **Electric Terrain** (`f0c66dd35`) — grounded Pokémon cannot fall asleep
  ("can't sleep because of the electricity!"), Rest fails, and a sleeping
  Pokémon that switches in is woken immediately. Electric-type moves deal ×1.3.
* **Psychic Terrain** (`33797b467`) — increased-priority moves are blocked
  against grounded targets (`Move_BlockedByPsychicTerrain`). Psychic-type moves
  by a grounded attacker deal ×1.3. This commit also added the shared
  `Battler_IsGrounded` helper used by the terrains.
  `subscript_move_fail_psychic_terrain` now prints "{mon} used {move}!"
  (`PrintAttackMessage`) before "{target} is protected by the psychic terrain!",
  so a blocked priority move is not swallowed silently.
* **Distortion Terrain** (`de20f3c30`) — every stat-stage change on the field is
  inverted, Contrary-style, for all battlers whether grounded or not.

**Weather and terrain share one slot.** A new `FIELD_CONDITION_ATMOSPHERE`
(= `FIELD_CONDITION_WEATHER | FIELD_CONDITION_TERRAIN`, in
`include/constants/battle/condition.h`) is cleared whenever weather or terrain is
set, so setting any weather removes any terrain and setting any terrain removes
any weather. The weather-move effect scripts (`effect_script_0136/0137/0115/0164`)
and the weather-ability subscripts
(`subscript_{drizzle,drought,sand_stream,snow_warning}`) now clear
`FIELD_CONDITION_ATMOSPHERE` instead of `FIELD_CONDITION_WEATHER`; the terrain
subscripts (`subscript_{psychic_surge,electric_surge,distortion_surge}` and the
`subscript_overworld_*_terrain` trio) gained a matching clear before they set
their bit. **Trick Room is deliberately not part of `FIELD_CONDITION_ATMOSPHERE`**
(bits 16-18 / 24), so no weather or terrain ever overrides it.

**Overworld Trick Room is permanent.** New `FIELD_CONDITION_TRICK_ROOM_PERM`
(bit 24, mirroring Space Warp's `GRAVITY_PERM`): `subscript_overworld_trick_room`
sets that bit instead of the 5-turn `TRICK_ROOM_INIT` counter, and the
end-of-turn countdown (`SIDE_COND_CHECK_STATE_TRICK_ROOM`) skips it entirely, so
a battle whose map `.weather` is `OVERWORLD_WEATHER_TRICK_ROOM` stays under Trick
Room for its whole length. `FIELD_CONDITION_TRICK_ROOM` now = counter | perm, so
the Trick Room *move* still toggles it off (`effect_script_0259` clears the whole
mask) — that is the only way to end it.

### Sticky Web is a per-side hazard
`FIELD_CONDITION_STICKY_WEB` (a field-wide "weather" bit) was replaced with the
per-side `SIDE_CONDITION_STICKY_WEB (1 << 11)` in
`include/constants/battle/condition.h`. `BtlCmd_CheckStickyWeb`, the end-of-turn
field-condition loop, `subscript_weather_continues` and the Weather Ball param
code were repointed at the new side condition (and the leftover Weather Ball
sticky-web power halving was dropped). Overworld Sticky Web
(`OVERWORLD_STICKY_WEB`) now webs the **player's** side only —
`BattleSystem_TriggerEffectOnSwitch` sets `SIDE_CONDITION_STICKY_WEB` on
`BATTLE_SIDE_PLAYER`, and `subscript_overworld_sticky_web` announces it and walks
the speed order slowing every grounded player-side battler (so the initial lead
is slowed at battle start, not just later switch-ins). `ov5_021D5F7C` (the
overworld weather-visual entry point) now renders any battle-only weather value
(≥ 31: Harsh Sun, Trick Room, the terrains, Sticky Web) as clear instead of
tripping its `< 31` assert.

The speed drop is now a single message. A new `SIDE_EFFECT_TYPE_STICKY_WEB` is set
by `BtlCmd_CheckStickyWeb`, and `BtlCmd_ChangeStatStage` buffers
`BattleStrings_Text_PokemonIsSlowedByStickyWeb` (reworded to "{mon} was caught in
a sticky web!", with new `_Wild`/`_Foe` variants) for that type, so the switch-in
subscripts no longer print their own line on top of "{mon}'s Speed fell!".

`BtlCmd_ChangeStatStage`'s stat-drop prevention (Mist, Clear Body, White Smoke,
"won't go lower", Contrary/Defiant) is normally gated behind
`attacker != sideEffectMon`, but the switch-in flow leaves `attacker` pointing at
the caught Pokémon itself. That gate now also opens for
`SIDE_EFFECT_TYPE_STICKY_WEB`, so a Clear Body / White Smoke lead (or one behind
Mist) walking into Sticky Web is correctly spared the Speed drop with the usual
"prevents stat loss!" message.

### Defog and Rapid Spin clear every hazard, plus Tailwind and Gravity
A shared battle-script command `BlowAwayHazards` (`asm/macros/btlcmd.inc`,
`include/data/scripts/btlcmd.h`, `src/battle/battle_script.c`) clears, one
condition per invocation and re-running until nothing is left: Spikes, Toxic
Spikes, Stealth Rock, Sticky Web and Tailwind **on both sides of the field**,
then `FIELD_CONDITION_GRAVITY` (the Gravity move's counter *and* Space Warp's
permanent bit, announced via `subscript_gravity_end` — "Gravity returned to
normal!"). Tailwind removal is announced with `subscript_tailwind_end` ("… 
tailwind petered out!").

`subscript_defog.s` now calls `BlowAwayHazards` for its hazard/Tailwind/Gravity
step (its "did anything happen" guard checks both sides) while keeping its own
screen / Mist / Safeguard clear on the target side, the evasion drop, and the
deep-fog message. `BtlCmd_RapidSpin` (`src/battle/battle_script.c`) is trimmed to
just the user's Bind / Leech Seed break, and `subscript_rapid_spin.s` calls
`BlowAwayHazards` afterwards. Clearing both sides is a deliberate deviation from
the modern user-side-only Rapid Spin. Palkia's Space Warp only re-applies gravity
when it next switches in.

### AI scores terrain like weather
`generated/ai_weather_types.txt` gains `AI_WEATHER_TERRAIN`;
`AICmd_LoadCurrentWeather` reports it when a terrain (and no real weather) is
active, and the Expert-tier Rain Dance / Sunny Day / Hail handlers in
`src/battle/trainer_ai/script.s` now add +1 for overriding an active terrain the
same way they do for overriding another weather.

### AI move-choice now accounts for each terrain's actual mechanical effect
The change above only covers weather-setting moves treating terrain like
weather. This covers the terrains' own gameplay effects (sleep prevention,
priority blocking, stat inversion) actually feeding into move scoring, all in
`src/battle/trainer_ai/script.s` unless noted:

* **Electric Terrain** — `Basic_CheckCannotSleep` (used for every direct
  sleep-inducing move via `BATTLE_EFFECT_STATUS_SLEEP`, and for Yawn via
  `BATTLE_EFFECT_STATUS_SLEEP_NEXT_TURN`, since both already routed through
  this same handler) gets a new branch: if Electric Terrain is active and the
  target is grounded, score -10 — the same penalty already used for "this
  can't work" cases like an already-statused target, matching
  `subscript_fall_asleep.s`'s own real block. `Expert_Rest` gets the same
  check against the *attacker* instead (Rest sleeps the user, not the
  target), ahead of its existing HP/speed-based scoring, matching
  `subscript_rest.s`. "Grounded" is approximated the same way in both
  (mirroring `BattlerIsGrounded` in `battle_lib.c`, minus a Magnet Rise check
  the AI script has no way to make): Gravity active, or a held
  `HOLD_EFFECT_SPEED_DOWN_GROUNDED` item (Iron Ball) forces grounding; absent
  those, Levitate or a Flying typing means airborne; otherwise grounded.
* **Psychic Terrain** — a new check at the very top of `Basic_Main` (ahead of
  the existing OHKO-move special case, now split out as `Basic_CheckOHKO`):
  if Psychic Terrain is active, the move has positive priority, and the
  defender is grounded (same approximation as above), score -10. The
  existing `IfTargetIsPartner Terminate` immediately above already guarantees
  the defender is a genuine opponent and not our own partner, matching the
  real block's "opposing side only" restriction in
  `Move_BlockedByPsychicTerrain` (`battle_lib.c`). Reading the move's
  priority required a new AI script command, since nothing previously
  exposed it: `LoadCurrentMovePriority` (`AICMD_LOADCURRENTMOVEPRIORITY` in
  `include/data/scripts/aicmd.h`, macro in `asm/macros/aicmd.inc`,
  `AICmd_LoadCurrentMovePriority` in `trainer_ai.c`), mirroring the existing
  `LoadCurrentMoveEffect`/`AICmd_LoadCurrentMoveEffect` exactly, but for
  `MOVE_DATA(...).priority` instead of `.effect`.
* **Distortion Terrain** — a new check at the top of `Basic_ScoreMoveEffect`
  (renamed to `Basic_ScoreMoveEffect_Dispatch` for the original per-effect
  chain, with `Basic_ScoreMoveEffect` now this new check followed by a fall-
  through into the dispatch): if Distortion Terrain is active, the current
  move's effect is looked up in one of two new tables. A move that
  *guarantees* the user raises its own stat(s) — the same set of moves this
  hack already gave 1-3 PP for their strength — scores a flat -8 and skips
  the normal per-effect dispatch entirely (the usual "don't bother, you're
  already maxed" checks below don't matter anymore, since the move is bad
  regardless of current stat stage). A damaging move with a mere *chance* to
  raise the user's stat on a hit (Metal Claw, Steel Wing, Ancient Power,
  Charge Beam) instead scores a much lighter -1, and terminates the same way
  as the guaranteed case above (none of the four `RAISE_*_HIT`/`LOWER_*_HIT`
  effects have any other entry in `Basic_ScoreMoveEffect_Dispatch`, so
  terminating here instead of falling through costs nothing).

### Distortion Terrain also rewards/penalizes stat-LOWERING effects, and gates chance-based effects behind Sheer Force
Extends the Distortion Terrain work above (`Basic_CheckDistortionStatMove`,
`src/battle/trainer_ai/script.s`) with the mirror-image cases it was missing:

* A move whose own drawback *guarantees* lowering the user's stat(s) —
  Superpower, Close Combat, Overheat/Draco Meteor/Psycho Boost, Hammer Arm —
  has that drawback become a genuine benefit under Distortion Terrain. New
  table `Basic_DistortionGuaranteedSelfLowerEffects` + handler
  `Basic_DistortionSelfLowerScorePlus8` score **+8** and then continue into
  the normal dispatch afterward (unlike every other case here, these moves
  are primarily valued for their damage, and that scoring still needs to run).
* A move that *guarantees* lowering the **target's** stat(s) — Growl, Leer,
  Tail Whip, String Shot, Sand Attack, Screech, Tickle, Captivate, and the
  rest — would actually raise them instead, directly helping the opponent.
  New table `Basic_DistortionGuaranteedOpponentLowerEffects` scores a matching
  **-8** deterrent and terminates.
* A damaging move with a mere *chance* to lower the target's stat on a hit
  (Rock Smash, Crunch, Psychic, Shadow Ball, Acid, Iron Tail, ...) would have
  that chance help the target instead. New table
  `Basic_DistortionChanceOpponentLowerEffects` scores a lighter **-1** and
  terminates, same as the existing chance-based self-boost case.
* **Sheer Force gate**: Sheer Force suppresses *any* move's secondary
  chance-based effect entirely (its `effect_chance` is 0 to begin with for a
  Sheer Force attacker — verified against the move data, e.g. Metal Claw's
  `chance: 10` vs. Superpower's `chance: 0`), so there's nothing left for
  Distortion Terrain to invert for either chance-based table. A new check —
  `LoadBattlerAbility AI_BATTLER_ATTACKER; IfLoadedEqualTo ABILITY_SHEER_FORCE,
  Basic_ScoreMoveEffect_Dispatch` — skips straight past both chance-based
  tables for a Sheer Force attacker. The two *guaranteed* tables are
  deliberately **not** gated by this, since a guaranteed effect isn't the kind
  of secondary chance Sheer Force touches.

### The AI now knows Sticky Web hasn't been evaluated as a hazard move
`src/battle/trainer_ai/script.s` — Sticky Web (added earlier as a new move,
see the Move changes section) had zero AI evaluation at all, unlike Stealth
Rock/Spikes/Toxic Spikes which each have a dedicated check. Added
`Basic_CheckStickyWebMove`, wired into `Basic_ScoreMoveEffect_Dispatch` via
`IfCurrentMoveEffectEqualTo BATTLE_EFFECT_STICKY_WEB, Basic_CheckStickyWebMove`,
mirroring `Basic_CheckStealthRock` exactly: -10 if the target's side already
has Sticky Web set, or -10 if the target is down to their last Pokemon.

### The AI's "does this move kill?" check now accounts for Sturdy and Focus Sash
`src/battle/trainer_ai/trainer_ai.c` — `AICmd_IfCurrentMoveKills` and
`AICmd_IfCurrentMoveDoesNotKill` (which back the `IfCurrentMoveKills`/
`IfCurrentMoveDoesNotKill` script commands used throughout `script.s`,
including kill-bonus scoring in `EvalAttack_ApplyKillBonuses` and
`TagStrategy_ScoreMove`) previously only compared the target's current HP to
the predicted damage, with no awareness that a full-HP target with Sturdy or a
held Focus Sash survives an otherwise-lethal hit at 1 HP instead of fainting
(the modernized Sturdy behavior from this hack's ability changes; Focus Sash's
`HOLD_EFFECT_ENDURE` behaves identically at full HP —
`BtlCmd_CheckHoldOnWith1HP` in `battle_script.c`). This meant the AI could
mis-predict a "kill" against either, missing that the target actually survives
to attack back or switch out. Fixed with a new read-only helper,
`TrainerAI_DefenderSurvivesLethalHitAtOneHP`, checked before both commands
treat a hit as lethal: true only when the defender is at full HP *and* either
holds a Focus Sash, or has Sturdy and the attacker's ability isn't Mold
Breaker. Deliberately implemented by hand rather than by calling the existing
`Battler_IgnorableAbility` helper, since that function has a mutating side
effect (marking Mold Breaker as "activated" for the turn) meant to fire once
during real move execution — calling it from AI evaluation, which
speculatively scores many candidate moves per turn, would trip that side
effect for moves that are never even chosen. Does not model Focus Band
(`HOLD_EFFECT_MAYBE_ENDURE`), since that's a random per-hit chance rather than
a guaranteed save the AI can rely on when predicting whether a move finishes
its target.

### The AI now recognizes when a Substitute blocks its move entirely
`src/battle/trainer_ai/script.s`, `Basic_ScoreMoveEffect` — a Substitute
intercepts almost every status effect and stat-lowering hit aimed at its
holder, but the AI never checked for this at all and would score these moves
completely normally against a target it can plainly see is behind one. This
was double-checked against the actual battle scripts before implementing,
not assumed: `subscript_paralyze.s`, `subscript_poison.s`,
`subscript_confuse.s`, `subscript_leech_seed_start.s`, `subscript_yawn.s`,
`subscript_mean_look.s`, `subscript_embargo_start.s`,
`subscript_heal_block_start.s`, `subscript_suppress_target_ability.s`,
`subscript_nightmare_start.s`, `subscript_fall_asleep.s`, and
`BtlCmd_ChangeStatStage`'s own `jumpBlockedBySubstitute` branch
(`battle_script.c`) all confirm the target's Substitute gates their effect.

Added a new check at the very top of `Basic_ScoreMoveEffect`, checked before
the existing Distortion Terrain check (a move a Substitute blocks outright
never reaches the point where Distortion Terrain's stat-inversion would even
matter): if the defender has an active Substitute, the current move's effect
is looked up in one of two new tables.

* **`Basic_SubstituteBlockedStatusEffects`** — pure status/stat-lowering
  moves whose entire effect is blocked (Thunder Wave, Toxic, Will-O-Wisp,
  Confuse Ray, Leech Seed, Nightmare, Mean Look, Embargo, Heal Block, Gastro
  Acid, Growl/Leer/Screech/Captivate and the rest of the guaranteed
  opponent-stat-lowering moves) score a flat **-10**, matching the existing
  "this will just fail" penalty used elsewhere, and terminate.
* **`Basic_SubstituteBlockedChanceEffects`** — a damaging move with only a
  secondary *chance* to lower the target's stat on hit (Rock Smash, Crunch,
  Psychic, Shadow Ball, Acid, Iron Tail, ...) still deals its normal damage
  to the Substitute; only the bonus effect is lost. These get a much lighter
  **-1** nudge instead, reusing the same effect list as
  `Basic_DistortionChanceOpponentLowerEffects` since both describe the exact
  same set of moves.

**Deliberately excludes** Disable, Taunt, Torment, Encore, and Attract.
Verifying this fix meant reading each relevant move's actual implementation
first, which surfaced five separate **pre-existing vanilla decomp bugs**
(not phmode-introduced, and not fixed here — flagged for a separate,
explicit decision): `BtlCmd_TryDisable`, `BtlCmd_TryEncore`, and
`BtlCmd_TryAttract` (`battle_script.c`), plus `subscript_taunt_start.s` and
`subscript_torment_start.s`, never check the target's Substitute at all, so
those five moves currently still work against one in this build. Scoring
them as blocked in the AI would have made trainers wrongly avoid a move that
actually functions fine today.

### Fixed Disable, Taunt, Torment, Encore, and Attract not checking for a Substitute
Confirmed vanilla decomp bugs (not phmode-introduced), found while double-checking
the AI Substitute fix above before deciding what to include in it:

* `BtlCmd_TryDisable`, `BtlCmd_TryEncore`, and `BtlCmd_TryAttract`
  (`src/battle/battle_script.c`) never checked `VOLATILE_CONDITION_SUBSTITUTE`
  on the target at all, so all three worked straight through one.
* `subscript_taunt_start.s` and `subscript_torment_start.s`
  (`res/battle/scripts/subscripts/`) had the same gap — neither called the
  existing `CheckSubstitute` battle-script command that every comparable
  status effect (Mean Look, Nightmare, Heal Block, Embargo, ...) already
  uses.

All five now correctly fail (`MOVE_STATUS_FAILED`, "But it failed!") against
a target behind a Substitute, matching every other status effect in the
game. `Basic_SubstituteBlockedStatusEffects` (`script.s`, added just above)
now includes `BATTLE_EFFECT_DISABLE`, `BATTLE_EFFECT_TAUNT`,
`BATTLE_EFFECT_TORMENT`, `BATTLE_EFFECT_ENCORE`, and
`BATTLE_EFFECT_INFATUATE`, so the AI now avoids all five the same way it
avoids Thunder Wave or Growl against a Substitute. (Taunt previously had no
AI scoring at all, for any reason — this is its first.)

### Fixed the documented Post-KO Switch-In AI scoring overflow
`src/battle/battle_lib.c`, `BattleAI_PostKOSwitchIn` — `docs/bugs_and_glitches.md`
already documented this exact bug and its one-line fix, just not yet applied
here: `score`/`maxScore` (Stage 1's type-matchup score, which can reach up to
320 for a quad-effective dual-type attacker) were declared as `u8`, silently
wrapping a quad-effective matchup down to 64. Changed both to `u32`, so the
AI's switch-in selection after a KO (or any switch that defers to this same
routine) now scores quad-effective matchups correctly instead of sometimes
undervaluing them relative to a merely-double-effective one.

### AI switch decisions now account for entry hazards on their own side
Two additions, both gated on the AI's own side already having Stealth Rock,
Spikes, Toxic Spikes, or Sticky Web up (`sideConditionsMask` checked against
all four):

* **`AI_ShouldSwitchForHazards`** (new, `src/battle/trainer_ai/trainer_ai.c`)
  — a small (1-in-6) chance to voluntarily switch out to a benched Pokemon
  that knows Defog or Rapid Spin. Checked in `TrainerAI_ShouldSwitch` right
  after the existing "don't switch, we already have the advantage" gates
  (`AI_HasSuperEffectiveMove`/`AI_IsHeavilyStatBoosted`), so it's a mild
  nudge that never overrides a clearly winning matchup, and directly picks
  the hazard-remover's party slot rather than deferring to generic
  matchup-based switch-in logic.
* **`BattleAI_PostKOSwitchIn` Stage 0** (new, `src/battle/battle_lib.c`) —
  before the existing type-matchup (Stage 1) and damage-score (Stage 2)
  logic, a 2-in-3 chance to prefer a benched Defog/Rapid Spin user over
  whatever those stages would otherwise pick. This is the routine used for
  *every* switch that doesn't already have a specific target in mind
  (fainting, Perish Song, the Natural Cure branches, and the new hazard
  check above when it defers), so this is the more broadly-applicable half
  of the two changes.

Both share a new `Pokemon_KnowsHazardRemovalMove` helper
(`src/battle/battle_lib.c`, declared in `include/battle/battle_lib.h`) that
checks a Pokemon's moveset for `MOVE_DEFOG`/`MOVE_RAPID_SPIN`. Previously,
neither "should I switch" nor "what should I switch into" gave any weight at
all to hazards already sitting on the AI's own field.

### Sucker Punch now weighs whether the target is likely to attack again
`src/battle/trainer_ai/script.s`, `Expert_SuckerPunch` — Sucker Punch only
works if the target uses a damaging move this turn, something the AI has no
way to know for certain in advance (it never sees the player's pending
choice for the turn — confirmed while investigating this). It now uses a
fair, already-known signal instead: `LoadBattlerPreviousMove
AI_BATTLER_DEFENDER` + `LoadPowerOfLoadedMove` (the same pattern
`Expert_Counter` already uses for its own "did they attack" check) checks
whether the target's *last* used move dealt damage — i.e. wasn't a status
move, and they weren't just switched in with no move history yet. If so,
score **+2**, independent of and in addition to the move's existing 75%
chance of +1. A target with no such signal (their last move was a status
move, or this is the first turn) just gets the existing baseline chance,
unchanged. The resisted/immune check above this still short-circuits to a
flat -1 regardless, since Sucker Punch is a poor choice either way if it
won't do meaningful damage.

### Fixed several documented vanilla decomp bugs (from docs/bugs_and_glitches.md)
Every "easy win" from the project's own known-bugs list was applied, in
addition to the Post-KO Switch-In overflow and Disable/Taunt/Torment/Encore/
Attract Substitute bugs already fixed above:

* **Acid Rain** — `res/battle/scripts/subscripts/subscript_pursuit.s`. A
  parameter swap in Pursuit's KO-while-switching-out handling (`SUB_TO_ZERO`
  against the field-conditions bitmask instead of setting the fainted-mon
  variable) could flip on every weather type simultaneously as if all of
  them were permanently active. Fixed to set `BTLVAR_FAINTED_MON` as
  intended.
* **Fire Fang Always Bypasses Wonder Guard** — `src/battle/battle_lib.c`,
  `MoveIsOnDamagingTurn`. An off-by-one copy/paste (`BATTLE_EFFECT_
  FLINCH_BURN_HIT`, Fire Fang's effect, instead of `BATTLE_EFFECT_
  SHADOW_FORCE`) let Fire Fang execute against Wonder Guard regardless of
  whether it would actually deal damage. Fixed to check the correct effect.
* **Using a Non-Rage Move After Rage Clears Every Volatile Status Except
  Rage** — `src/battle/battle_controller_player.c`. A backwards bitmask
  (`&= VOLATILE_CONDITION_RAGE` instead of `&= ~VOLATILE_CONDITION_RAGE`)
  meant switching off of Rage wiped every *other* volatile condition
  (Confusion, Substitute, Leech Seed, etc.) while leaving Rage itself
  intact — the exact opposite of the intended effect. Fixed to negate the
  mask.
* **Trainers Do Not Use the Correct Stats of Pokémon Forms** —
  `src/trainer_data.c`. Trainer Pokémon with a form that changes base stats
  (Wormadam, Rotom, etc.) were built using the base form's stats regardless
  of which form was actually assigned. Added `Pokemon_CalcStats(mon)` after
  each of the four `Pokemon_SetValue(mon, MON_DATA_FORM, &form)` call sites.
* **Defog HM Uses Water Palette** — `res/items/data/hm05.json`. The Bag icon
  for HM05 (Defog, a Flying-type move) used the Water-type TM/HM palette.
  Fixed to the Flying-type palette.
* **Fishing Encounters Ignore Sticky Hold and Suction Cups** —
  `src/overlay006/wild_encounters.c`. A statement with no effect
  (`newEncRate * 2;` instead of `newEncRate *= 2;`) meant these abilities
  never actually doubled the fishing encounter rate as intended.
* **Surfing and Fishing Encounters Ignore Magnet Pull** — same file. Static's
  encounter-slot check ran unconditionally after Magnet Pull's for Surf and
  fishing encounters (missing the `if (!forcedSlot)` guard land encounters
  already have), so Static always overwrote whatever slot Magnet Pull had
  already forced. Fixed to only check Static when Magnet Pull didn't already
  find a match, matching the land-encounter logic.
* **Giratina Hover Range** (title screen) —
  `src/applications/title_screen.c`. The hover angle was needlessly rescaled
  before being passed to `CalcSineDegrees_Wraparound`, which already expects
  plain degrees, shrinking the intended range of motion. Fixed to pass the
  angle directly.
* **Battle animation sprite drift** — four small overlapping-delay/missing-
  reset bugs that permanently nudged a sprite's position over the course of
  an animation: Facade (`res/moves/facade/anim.s`, delays increased from 8
  to 10 frames, matching sound-effect timing), DynamicPunch
  (`res/moves/dynamic_punch/anim.s`, 3 → 4 frames), Helping Hand
  (`res/moves/helping_hand/anim.s`, moved a `Delay 1` inside its loop),
  Strength, and Spit Up (`src/battle_anim/script_funcs_0.c` /
  `script_funcs_3.c`, both now reset the sprite's X position via
  `BattleAnimUtil_GetBattlerDefaultPos` once their shake-and-scale sequence
  ends, rather than leaving it wherever the last shake increment left it).

Not fixed: the 3D-rendering `G3DPipeline_InitEx` VRAM-manager bug, since the
project's own documentation notes it never actually triggers in the shipped
game (the affected parameter is always set to the other value) — there is no
player-visible behavior to correct.

### Paralysis now uses modern (Gen 9-style) mechanics
Three related changes, all in the base paralysis mechanic rather than any
one move:

* **Electric-types are now immune to paralysis entirely**, regardless of
  what would have caused it (Thunder Wave, Stun Spore, Glare, or a damaging
  move's paralysis chance). Added to `subscript_paralyze.s`
  (`res/battle/scripts/subscripts/`): a new `BATTLEMON_TYPE_1`/`_TYPE_2`
  check for `TYPE_ELECTRIC`, placed the same way `subscript_burn.s` already
  checks for Fire-type immunity to burns. This reuses a message block
  ("It doesn't affect {0}...") that existed in the vanilla file but was
  unreachable dead code, since nothing ever needed to jump to it before now.
* **Paralysis now only halves Speed** instead of cutting it to a quarter.
  `src/battle/battle_lib.c` — both `battler1Speed /= 4` and
  `battler2Speed /= 4` in the shared speed-comparison routine changed to
  `/= 2`. Since the trainer AI's own speed comparisons run through this
  same function (confirmed — it has no separate copy of this logic), this
  applies automatically and correctly to every AI speed-order decision with
  no separate AI-side change needed.
* **Full paralysis (failing to act) now has a 12.5% chance instead of 25%.**
  `src/battle/battle_controller_player.c`, `CHECK_STATUS_STATE_PARALYSIS` —
  changed `BattleSystem_RandNext(battleSys) % 4 == 0` to `% 8 == 0`.

**AI updated accordingly:** `Basic_CheckCannotParalyze`
(`src/battle/trainer_ai/script.s`) gained a new Electric-type check (mirrors
`Basic_CheckCannotBurn`'s existing Fire-type check exactly), so trainers now
correctly recognize an Electric-type target can't be paralyzed at all and
avoid wasting a turn on Thunder Wave/Stun Spore/Glare against one — the same
-10 "this will just fail" penalty used for every other guaranteed-failure
case. The qualitative scoring in `Expert_StatusParalyze` (bonus for being
slower, penalty for wasting a turn while hurt) is unchanged, since paralysis
is still valuable to a slower attacker under the new 50%/12.5% numbers, just
less dramatically so — not something that needed rebalancing to remain
correct.

### Ghost-types are now immune to every form of trapping (modern mechanic)
Mean Look/Block/Spider Web, binding moves (Wrap, Fire Spin, Whirlpool, Sand
Tomb, Clamp), Shadow Tag, Arena Trap, and Magnet Pull can no longer prevent a
Ghost-type from switching or fleeing. Ingrain is unaffected, since that's the
user choosing to root itself rather than being trapped by an opponent.

* `Battler_IsTrapped` (`src/battle/battle_lib.c`) — the switch-menu
  availability gate — now returns early for a Ghost-type battler before any
  of the ability/volatile-status checks, checking Ingrain first since that
  still applies.
* `Battler_IsTrappedMsg` (same file) — the Run-command/Teleport gate — split
  its combined Ingrain+trapped check apart the same way, so Ingrain still
  blocks escape but a Ghost-type skips everything else.
* `BattleControllerPlayer_FleeCommand` (`src/battle/battle_controller_player.c`)
  — a wild Ghost-type can now still flee despite Mean Look/Bind.
* `subscript_mean_look.s` and `subscript_bind_start.s`
  (`res/battle/scripts/subscripts/`) — the moves themselves now fail
  outright ("It doesn't affect...") against a Ghost-type target, rather than
  applying a volatile status that would have had no effect anyway. A binding
  move still deals its initial hit of damage; only the ongoing trap/damage
  is skipped.
* `TrainerAI_ShouldSwitch`'s illegal-switch check (`src/battle/trainer_ai/trainer_ai.c`)
  updated to match `Battler_IsTrapped`.
* **AI updated:** `Basic_CheckMeanLook` now scores -10 against a Ghost-type
  target (script.s), and `Expert_BindingMove` skips its "lock them in for
  the kill" utility bonus entirely against one, since trapping a Ghost-type
  does nothing.

### Grass-types are now immune to powder moves (modern mechanic)
Cotton Spore, Poison Powder, Sleep Powder, Spore, and Stun Spore now fail
outright against a Grass-type target.

Checked in each move's actual application path (mirroring the Fire/burn and
Electric/paralysis type-immunity pattern already established): `subscript_fall_asleep.s`
(Sleep Powder/Spore, both entry points), `subscript_poison.s` (Poison
Powder), `subscript_paralyze.s` (Stun Spore), and `BtlCmd_ChangeStatStage`
in `battle_script.c` (Cotton Spore) — each checks the *current move's own
identity* against the five powder moves by ID (`Move_IsPowderMove` in
`battle_lib.c`/`battle_lib.h`, and direct `CompareVarToValue ..., BTLVAR_CURRENT_MOVE,
MOVE_X, ...`/`IfMoveEqualTo MOVE_X, ...` comparisons in the subscripts and
AI script respectively), not just its effect ID, so a non-powder move
sharing the same effect (Yawn vs. Sleep Powder, String Shot vs. Cotton
Spore) is correctly unaffected.

**AI updated:** `Basic_CheckCannotSleep`, `Basic_CheckCannotPoison`,
`Basic_CheckCannotParalyze`, and `Basic_CheckLowStatStage_Speed` (`script.s`)
each gained a check: if the current move is one of the five powder moves
(`IfMoveEqualTo`) and the target is Grass-type, score -10 — the AI now
avoids all five powder moves against a Grass-type the same way it avoids
Thunder Wave against an Electric-type.

**Bug fix (game-breaking):** the first implementation added a 9th
`MOVE_FLAG_*` bit and widened `MoveTable.flags` from `u8` to `u16` to fit
it (`include/move_table.h`), reading the wider value in
`tools/dataproc/src/moveproc.c`. `MoveTable` is packed byte-for-byte by that
host-side tool (`nitroarc_ppack(&archives[0].packer, &data, sizeof(data), NULL)`
in `moveproc.c`) and read back as a raw struct at runtime by the ARM9 game —
so widening a field changes the struct's packed size/layout, and the host
compiler (building `dataproc`) and the target compiler (CodeWarrior,
building the ARM9 binary) don't necessarily agree on the resulting padding.
That mismatch corrupted every move's data at and past the widened field,
which made the trainer AI hang instantly and irrecoverably (an infinite
loop scanning corrupted data) on literally the first wild or trainer battle
of the game, with any move — an extensive multi-emulator debugging session
was needed just to isolate it via `git bisect`, since the corruption gave
no useful error, only a silent freeze. Fixed by reverting `MoveTable.flags`
to `u8` and checking the five powder moves by move ID instead of a data
flag, which needs no `MoveTable` changes at all. The `LoadCurrentMoveFlags`
AI command, `MOVE_FLAG_POWDER`, and the widened struct field were removed
entirely rather than left as unused dead code.

### Rapid Spin now only clears hazards from the user's own side
`BtlCmd_BlowAwayHazards` (`src/battle/battle_script.c`), shared by Rapid Spin
and Defog, previously cleared Spikes/Toxic Spikes/Stealth Rock/Sticky
Web/Tailwind from *both* sides of the field for either move — a deliberate
departure from modern games noted when hazard-clearing was first added.
Reverted for Rapid Spin specifically: it now only clears its user's own
side, matching Gen 6+. Defog is unchanged and still clears both sides.
Gravity-clearing (shared by both moves) is unaffected either way, since
Gravity is a whole-field condition with no separate "side" to limit it to.
No AI change was needed — nothing in the trainer AI has dedicated Rapid Spin
scoring; it's evaluated like any other damaging move.

### Gen 4 vs. Gen 9/Champions mechanics audit — decisions and changes
Follow-up to a research pass identifying places where this game's underlying
(Gen 4) mechanics differ from modern (Gen 9/Champions) ones. Each item below
was explicitly decided rather than defaulted one way or the other:

* **Steel no longer resists Ghost or Dark** (changed). `src/battle/battle_lib.c`,
  `sTypeMatchupMultipliers` — removed the `{ TYPE_GHOST, TYPE_STEEL,
  TYPE_MULTI_NOT_VERY_EFF }` and `{ TYPE_DARK, TYPE_STEEL,
  TYPE_MULTI_NOT_VERY_EFF }` rows (the table only lists non-neutral
  matchups, so removing a row makes it fall through to neutral). Matches
  the Gen 6+ type chart. No AI change needed — the trainer AI's damage
  prediction reads this exact same table via `BattleSystem_ApplyTypeChart`,
  so the corrected matchup is automatically reflected in its scoring.
* **Confusion self-hit chance is now 33% (1-in-3)** (changed), not the
  vanilla 50% coin flip. `src/battle/battle_controller_player.c`,
  `CHECK_STATUS_STATE_CONFUSION` — changed `BattleSystem_RandNext(battleSys)
  & 1` to `% 3 != 0`. Matches Gen 7+. No AI change needed - nothing in the
  AI's confusion scoring (`Expert_StatusConfuse`) models the self-hit odds
  directly, only the target's HP.
* **Sleep now lasts a flat 1-3 turns** (changed), not the vanilla 2-5 turns.
  `res/battle/scripts/subscripts/subscript_fall_asleep.s` — changed
  `Random 3, 2` to `Random 2, 1`. Matches Gen 5+. No AI change needed -
  nothing in the AI's sleep-move scoring assumes a specific duration.
* **Weather-setting abilities (Drought/Drizzle/Sand Stream/Snow Warning)
  remain permanent** (deliberately not changed), unlike Gen 6+'s 5-turn
  duration (9 with the matching rock item). Kept as the original Gen
  3-5-style "lasts until something else changes the weather" behavior.
* **Critical hit rate remains the Gen 4 table** (deliberately not changed):
  1/16 base, then 1/8, 1/4, 1/3, 1/2 per stage (`sCriticalStageRates` in
  `src/battle/battle_lib.c`), rather than Gen 6+'s 1/24, 1/8, 1/2,
  guaranteed, guaranteed.

**Verified, no change needed:** a sleeping Pokemon's remaining turn count is
already correctly preserved across switching out and back in. The per-turn
sleep-counter decrement (`CHECK_STATUS_STATE_SLEEP` in
`battle_controller_player.c`) runs inside the same status-check routine that
unconditionally calls `BattleMon_CopyToParty` before returning, which syncs
the just-decremented status back to the persistent party Pokemon data
(`MON_DATA_STATUS`) every turn — not just on switch-out. Switching back in
later re-reads that same persisted value (`BattleSystem_InitBattleMon`), so
the remaining sleep turns are never lost or re-rolled.

**Fixed:** the "Inflict status" PC debug menu's Sleep option
(`ScrCmd_InflictPartyMonStatus` in `src/scrcmd_party.c`) always set 3 turns
remaining (`MON_CONDITION_SLEEP_0 | MON_CONDITION_SLEEP_1`); changed to
always set exactly 1 turn remaining (`MON_CONDITION_SLEEP_0` alone), per
request.

### Creation-trio signature abilities
Dialga, Palkia and Giratina lose Pressure/Levitate and get a new ability each.
Three abilities were added to `generated/abilities.txt` (124-126) with entries in
`res/text/ability_names.json`, `ability_names_uppercase.json` and
`ability_descriptions.json`, and assigned in `res/pokemon/{dialga,palkia,giratina}/data.json`
plus `res/pokemon/giratina/forms/origin/data.json`.

* **Time Warp** (Dialga) — while any Time Warp user is on the field, every
  two-turn move resolves in a single turn: charging moves (Sky Attack, Solar
  Beam, Razor Wind, Skull Bash, Meteor Beam, Freeze Shot, Ice Burn) and the
  semi-invulnerable moves (Fly, Dig, Dive, Bounce, Phantom Force) fire the turn
  they are chosen, and recharge moves (Hyper Beam, Giga Impact, Roar of Time, …)
  skip the recharge turn. Implemented with a new battle script command
  `GoToSubscriptIfAbilityOnField` (`asm/macros/btlcmd.inc`,
  `include/data/scripts/btlcmd.h`, `src/battle/battle_script.c`) added to the
  nine charge effect scripts (`res/battle/scripts/effects/effect_script_0039.s`
  and friends) beside their Power Herb check, plus `effect_script_0080.s` for the
  recharge. `subscript_item_skip_charge_turn` / `subscript_power_herb_skull_bash`
  now skip the "fully charged due to its item!" line and the item removal when
  the skip came from Time Warp rather than a held item. On switch-in it also
  announces itself — "[Pokémon] is warping time around it!" — via
  `subscript_time_warp_announce`.
* **Space Warp** (Palkia) — sets battle-long gravity on switch-in. A new
  `FIELD_CONDITION_GRAVITY_PERM` bit (`include/constants/battle/condition.h`) is
  folded into `FIELD_CONDITION_GRAVITY`, so every "is gravity active" check
  honours it — the modern Gravity effect is unchanged (all-move accuracy ×5/3;
  Flying-types and Levitate grounded for Ground moves, Spikes/Toxic Spikes/Sticky
  Web and Arena Trap; Fly/Bounce/Jump Kick/Hi Jump Kick/Splash/Magnet Rise fail).
  The per-turn gravity countdown in `battle_controller_player.c` is guarded to
  leave the permanent flag alone. `subscript_space_warp` announces the ability,
  sets the flag, then calls `subscript_gravity_start` for the same one-time
  knock-down of airborne Pokémon and Magnet Rise; `subscript_gravity_start` skips
  its own "Gravity intensified!" line when the permanent flag is already set.
* **Distortion Surge** (Giratina, both forms) — sets distortion terrain on
  switch-in via `subscript_distortion_surge`; the terrain bit already persists
  for the rest of the battle.

New switch-in ability cases live in `battle_lib.c`'s
`SWITCH_IN_CHECK_STATE_WEATHER_ABILITIES`; the three new subscripts
(`subscript_{distortion_surge,space_warp,time_warp_announce}`) are registered in
`res/battle/scripts/subscripts/{sub_seq.order,meson.build}`. The new announce
strings in `res/text/battle_strings.json` each come in the `_Ally`/`_Wild`/`_Foe`
trio that `BattleMessage_CheckSide` expects for `TAG_NICKNAME`/`TAG_NICKNAME_ABILITY`.

### Normalize also boosts Normal-move power
`src/battle/battle_lib.c` — in `BattleSystem_CalcMoveDamage`, an attacker with
Normalize now gets a ×1.2 power boost on the moves it converts, matching the
modern ability (previously it only changed the move's type).

### Trainer AI bug fixes (vanilla decomp bugs, not phmode-introduced)
Three pre-existing bugs in the trainer AI — already flagged with `// BUG:`
comments in the decomp, or independently confirmed by re-reading the logic —
were actually fixed rather than left as documented-but-live bugs:

* **Dry Skin was never recognized as a Water immunity.**
  `src/battle/trainer_ai/script.s`, `Basic_CheckForImmunity` — the ability chain
  (Volt Absorb / Motor Drive / Water Absorb / Flash Fire / Wonder Guard /
  Levitate) had a final entry that checked `ABILITY_LEVITATE` a second time
  instead of `ABILITY_DRY_SKIN`. Since the line above it already branches away
  whenever the ability *is* Levitate, the duplicate check could never be true —
  dead code — so the AI never treated Dry Skin as a Water-move immunity/heal
  when scoring any move against a Dry Skin holder. Now checks `ABILITY_DRY_SKIN`
  as originally intended.
* **Metal Burst's speed check looked for a Shiny Stone instead of a Lagging
  Tail.** `src/battle/trainer_ai/script.s`, `Basic_CheckMetalBurst` — deciding
  whether either battler reliably moves last (relevant to a move that wants to
  go after being hit) checked `IfHeldItemEqualTo ..., ITEM_SHINY_STONE` for both
  the attacker and defender — an unrelated evolution item with no bearing on
  turn order. Replaced with `LoadHeldItemEffect` + `HOLD_EFFECT_PRIORITY_DOWN`
  (the hold effect shared by Lagging Tail and Full Incense, both of which force
  the holder to always move last), matching what the existing "BUG:" comments
  said this check should have been doing all along.
* **The AI could dump its entire remaining item pool in one decision.**
  `src/battle/trainer_ai/trainer_ai.c`, `TrainerAI_ShouldUseItem` — this loops
  over every item slot in the trainer's tracked inventory and, on finding one
  that should be used this turn, sets a shared `result` flag to `TRUE` and
  records that slot. The loop never stopped there, though, and `result` was
  never reset back to `FALSE` for the next iteration — so every *subsequent*
  slot's `if (result == TRUE) { ...; trainerItems[i] = 0; }` fired too,
  regardless of whether that later item's own condition was ever met, silently
  zeroing every remaining item out of the trainer's list as if each had been
  used, while only the last one processed actually got recorded as `usedItem`.
  This is the "AI seems to use up all its items at once" bug — a single
  qualifying item earlier in the list could wipe out every item after it in the
  same turn's check. Fixed by `break`ing out of the loop the moment one item is
  chosen, since only one item is ever actually used per turn anyway.

### Trainer AI: a small penalty for moves that *might* be walled by an unrevealed ability
`src/battle/trainer_ai/script.s`, `Basic_CheckForImmunity` — previously, once the
existing confirmed-or-guessed ability check (`LoadBattlerAbility`, which either
already knows the defender's ability or coin-flips between its two possible
ones) found no immunity, the move was scored with no further caution at all,
even if the *other*, un-guessed possible ability would have granted one.

Now, when none of that finds a confirmed/guessed immunity, a new
`Basic_CheckPossibleImmunityAbility` step runs: for the move's type, it checks
each relevant immunity ability (Volt Absorb/Motor Drive for Electric,
Water Absorb/Dry Skin for Water, Flash Fire for Fire, Levitate for Ground) via
`CheckBattlerAbility`. That command — unlike `LoadBattlerAbility` — doesn't
guess: if the ability being asked about is one of the defender's two possible
abilities but hasn't been confirmed in battle, it deterministically reports
`AI_UNKNOWN` rather than picking one. `AI_UNKNOWN` already existed as a result
value but was never actually checked anywhere in the AI scripts before this.
When it comes back, the move gets a small `-2` (not the full `-10`/`-12` a
confirmed immunity gets) — enough to make the AI lean away from a move that
*might* be walled, without it ever assuming for certain that it will be.
Wonder Guard is deliberately excluded: its only vanilla holder (Shedinja) has
just one possible ability, so there's never a genuine "could be either" case
for it to matter.

Modeled in `ai_tests/` as `AI_PossibleImmunityPenalty` (see
`ai_tests/scenarios/ability_interactions.c`), alongside the existing
`AI_IsImmuneToMove` model of the confirmed/guessed check this complements.

### Trainer AI bug fix: Charge was never evaluated at all, letting it get used with no regard for whether it was actually useful
Reported: a level 10 Shinx (Lass Sarah) used Charge against a purely physical
attacker despite not even knowing an Electric move to combo its power boost
with. Root cause: `BATTLE_EFFECT_SP_DEF_UP_DOUBLE_ELECTRIC_POWER` (Charge's
effect - Sp. Def +1 and double the user's next Electric move's power) was
missing from both AI dispatch tables in `src/battle/trainer_ai/script.s`
(`Basic_ScoreMoveEffect_Dispatch` and `Expert_Main`), unlike every other
stat-boosting effect (`SP_DEF_UP`, `SP_DEF_UP_2`, etc.), each of which routes
to a handler that at least checks whether the stat is already maxed. Missing
from the dispatch entirely, Charge fell through with no evaluation
whatsoever - not even that baseline check - and kept whatever default score
non-damaging moves get regardless of context.

Fixed by routing `BATTLE_EFFECT_SP_DEF_UP_DOUBLE_ELECTRIC_POWER` to the exact
same handlers plain `SP_DEF_UP` already uses: `Basic_CheckHighStatStage_SpDefense`
(discourages it once Sp. Def is already maxed) and, for `AI_FLAG_EXPERT`
trainers, `Expert_StatusSpDefenseUp` (which also discourages it based on the
target's last-used move class - e.g. a good chance of -2 if the target's last
move was Physical, exactly the reported scenario). This mirrors how
`BATTLE_EFFECT_DEF_UP_DOUBLE_ROLLOUT_POWER` (Defense Curl's effect) already
reuses the plain `DEF_UP` handler rather than a bespoke one, so this brings
Charge in line with the existing convention for "boost a defensive stat and
power up a specific follow-up move" effects instead of adding new logic.

---

## Abilities

Newly implemented abilities, added to `generated/abilities.txt` (127-142) with
`res/text/ability_names.json` / `ability_names_uppercase.json` /
`ability_descriptions.json` entries. The creation-trio abilities (Time Warp /
Space Warp / Distortion Surge, 124-126) are covered under "Battle changes".

The `ability_descriptions.json` lines for Defiant, Competitive and Refrigerate
were reworded to stay within the ~26-character-per-line width of the summary
screen's ability-description window (`SUMMARY_WINDOW_ABILITY_DESCRIPTION`); the
same pass shortened the Rapid Spin, Sticky Web, Power Whip and Wild Charge move
descriptions to fit the 23-character move-info window.

* **Sheer Force** (127) — `src/battle/battle_lib.c`. A move with a non-zero
  `effectChance` deals ×1.3 damage in `BattleSystem_CalcMoveDamage`, and its
  chance-based added effect is suppressed (`BattleSystem_TriggerSecondaryEffect`
  skips the roll).
* **Defiant** (128) / **Competitive** (129) — `src/battle/battle_script.c`.
  When an opposing battler lands a stat drop on this Pokémon, `BtlCmd_ChangeStatStage`
  flags it (`battleCtx->defiantReactType`); once the "stat fell!" line has shown,
  `subscript_update_stat_stage` runs the new `BtlCmd_CheckDefiantReaction`, which
  re-enters the stat-change subscript as a `SIDE_EFFECT_TYPE_ABILITY` **+2 Attack**
  (Defiant) or **+2 Sp. Atk** (Competitive) raise — so the boost gets its own
  animation and a "{mon}'s {ability} sharply raised its {stat}!" message (a new
  `BattleStrings_Text_PokemonsAbilitySharplyRaisedItsStat_*` trio; `+1` ability
  raises still use the plain "raised its {stat}!"). Only fires when
  the drop actually landed (not when Clear Body / Mist / a -6 stat blocks it) and
  only from the opposing side (so a Sticky Web / Intimidate from a foe counts).
  New btlcmd registered in `asm/macros/btlcmd.inc` /
  `include/data/scripts/btlcmd.h`.
* **Prankster** (130) — `src/battle/battle_lib.c`, `BattleSystem_CompareBattlerSpeed`.
  Grants +1 priority to `CLASS_STATUS` moves.
* **Contrary** (131) — `src/battle/battle_script.c`, `BtlCmd_ChangeStatStage`.
  Inverts every stat-stage change on the Pokémon that has it (runs before the
  Distortion-Terrain inversion, so the two cancel out).
* **Psychic Surge** (132) / **Electric Surge** (133) —
  `src/battle/battle_lib.c` `SWITCH_IN_CHECK_STATE_WEATHER_ABILITIES`, plus
  `subscript_psychic_surge` / `subscript_electric_surge` (registered in
  `res/battle/scripts/subscripts/{sub_seq.order,meson.build}`) and `_Ally`/`_Wild`/`_Foe`
  message trios in `res/text/battle_strings.json`. On switch-in they set the
  existing `FIELD_CONDITION_PSYCHIC_TERRAIN` / `FIELD_CONDITION_ELECTRIC_TERRAIN`
  (added with the battle terrains), so the terrain lasts the rest of the battle.
* **Shed Spines** (134) — same switch-in hook. On entry it adds one layer of
  Spikes to the opposing side (up to the normal 3), then breaks to
  `subscript_shed_spines` for the announce message (`_Ally`/`_Wild`/`_Foe` trio in
  `res/text/battle_strings.json`). The layer itself is added inline in C so it
  can target the switched-in mon's opposing side.
* **Toxic Boost** (135) — `src/battle/battle_lib.c` `BattleSystem_CalcMoveDamage`,
  next to Guts. While the user is poisoned (regular or bad), its physical moves
  get ×1.5 power.
* **Protean** (136) — `src/battle/battle_controller_player.c`. A new
  `BEFORE_MOVE_STATE_PROTEAN` step in `BattleControllerPlayer_BeforeMove` runs
  after the "does the move have a target?" check (so it does **not** activate when
  the move fails for lack of a target): if the attacker has Protean and is not
  already that move's pure type, it is retyped to the move's type and
  `subscript_protean` announces it (reusing the existing
  `BattleStrings_Text_PokemonsAbilityMadeItType_*` message trio, as Color Change
  does; new subscript registered in
  `res/battle/scripts/subscripts/{sub_seq.order,meson.build}`). Unlike the modern
  games there is no once-per-switch-in limit.
* **Refrigerate** (137) — `src/battle/battle_lib.c` helper `Move_AteAbilityType`,
  mirrored everywhere the existing Normalize override is applied
  (`BattleSystem_ApplyTypeChart`, `BattleSystem_CalcEffectiveness`, Color Change,
  `BattleSystem_CalcMoveDamage`, and the `CalcMoveType` helpers in
  `battle_controller_player.c` / `battle_script.c`). The user's Normal-type moves
  become Ice-type (for effectiveness, STAB and immunity checks) and gain ×1.2
  power. Hidden Power, Weather Ball, Natural Gift, Judgment and Struggle are
  exempt.
* **Flare Boost** (138) — `src/battle/battle_lib.c` `BattleSystem_CalcMoveDamage`,
  next to Toxic Boost. While the user is burned, its special moves deal ×1.5
  power.
* **Moxie** (139) — new `BtlCmd_TryMoxie` in
  `subscript_faint_check_destiny_bond`, so the boost is announced **after** the
  "{mon} fainted!" message (not on hit). When a Moxie holder's move KOs a
  Pokémon on the other side and the holder is still alive, its Attack rises one
  stage ("{mon}'s Moxie raised its Attack!").
* **Poison Touch** (140) — new `BattleSystem_TriggerAttackerAbilityOnHit`, run in
  its own after-move step (`ONE_HIT_TRIGGER_ATTACKER_ABILITY` /
  `MULTI_HIT_TRIGGER_ATTACKER_ABILITY`) so it stacks with the defender's on-hit
  ability instead of pre-empting it. The attacker's contact moves have a 30%
  chance to poison the target, reusing `subscript_poison` with the attacker as
  the ability holder. Not blocked by Shield Dust.
* **Justified** (141) — a proper `case ABILITY_JUSTIFIED` in
  `BattleSystem_TriggerAbilityOnHit`'s defender-ability switch. Being hit by a
  Dark-type damaging move raises the holder's Attack one stage (reuses
  `subscript_moxie` with the defender as the stat target).
* **Sharpness** (142) — `src/battle/battle_lib.c` `BattleSystem_CalcMoveDamage`,
  next to Sheer Force. Slicing / cutting moves deal ×1.5 power. The slicing set
  (`Move_IsSlicing`) is Cut, Slash, Night Slash, Psycho Cut, Leaf Blade,
  X-Scissor, Air Slash, Air Cutter, Fury Cutter, Cross Poison, Aerial Ace,
  Razor Leaf and Razor Wind.

### Sturdy — modern version
`src/battle/battle_controller_player.c` (main damage application) and
`src/battle/battle_script.c` `BtlCmd_CheckHoldOnWith1HP`. In addition to blocking
OHKO moves (already implemented), a Pokémon with Sturdy that is at full HP now
survives any otherwise-lethal hit at 1 HP. Mold Breaker ignores it
(`Battler_IgnorableAbility`). It sets a dedicated `MOVE_STATUS_STURDY_HELD_ON`
flag (not the Endure flag), so `subscript_move_followup_message` /
`subscript_future_sight_damage` print **"{mon} survived the hit with Sturdy!"**
(`BattleStrings_Text_PokemonSurvivedTheHitWithSturdy_*`) rather than the generic
"endured the hit!".

---

## Moves

### HM move retypes
`res/moves/<move>/data.json` — three HM moves get a type matching their theme
(everything else about them is unchanged):

* **Strength** → Fighting
* **Cut** → Grass
* **Rock Climb** → Rock

### X-Scissor — high critical-hit ratio
`res/moves/x_scissor/data.json` — effect changed from `BATTLE_EFFECT_HIT` to
`BATTLE_EFFECT_HIGH_CRITICAL`.

### Boomburst (new move, id 468)
`res/moves/boomburst/{data.json,anim.s,script.s}` + `generated/moves.txt`.
Normal-type special, 140 power / 100 accuracy / 10 PP, hits all adjacent Pokémon,
`BATTLE_EFFECT_HIT`. Added to `sSoundMoves` in `src/battle/battle_lib.c` so it
bypasses Substitute and is blocked by Soundproof. `moveproc` generates its
name/description text from `data.json`.

### Knock Off — modern version
`res/moves/knock_off/data.json` — base power 20 → 65, description updated.
`src/battle/battle_lib.c` `BattleSystem_CalcMoveDamage` — Knock Off deals ×1.5
damage when the target is holding an item that the move can actually remove. This
mirrors the full removal condition in `subscript_knock_off`: no boost when the
item is pinned by Sticky Hold, a Substitute, Multitype, the Griseous Orb, or an
already-triggered Quick Claw / Custap Berry.

### Thief, Switcheroo, and Trick consolidated into Knock Off
With Knock Off now the strong, modern item-removal move above, the other three
"mess with the foe's item" moves are removed as separately learnable and folded
into it, so every species that had one of them now learns Knock Off instead:

* **Level-up learnsets** — 17 entries across
  `res/pokemon/{kecleon,mightyena}` (Thief), `res/pokemon/{linoone,persian,hypno}`
  (Switcheroo), and `res/pokemon/{kadabra,alakazam,mime_jr,mr_mime,shuppet,banette,rotom}`
  + all 5 Rotom forms (Trick) — each `[level, "MOVE_X"]` entry changed in place
  to `[level, "MOVE_KNOCK_OFF"]` at the same level. Egg-move / tutor-compatibility
  list entries for these moves (kecleon, mime_jr, mr_mime, buneary, etc.) are
  untouched — this only affects level-up learnsets.
* **TM46** (`res/items/data/tm46.json`) — used to teach Thief, now teaches
  Knock Off; description updated to match. No other TM taught Knock Off before
  this, so there's no conflict — the 217 species with `TM46` in their `by_tm`
  list are now TM46-compatible with Knock Off instead of Thief.
* **Move tutor** (`res/pokemon/move_tutors.json`) — the Route 212 shard tutor
  already separately taught Knock Off (4 Red + 4 Blue Shards) alongside its
  Trick entry (4 Blue + 4 Yellow Shards); rather than point Trick's entry at a
  move that'd then be listed twice in the same tutor's menu, the Trick entry is
  deleted outright and the existing Knock Off entry (and its cost) is left as
  the sole way to tutor it there.

### Leech Life — modern power
`res/moves/leech_life/data.json` — base power 20 → 80 (in line with Gen 7+).

### Attack Order — spread hit
`res/moves/attack_order/data.json` — power 90 → 100, range `RANGE_SINGLE_TARGET`
→ `RANGE_ADJACENT_OPPONENTS`, effect `BATTLE_EFFECT_HIGH_CRITICAL` →
`BATTLE_EFFECT_HIT` (the high critical-hit ratio is removed).

### Defend Order — sharp boost
`res/moves/defend_order/data.json` — effect `BATTLE_EFFECT_DEF_SPD_UP` →
`BATTLE_EFFECT_DEF_SPD_UP_2`, a new effect (id 278) with its own
`effect_script_0277.s` and `subscript_user_def_and_spdef_up_2_stages`, raising
the user's Defense and Sp. Def by **two** stages each. Cosmic Power still uses the
one-stage effect.

### Heal Order — heals the ally too
`res/moves/heal_order/data.json` — effect `BATTLE_EFFECT_RESTORE_HALF_HP` →
`BATTLE_EFFECT_HEAL_ORDER` (id 279, `effect_script_0278.s`). New
`subscript_heal_order` heals the user for half its max HP as before, then a new
btlcmd `TryHealOrderAlly` (`BtlCmd_TryHealOrderAlly` in `src/battle/battle_script.c`,
macro in `asm/macros/btlcmd.inc`, entry in `include/data/scripts/btlcmd.h`) heals
the attacker's ally for half of *its* max HP in a double battle, skipping the
second heal when there is no hurt, living ally.

### Power Whip — perfect accuracy, recoil
`res/moves/power_whip/data.json` — accuracy 85 → 100, effect `BATTLE_EFFECT_HIT`
→ `BATTLE_EFFECT_RECOIL_QUARTER` (the user takes 25% of the damage dealt as
recoil).

### Rapid Spin — modern version
`res/moves/rapid_spin/data.json` — base power 20 → 50, description updated.
`subscript_rapid_spin` now raises the user's Speed one stage on hit (after the
hazard-clearing step), via `MOVE_SUBSCRIPT_PTR_SPEED_UP_1_STAGE` with
`SIDE_EFFECT_TYPE_INDIRECT` ("{mon}'s Speed rose!"). Still removes the user's
Bind and Leech Seed; entry-hazard / Tailwind / Gravity removal is handled by the
shared `BlowAwayHazards` command (see "Defog and Rapid Spin clear every hazard"
above).

### Sticky Web (new move, id 471)
`res/moves/sticky_web/{data.json,anim.s,script.s}` + `generated/moves.txt`
(`MAX_MOVES` is now 472), new `BATTLE_EFFECT_STICKY_WEB` (id 280) with
`effect_script_0279.s` and the new `TryStickyWeb` btlcmd
(`BtlCmd_TryStickyWeb`). Bug-type status, `RANGE_OPPONENT_SIDE`: lays
`SIDE_CONDITION_STICKY_WEB` on the opposing side, so grounded foes that switch in
have their Speed lowered one stage. Fails if that side is already webbed. Uses
`BattleStrings_Text_YourSideStickyWeb` / `..._TheFoesSideStickyWeb` via the
side-conscious message tag. `sticky_web/anim.s` plays Spider Web's particle
animation (`spider_web_spa`) instead of the placeholder white-flash.

### Wild Charge (new move, id 470)
`res/moves/wild_charge/{data.json,anim.s,script.s}` + `generated/moves.txt`
Electric-type physical, 90 power / 100 accuracy / 15 PP,
single target, `BATTLE_EFFECT_RECOIL_QUARTER` (the user takes 25% of the damage
dealt as recoil). `moveproc` generates its name/description text from `data.json`.

### Expanding Force (new move, id 469)
`res/moves/expanding_force/{data.json,anim.s,script.s}` + `generated/moves.txt`.
Psychic-type special, 80 power / 100 accuracy / 10 PP,
single target, `BATTLE_EFFECT_HIT`. `src/battle/battle_lib.c`
`BattleSystem_CalcMoveDamage` gives it ×1.5 power while Psychic Terrain is up and
the user is grounded (on top of the terrain's own generic ×1.3), approximating the
modern "power becomes 120 in terrain". Under the same condition it also **hits
both opponents** in a double battle: a new `Move_EffectiveRange` helper
(`src/battle/battle_lib.c`) reports `RANGE_ADJACENT_OPPONENTS` instead of
`RANGE_SINGLE_TARGET` for Expanding Force in that state, and is used by
`BattleSystem_Defender`, the spread-damage ×0.75 check, the
`LoopSpreadMoves` re-loop, the target-select layout, and the Pressure PP cost.

### Stat-boosting moves have very low PP
`res/moves/<move>/data.json` — every non-damaging move whose purpose is to raise
the user's (or a random / ally) stat stages now has **3 PP**, so setup can't be
spammed:

* Acid Armor, Acupressure, Agility, Amnesia, Barrier, Belly Drum, Bulk Up,
  Calm Mind, Charge, Cosmic Power, Curse, Defend Order, Defense Curl, Double
  Team, Growth, Harden, Howl, Iron Defense, Meditate, Minimize, Rock Polish,
  Sharpen, Stockpile, Tail Glow, Withdraw → **3 PP**.
* **Swords Dance, Nasty Plot, Dragon Dance → 1 PP.**

Damaging moves that happen to raise a stat (Charge Beam, Metal Claw, Ancient
Power, …) are untouched, as are stat-*lowering* moves and Focus Energy (raises
critical-hit ratio, not a stat).

Because every one of these moves now has a base max PP below the
`PP_UP_REQUIREMENT` of 5 (`src/item_use_pokemon.c`), a **PP Up or PP Max used on
any of them does nothing** — the party menu shows "It won't have any effect."
This is the existing Sketch guard, not new code.

### Accuracy buffs
`res/moves/<move>/data.json` — a batch of historically unreliable moves had their
`accuracy` raised (nothing else about them changed):

* **→ 100:** Tackle (95), Submission (80), Kinesis (80), Doom Desire (85).
* **→ 95:** Pin Missile (85), Fire Spin (70), Bounce (85), Bone Club (85),
  Mud Bomb (85), Sand Tomb (70), Barrage (85), Bind (75), Comet Punch (85),
  Double Slap (85), Double Hit (90), Fury Attack (85), Fury Swipes (80),
  Mega Punch (85), Slam (75), Take Down (85), Wrap (85), Smog (70),
  Psywave (80), Steel Wing (90), Mirror Shot (85), Clamp (75), Octazooka (85),
  Whirlpool (70), Aqua Tail (90).
* **→ 90:** Rock Climb (85), Head Smash (80), Rock Blast (80), Rock Tomb (80),
  Crabhammer (85).
* **→ 85:** Thunder (70), Focus Blast (70), Magma Storm (70), Blizzard (70),
  Dragon Rush (75), Egg Bomb (75), Mega Kick (75), Gunk Shot (70),
  Poison Gas (55), Poison Powder (75), Glare (75), Stone Edge (80),
  Iron Tail (75), Hydro Pump (80), Will-O-Wisp (75).

---

## Species changes

Stat lines below are **vanilla → current**; every base stat total is preserved
from the base game. Ability lines give the slot and the vanilla → current
ability; any slot not mentioned is unchanged.

### Bulbasaur
* Ability slot 2: none → **Chlorophyll**.

### Ivysaur
* Ability slot 2: none → **Chlorophyll**.

### Venusaur
* Ability slot 2: none → **Chlorophyll**.

### Charmander
* Ability slot 2: none → **Solar Power**.

### Charmeleon
* Ability slot 2: none → **Solar Power**.

### Charizard
* Ability slot 2: none → **Solar Power**.

### Squirtle
* Ability slot 2: none → **Rain Dish**.

### Wartortle
* Ability slot 2: none → **Rain Dish**.

### Blastoise
* Ability slot 2: none → **Rain Dish**.

### Beedrill
* Learnset: X-Scissor replaces Rage at level 19; Cross Poison replaces Toxic
  Spikes at level 25.

### Sandshrew
* Ability slot 1: Sand Veil → **Rough Skin**.

### Sandslash
* Ability slot 1: Sand Veil → **Rough Skin**.

### Nidoqueen
* Ability slot 2: Rivalry → **Sheer Force** (slot 1 stays Poison Point).

### Nidoking
* Ability slot 2: Rivalry → **Sheer Force** (slot 1 stays Poison Point).

### Vulpix
* Ability slot 2: none → **Drought** (slot 1 stays Flash Fire).

### Ninetales
* Ability slot 2: none → **Drought** (slot 1 stays Flash Fire).

### Mankey
* Ability slot 1: Vital Spirit → **Defiant** (slot 2 stays Anger Point).

### Primeape
* Ability slot 1: Vital Spirit → **Defiant** (slot 2 stays Anger Point).

### Parasect
* Stats: HP 60 → 90, Sp. Atk 60 → 30. BST 405 (unchanged).

### Ponyta
* Evolves into Rapidash at level 30 (was 40).

### Slowpoke
* Evolves into Slowbro at level 30 (was 37). The King's Rock → Slowking
  evolution is unchanged — like every trade evolution in this romhack it now
  happens by using the item on it, not by trading (see Link Cable).

### Farfetch'd
* Ability slot 1: Keen Eye → **Defiant** (slot 2 stays Inner Focus).

### Onix
* Stats: Atk 45 → 70, Spe 70 → 45. BST 385 (unchanged).
* Evolution: trade holding Metal Coat → use Metal Coat (see Link Cable).

### Tangela
* Learnset: Power Whip moved from level 54 to 40, Natural Gift from 40 to 54.

### Seaking
* Stats: Atk 92 → 100, Spe 68 → 60. BST 450 (unchanged).

### Jolteon
* Learnset: Thunderbolt replaces Thunder Fang at level 43; Discharge replaces
  Last Resort at level 50 (moved up from its old level-78 slot).

### Flareon
* Learnset: Fire Fang and Fire Spin swapped (Fire Fang @ 36, Fire Spin @ 43);
  Flare Blitz replaces Fire Blast at level 71.

### Omanyte
* Evolves into Omastar at level 30 (was 40).

### Kabuto
* Evolves into Kabutops at level 30 (was 40).

### Chikorita
* Ability slot 2: none → **Leaf Guard**.

### Bayleef
* Ability slot 2: none → **Leaf Guard**.

### Meganium
* Ability slot 2: none → **Leaf Guard**.

### Cyndaquil
* Ability slot 2: none → **Flash Fire**.

### Quilava
* Ability slot 2: none → **Flash Fire**.

### Typhlosion
* Ability slot 2: none → **Flash Fire**.

### Totodile
* Ability slot 2: none → **Sheer Force**.

### Croconaw
* Ability slot 2: none → **Sheer Force**.

### Feraligatr
* Ability slot 2: none → **Sheer Force**.

### Furret
* Stats: Atk 76 → 91, Spe 90 → 100, Sp. Atk 45 → 20. BST 415 (unchanged).

### Noctowl
* Stats: Atk 50 → 26, Sp. Atk 76 → 100. BST 442 (unchanged).

### Ledian
* Stats: Atk 35 → 90, Sp. Atk 55 → 20, Sp. Def 110 → 90. BST 390 (unchanged).

### Spinarak
* Learnset: Spider Web at level 29 → **Sticky Web**.

### Ariados
* Stats: Atk 90 → 100, Sp. Atk 60 → 50. BST 390 (unchanged).
* Learnset: Spider Web at level 32 → **Sticky Web**.

### Togepi
* Learnset: gains Extrasensory at level 17.

### Togetic
* Learnset: gains Extrasensory at level 17.

### Xatu
* Stats: Atk 75 → 60, Sp. Atk 95 → 110. BST 470 (unchanged).

### Ampharos
* Ability slot 2: none → **Electric Surge**.

### Politoed
* Ability slot 2: Damp → **Drizzle**.

### Jumpluff
* Learnset: gains Tailwind at level 30.

### Sunflora
* Stats: Atk 75 → 65, Sp. Atk 105 → 115. BST 425 (unchanged).

### Yanma
* Learnset: Ancient Power and Pursuit swapped (Ancient Power @ 30, Pursuit @ 33);
  Wing Attack moved from level 43 to 36; Bug Buzz moved from level 57 to 43.
  Hypnosis stays at 38; the old level-57 slot is gone.

### Unown
* Stats: Atk 72 → 44, Sp. Atk 72 → 100. BST 336 (unchanged).

### Girafarig
* Stats: HP 70 → 90, Atk 80 → 60, Def 65 → 90, Spe 85 → 65, Sp. Atk 90 → 60,
  Sp. Def 65 → 90. BST 455 (unchanged).

### Dunsparce
* Stats: Def 70 → 90, Spe 45 → 25. BST 415 (unchanged).

### Qwilfish
* Stats: HP 65 → 90, Spe 85 → 60. BST 430 (unchanged).

### Magcargo
* Ability slot 1: Magma Armor → **Solid Rock**.

### Swinub
* Ability slot 1: Oblivious → **Thick Fat** (slot 2 stays Snow Cloak).

### Piloswine
* Ability slot 1: Oblivious → **Thick Fat** (slot 2 stays Snow Cloak).

### Corsola
* Stats: HP 55 → 90, Sp. Atk 65 → 30. BST 380 (unchanged).

### Octillery
* Stats: Atk 105 → 85, Sp. Atk 105 → 125. BST 480 (unchanged).

### Delibird
* Stats: Atk 55 → 25, Sp. Atk 65 → 95. BST 330 (unchanged).
* Ability slot 2: Hustle → **Snow Warning**.
* Learnset: Swift @ 10, Ice Shard @ 15, Icy Wind @ 20, Fake Out @ 30,
  Blizzard @ 40.

### Mantine
* Stats: Atk 40 → 20, Sp. Atk 80 → 100. BST 465 (unchanged).

### Houndoom
* Learnset: gains Dark Pulse at level 66.

### Stantler
* Stats: Atk 95 → 110, Sp. Atk 85 → 70. BST 465 (unchanged).

### Treecko
* Ability slot 2: none → **Unburden**.

### Grovyle
* Ability slot 2: none → **Unburden**.

### Sceptile
* Ability slot 2: none → **Unburden**.

### Torchic
* Ability slot 2: none → **Speed Boost**.

### Combusken
* Ability slot 2: none → **Speed Boost**.

### Blaziken
* Ability slot 2: none → **Speed Boost**.

### Mudkip
* Ability slot 2: none → **Damp**.

### Marshtomp
* Ability slot 2: none → **Damp**.

### Swampert
* Ability slot 2: none → **Damp**.

### Mightyena
* Stats: Spe 70 → 100, Sp. Atk 60 → 30. BST 420 (unchanged).

### Linoone
* Stats: Atk 70 → 90, Sp. Atk 50 → 30. BST 420 (unchanged).

### Beautifly
* Stats: Atk 70 → 50, Sp. Def 50 → 70. BST 385 (unchanged).

### Dustox
* Stats: Atk 50 → 30, Sp. Atk 50 → 70. BST 385 (unchanged).

### Shiftry
* Stats: Atk 100 → 120, Sp. Atk 90 → 70. BST 480 (unchanged).

### Masquerain
* Stats: Atk 60 → 30, Spe 60 → 110, Sp. Def 82 → 62. BST 414 (unchanged).

### Breloom
* Learnset: gains Spore at level 50.

### Exploud
* Learnset: gains Boomburst at level 45; its level-up Roar was dropped to stay
  within the 20-move learnset cap (Roar is still on TM05 for it).

### Sableye
* Ability slot 2: Stall → **Prankster**.
* Learnset: gains Gravity at level 30, loses Mean Look at level 57.

### Mawile
* Stats: Atk 85 → 105, Spe 50 → 30. BST 380 (unchanged).

### Plusle
* Stats: Sp. Atk 85 → 105, Sp. Def 75 → 55. BST 405 (unchanged).

### Minun
* Stats: Atk 40 → 20, Def 50 → 70. BST 405 (unchanged).

### Volbeat
* Stats: Atk 73 → 58, Spe 85 → 100. BST 400 (unchanged).
* Ability slot 2: Swarm → **Prankster**.

### Illumise
* Stats: Atk 47 → 32, Sp. Atk 73 → 88. BST 400 (unchanged).
* Ability slot 1: Oblivious → **Prankster**.

### Swalot
* Stats: Def 83 → 98, Spe 55 → 25, Sp. Def 83 → 98. BST 467 (unchanged).

### Sharpedo
* Stats: Def 40 → 85, Sp. Atk 95 → 50. BST 460 (unchanged).

### Wailord
* Stats: Def 45 → 55, Spe 60 → 40, Sp. Def 45 → 55. BST 500 (unchanged).
* Ability slot 2: Oblivious → **Filter**.

### Camerupt
* Stats: HP 70 → 90, Spe 40 → 20. BST 460 (unchanged).
* Ability slot 1: Magma Armor → **Anger Point**.

### Torkoal
* Ability slot 2: none → **Drought**.

### Grumpig
* Ability slot 2: Own Tempo → **Psychic Surge**.

### Spinda
* Ability slot 1: Own Tempo → **Contrary**.

### Flygon
* Stats: Atk 100 → 120, Sp. Atk 80 → 60. BST 520 (unchanged).

### Cacturne
* Ability slot 1: Sand Veil → **Shed Spines**.

### Zangoose
* Ability slot 2: none → **Toxic Boost**.

### Crawdaunt
* Learnset: Swift @ 30 → Aqua Jet @ 30, Taunt @ 34 → Sucker Punch @ 34.

### Baltoy
* Learnset: Recover replaces Power Trick at level 31.

### Claydol
* Learnset: Recover replaces Power Trick at level 31.

### Lileep
* Ability slot 2: none → **Storm Drain** (slot 1 stays Suction Cups).

### Cradily
* Ability slot 2: none → **Storm Drain** (slot 1 stays Suction Cups).

### Anorith
* Ability slot 2: none → **Swift Swim** (slot 1 stays Battle Armor).

### Armaldo
* Ability slot 2: none → **Swift Swim** (slot 1 stays Battle Armor).

### Milotic
* Ability slot 2: none → **Competitive** (slot 1 stays Marvel Scale).

### Castform
* Stats: Atk 70 → 40, Def 70 → 55, Spe 70 → 100, Sp. Atk 70 → 100,
  Sp. Def 70 → 55. BST 420 (unchanged).

### Kecleon
* Stats: Atk 90 → 100, Spe 40 → 30. BST 440 (unchanged).
* Ability slot 2: none → **Protean**.

### Banette
* Ability slot 1: Insomnia → **Prankster**.
* Learnset: Grudge moved from level 58 to 25, Trick from 66 to 58, gains
  Destiny Bond at level 66.

### Chimecho
* Typing: Psychic → **Psychic / Steel**.
* Learnset: Protect replaces Double-Edge at level 33; Wish replaces Safeguard at
  level 41.

### Glalie
* Ability slot 1: Inner Focus → **Refrigerate**.

### Walrein
* Stats: Atk 80 → 95, Sp. Atk 95 → 80. BST 530 (unchanged).

### Luvdisc
* Learnset: Lucky Chant and Captivate swapped — Captivate now at level 17,
  Lucky Chant at level 40.

### Turtwig
* Ability slot 2: none → **Shell Armor**.

### Grotle
* Ability slot 2: none → **Shell Armor**.

### Torterra
* Ability slot 2: none → **Shell Armor**.

### Chimchar
* Ability slot 2: none → **Iron Fist**.

### Monferno
* Ability slot 2: none → **Iron Fist**.

### Infernape
* Ability slot 2: none → **Iron Fist**.

### Piplup
* Ability slot 2: none → **Competitive**.

### Prinplup
* Ability slot 2: none → **Competitive**.

### Empoleon
* Ability slot 2: none → **Competitive**.

### Kricketune
* Ability slot 1: Swarm → **Technician**; ability slot 2: none → **Technician**
  (Technician in both slots).
* Learnset: Aerial Ace replaces X-Scissor at level 30; Bug Bite replaces
  Screech at level 34.

### Shinx
* Ability slot 1: Rivalry → **Guts** (slot 2 stays Intimidate).

### Luxio
* Ability slot 1: Rivalry → **Guts** (slot 2 stays Intimidate).

### Luxray
* Ability slot 1: Rivalry → **Guts** (slot 2 stays Intimidate).
* Learnset: Wild Charge replaces Scary Face at level 49.

### Cranidos
* Ability slot 2: none → **Sheer Force** (slot 1 stays Mold Breaker).

### Rampardos
* Ability slot 2: none → **Sheer Force** (slot 1 stays Mold Breaker).

### Shieldon
* Ability slot 2: none → **Soundproof** (slot 1 stays Sturdy).

### Bastiodon
* Ability slot 2: none → **Soundproof** (slot 1 stays Sturdy).

### Wormadam
One species, three cloak forms (`res/pokemon/wormadam/data.json` for Plant Cloak,
`forms/sandy/data.json` and `forms/trash/data.json` for the others).

* Stats: +20 Attack, -20 Sp. Atk on every form. BST unchanged (424 each).

| Cloak | Attack | Sp. Atk | Speed |
| --- | --- | --- | --- |
| Plant | 59 → 99 | 79 → 39 | 36 |
| Sandy | 79 → 99 | 59 → 39 | 36 |
| Trash | 69 → 79 | 69 → 79 | 36 → 16 |

* Learnset (each cloak has its own list): the level-29 status move
  (Growth / Harden / Metal Sound) becomes **Leech Life** on all three; at level 47
  Plant's Leaf Storm becomes **Seed Bomb** and Sandy's Fissure becomes
  **Earthquake** (Trash keeps Iron Head); all three gain **Sucker Punch** at
  level 50. The level-26 cloak move (Razor Leaf / Rock Blast / Mirror Shot) was
  left as is.

### Mothim
* Stats: Atk 94 → 74, Sp. Atk 94 → 114. BST 424 (unchanged).

### Combee
* Gender ratio: 12.5% female → **75% female** (`GENDER_RATIO_FEMALE_75`).

### Pachirisu
* Ability slot 2: Pickup → **Volt Absorb**.

### Cherrim
* Stats: HP 70 → 90, Spe 85 → 65. BST 450 (unchanged).

### Gastrodon
* Ability slot 1: Sticky Hold → **Storm Drain** (slot 2 already Storm Drain), so
  Storm Drain is now its only ability. Shellos is unchanged.

### Drifloon
* Ability slot 1: Aftermath → **Flare Boost** (slot 2 stays Unburden).

### Drifblim
* Ability slot 1: Aftermath → **Flare Boost** (slot 2 stays Unburden).

### Buneary
* Ability slot 1: Run Away → **Limber**; ability slot 2: Klutz → **Scrappy**.

### Lopunny
* Ability slot 1: Cute Charm → **Limber**; ability slot 2: Klutz → **Scrappy**.

### Honchkrow
* Ability slot 1: Insomnia → **Moxie** (slot 2 stays Super Luck).

### Glameow
* Evolves into Purugly at level 17 (was 38).

### Purugly
* Abilities: Thick Fat / Own Tempo → **Defiant** (slot 1) / **Thick Fat**
  (slot 2). Own Tempo is dropped.

### Stunky
* Evolves into Skuntank at level 24 (was 34).

### Chatot
* Stats: Atk 65 → 55, Spe 91 → 101. BST 411 (unchanged).

### Garchomp
* Ability slot 1: Sand Veil → **Rough Skin**. Gible and Gabite are unchanged.

### Croagunk
* Ability slot 1: Anticipation → **Poison Touch** (slot 2 stays Dry Skin).

### Toxicroak
* Ability slot 1: Anticipation → **Poison Touch** (slot 2 stays Dry Skin).

### Carnivine
* Stats: Atk 100 → 130, Sp. Atk 90 → 60. BST 454 (unchanged).

### Finneon
* Evolves into Lumineon at level 20 (was 31).

### Lumineon
* Learnset: Tailwind replaces Captivate at level 26.

### Lickilicky
* Ability slot 1: Own Tempo → **Cloud Nine**; ability slot 2: Oblivious →
  **Filter**.

### Tangrowth
* Learnset: Power Whip moved from level 54 to 40, Natural Gift from 40 to 54.

### Togekiss
* Learnset: also knows Ancient Power, Last Resort, Wish, Follow Me, Encore,
  Yawn and Extrasensory as level-1 moves.

### Yanmega
* Learnset: Ancient Power and Pursuit swapped (Ancient Power @ 30, Pursuit @ 33);
  the level-43 Slash becomes Wing Attack at level 36; Bug Buzz moved from level 57
  to 43.

### Leafeon
* Learnset: Seed Bomb replaces Magical Leaf at level 36.

### Glaceon
* Learnset: Ice Beam replaces Ice Fang at level 43.

### Gliscor
* Ability slot 2: Sand Veil → **Poison Heal** (slot 1 stays Hyper Cutter).

### Mamoswine
* Ability slot 1: Oblivious → **Thick Fat** (slot 2 stays Snow Cloak).

### Gallade
* Ability slot 1: Steadfast → **Justified**; ability slot 2: Steadfast →
  **Sharpness**.

### Dusknoir
* Stats: HP 45 → 75, Sp. Atk 65 → 35. BST 525 (unchanged).

### Froslass
* Stats: Atk 80 → 50, Sp. Atk 80 → 110. BST 480 (unchanged).
* Ability slots 1 and 2: Snow Cloak → **Snow Warning** (Snow Warning is now its
  only ability).

### Rotom
Each appliance form now takes a second type like the modern games, replacing
Ghost (`res/pokemon/rotom/forms/<form>/data.json`). The type follows the form
automatically because `MON_DATA_TYPE_1/2` is derived from per-form species data.
Base Rotom stays Electric / Ghost.

| Form | Typing |
| --- | --- |
| Heat | Electric / Fire |
| Wash | Electric / Water |
| Frost | Electric / Ice |
| Fan | Electric / Flying |
| Mow | Electric / Grass |

### Uxie
* Ability slot 1: Levitate → **Psychic Surge**; ability slot 2: none →
  **Psychic Surge** (Psychic Surge is now its only ability, creating Psychic
  Terrain on switch-in).
* Learnset: Extrasensory at level 51 → **Expanding Force**.

### Mesprit
* Ability slot 1: Levitate → **Psychic Surge**; ability slot 2: none →
  **Psychic Surge** (Psychic Surge is now its only ability, creating Psychic
  Terrain on switch-in).
* Learnset: Extrasensory at level 51 → **Expanding Force**.

### Azelf
* Ability slot 1: Levitate → **Psychic Surge**; ability slot 2: none →
  **Psychic Surge** (Psychic Surge is now its only ability, creating Psychic
  Terrain on switch-in).
* Learnset: Extrasensory at level 51 → **Expanding Force**.

### Arceus
* Learnset: also knows **Spacial Rend**, **Roar of Time** and **Shadow Force**
  from the start (added at level 1).

---

## Level cap (`5c436cf12`)

A hard cap on Pokémon level, stored in `VAR_HARD_LEVEL_CAP`.

* Seeded to `DEFAULT_HARD_LEVEL_CAP` (14) by `scripts_init_new_game.s`. A stored
  0 (pre-feature save) is treated as 14. Scripts only ever raise it from there.
* A full 9-tier ladder, one `SetVar VAR_HARD_LEVEL_CAP, <level>` added right
  after each gym leader's `GiveBadge`/`IncrementTrainerScore2
  TRAINER_SCORE_EVENT_BADGE_EARNED` lines in their gym script: 14 (start) → 23
  (Roark) → 30 (Gardenia) → 37 (Fantina) → 45 (Maylene) → 51 (Wake) → 57
  (Byron) → 62 (Candice) → 67 (Volkner).
* Helpers in `src/pokemon.c`: `Pokemon_GetHardLevelCap`,
  `Pokemon_ClampExpToHardLevelCap`, `Pokemon_BelowHardLevelCap`. Var accessors
  `SystemVars_GetHardLevelCap` / `SystemVars_SetHardLevelCap` in
  `src/system_vars.c`.
* Enforced at every Exp. source:
  * `src/battle/battle_script.c` `BattleScript_GetExpTask` — a Pokémon can reach
    the cap but gains no Exp. past it. Skipped in link battles.
  * `src/overlay005/daycare.c` — Day Care Exp. is clamped to the cap.
  * `src/item_use_pokemon.c` — a Rare Candy jumps a Pokémon **straight to the
    cap** in one use (not one level).
* `src/applications/party_menu/callbacks.c` + `Pokemon_LevelUpMoveUpTo` in
  `src/pokemon.c` — after the multi-level Rare Candy jump, the party menu offers
  every level-up move that was skipped across all the gained levels.
  **Bug fix**: `Pokemon_LevelUpMoveUpTo` originally had no lower bound at all —
  it scanned the *entire* learnset from level 1 and used "is this move
  currently sitting in one of the Pokémon's 4 move slots" as a stand-in for
  "already handled." That's a false proxy for anything above roughly level
  20-30, since normal leveling naturally forgets/overwrites most early
  level-up moves long before reaching a higher level — so jumping a level 50
  Pokémon to 60 with one Rare Candy re-offered its *entire* learnset back to
  level 1, not just levels 51-60. Fixed by capturing the pre-jump level in
  `PartyMenuCB_UseItem_RareCandy` (a new `oldLevel` field on `PartyMenu`,
  `include/applications/party_menu/defs.h`) and threading it into
  `Pokemon_LevelUpMoveUpTo` as an actual lower bound (`include/pokemon.h`)
  — the move-slot check remains as a secondary guard, but no longer does the
  range-bounding on its own.
* When the rival catches up to you on Route 201 to propose going to check
  out the lake (`Route201_CoordEvent_RivalStartFollowing` →
  `Route201_SetRivalPartner` in `scripts_route_201.s`, right after he starts
  following as your partner), he now also hands over **999 Rare Candies**
  ("Oh, I found these on the floor. Maybe you'll find a use for them." —
  new `Route201_Text_FoundTheseOnFloor` in `res/text/route_201.json`), the
  max stack size (`BAG_MAX_QUANTITY_ITEM` in `src/bag.c`). Guarded by the
  usual `GoToIfCannotFitItem` bag-space check, falling through to
  `Route201_ContinueAfterRareCandy` if the bag can't fit them so the story
  isn't blocked.

### Leftover testing scaffolding removed
While building the level cap feature, a few testing conveniences were left
enabled in the tree and never should have shipped on `main`:
* Route 201's wild encounters (`res/field/encounters/encounters_route_201.json`)
  had been swapped to a 90% chance of a level 50 Dialga/Palkia/Giratina,
  presumably to make it easy to test abilities against them on the very first
  route. Reverted to the original level 2-3 Starly/Bidoof/Kricketot at a 30%
  rate.
* The Twinleaf Town guitarist (`TwinleafTown_Guitarist` in
  `scripts_twinleaf_town.s`) handed over 99 Master Balls and a Rare Candy the
  first time you talked to him — reverted back to his original dialogue with
  no item hand-off, and the now-unused
  `TwinleafTown_Text_GuitaristRareCandyGift` entry removed from
  `res/text/twinleaf_town.json`. The flag it repurposed
  (`FLAG_RECEIVED_TWINLEAF_TOWN_GUITARIST_RARE_CANDY`) is renamed back to
  `FLAG_UNUSED_0x0114` in `generated/vars_flags.txt`.
* Three redundant `SetVar VAR_HARD_LEVEL_CAP, 14`/`60` lines (in
  `Route201_CoordEvent_ChooseStarterScene`, `TwinleafTown_Guitarist`, and
  `TwinleafTown_RivalWentTearingOffOuch`) were left over from testing the cap
  at different values before it settled on the real 9-tier ladder above —
  removed as dead weight, since `scripts_init_new_game.s` already seeds the
  cap to `DEFAULT_HARD_LEVEL_CAP` (14) once at save creation.

### Every pre-Elite-Four trainer relevelled to match the cap ladder
All 434 in-scope trainer files under `res/trainers/data/*.json` (every
line-of-sight or scripted trainer battle up to and including Volkner, minus
Battle Frontier, rematch, unused, dummy, leader/E4/champion, and other
postgame-only encounters) had every Pokémon in their party set to a single
target level derived from the map(s) they battle on, so trainer strength
tracks the 9-tier level-cap ladder (14/23/30/37/45/51/57/62/67) instead of the
vanilla decomp's levels:
* **Gym trainers** (identified by battling on one of the gym maps) →
  `(cap active when entering that gym, i.e. the previous milestone's cap) − 1`
  — Roark's gym is the first, so its trainers use the starting cap (14) minus
  1, not the cap Roark himself unlocks.
* **Team Galactic grunts** (`class` containing `GALACTIC_GRUNT`) →
  `(map tier's cap) − 3`.
* **Team Galactic bosses/commanders** (`class` containing `GALACTIC_BOSS` or
  `COMMANDER`) → `(map tier's cap) − 2`.
* **Rival battles** (`class` is `TRAINER_CLASS_RIVAL` — Route 201, Route 203,
  Route 209, Pastoria City, Canalave City, Spear Pillar, Pokémon League North
  Pokécenter; 21 files, 7 encounters × 3 starter variants) → `(map tier's
  cap) − 2`, same as a Galactic boss/commander. **Exception:** the very first
  rival battle (`rival_route_201_{chimchar,piplup,turtwig}.json`) keeps its
  vanilla level 5 instead — the level-cap ladder hasn't started yet at that
  point (Tier 1's cap of 14 wouldn't kick in until 2 levels below it, level
  12, which is too high for a level-5-or-6 starter fight).
* **Everyone else** → `(map tier's cap) − 4`.
* A trainer whose maps span more than one tier uses the lowest (earliest)
  tier/cap among them.
* Route 204 South's trainers (`lass_samantha`, `lass_sarah`,
  `youngster_tyler`) are tiered as Tier 1 (cap 14) rather than Tier 2 like
  the rest of Route 204/Floaroma Meadow/Valley Windworks, since that stretch
  is reachable before Roark. Now Lv. 10 (was Lv. 19).
* The 5 `*_battleground.json` roaming trainers (Cheryl, Riley, Marley, Buck,
  Mira) were left untouched — `battleground` is physically connected to and
  functionally part of the postgame Survival Area
  (`res/field/events/events_battleground.json` references
  `MAP_HEADER_SURVIVAL_AREA`), despite not matching the existing postgame map
  name filters.
* The whole Team Galactic climax — Galactic HQ (all floors/Control Room),
  Lake Verity, Lake Valor Drained, Valor Cavern, Spear Pillar, and Distortion
  World B7F — is tiered as happening between Candice and Volkner (the 7th and
  8th badges), not after Volkner, so it uses the post-Candice cap (62)
  instead of the post-Volkner one (67).
* Applied via a one-off script (not checked into the repo) that reads each
  trainer JSON, sets every `party[].level` to the computed target, and
  rewrites the file with `json.dump(..., indent=4, ensure_ascii=False)`,
  preserving each file's original trailing-newline convention.

### Level cap increases are announced in-game
Each of the 8 gym scripts now prints "The level cap has been raised to
`X`!" right after the badge is awarded and `VAR_HARD_LEVEL_CAP` is set to
the new value (`res/field/scripts/scripts_{oreburgh,eterna,hearthome}_city_gym{,_leader_room}.s`,
`scripts_{veilstone,pastoria,canalave,snowpoint}_city_gym.s`,
`scripts_sunyshore_city_gym_room_3.s`) — `BufferNumber 0,
VAR_HARD_LEVEL_CAP` followed by a new `<Gym>_Text_LevelCapRaised` message
(`{STRVAR_1 52, 0, 0}` in the text) reads the cap straight back out of the
variable that was just set, so the number can't drift out of sync with the
`SetVar` above it. The very first cap set (game start, 14) is the initial
value rather than an increase, so it doesn't get a message.

---

## Trainer data

### Every trainer now uses the Evaluate Attack and Expert AI flags by default
`res/trainers/data/*.json` — every trainer's `ai_flags` now includes
`AI_FLAG_EVAL_ATTACK` and `AI_FLAG_EXPERT` alongside whatever it already had
(`AI_FLAG_BASIC`, plus any of `AI_FLAG_PRIORITIZE_EXTREMES`,
`AI_FLAG_SETUP_FIRST_TURN`, `AI_FLAG_RISKY` it already used), in the same
`BASIC, EVAL_ATTACK, EXPERT, ...` order the 232 trainers that already had
both were already using. 695 of the 928 trainer files were missing one or
both and got them added; 232 already had both and were untouched;
`none.json` (`TRAINER_NONE`, no party) was left alone. This makes every
trainer battle use full attack-effectiveness evaluation and expert-level
move scoring (Substitute/Distortion Terrain awareness, Sucker Punch
prediction, kill-confirmation logic, etc.) instead of only the ~1/3 of
trainers that previously had `AI_FLAG_EXPERT` and/or `AI_FLAG_EVAL_ATTACK`
explicitly set, matching the general "every fight should be a real fight"
intent of this hack.

### Every trainer Pokémon now has perfect IVs, and can have a fixed nature
Every Pokémon on every non-Frontier trainer (gym trainers, rivals, random NPCs
— anything built by `TrainerData_BuildParty` in `src/trainer_data.c`) now
always has 31 IVs in every stat, matching a hardcore-nuzlocke expectation
that trainer battles are a fair fight on IVs. Previously, each party member's
`iv_scale` (0–255, JSON) was scaled down to a single flat IV (0–31) applied to
all six stats — most trainers had middling IVs, not perfect ones.

* `include/struct_defs/trainer_data.h` — the now-unused `ivScale` field (and
  `MAX_IV_SCALE`) was replaced in place with `nature` in all four
  `TrainerMon*` structs — same slot, same `u16` width, so the packed struct's
  size and layout are completely unchanged (both were already the first
  `u16` field in an all-`u16` struct, so there's no padding to get wrong).
* `res/trainers/data/*.json` — every party member's `"iv_scale"` key was
  replaced with `"nature"`. `null` (used everywhere here) means "no specific
  nature requested," the same convention already used by `item`.
* `tools/dataproc/src/trainerproc.c` — parses `"nature"` as `enum Nature`
  when it's a string, or the sentinel `NATURE_COUNT` (one past the last real
  nature, so it can never collide with a real value) when it's `null`,
  mirroring the existing `item`/`moves` optional-field pattern exactly.
* `src/trainer_data.c` — new shared `TrainerData_MakePersonality` helper
  (previously this logic was duplicated identically across all four
  `TRDATATYPE_*` cases) builds the personality value exactly as before
  (preserving the existing trainer-class-gender bias baked into its low
  byte), then, if a specific nature was requested, keeps re-rolling the
  upper bits — via the same reject-sampling loop already used for Battle
  Frontier trainers (`frontier_opponents.c`) and fixed-nature static
  encounters (`sub_02074044` in `pokemon.c`) — until `Pokemon_GetNatureOf`
  of the result matches. A request for `NATURE_COUNT` or higher is treated
  as "leave it random" rather than looping forever trying to match a nature
  `Pokemon_GetNatureOf` can never produce. `Pokemon_InitWith`'s IV argument
  is now always the literal `MAX_IVS_SINGLE_STAT` (31) instead of a
  per-mon-computed value.
* `docs/datafiles/trainers.md` updated to match.
* Battle Frontier trainers (`res/trainers/frontier/`) are unaffected — they
  already have their own separate, rank-tiered IV system and their own
  `nature` field (`FrontierPokemonBase` in `frontier_opponents.c`), unrelated
  to this one.

### Docs fix: the `item`/`moves` "all-or-nothing" rule is decided by `party[0]` only, and is asymmetric
`docs/datafiles/trainers.md` previously claimed that if *any* Pokémon in a
trainer's party specifies an `item` or a custom `moves` list, every other
Pokémon must too. That's not what `trainerproc.c` actually does, and one of
the doc's own "invalid" examples was wrong as a result (it builds
successfully). Verified against the real tool by deliberately breaking builds
both ways:

* The check is made *only* against `party[0]` (`trainerproc.c`'s
  `party_has_items`/`party_has_moves`), never any other member.
* If `party[0]` has a real `item`, every other member is required to as well
  — `null` anywhere else is a build error (confirmed).
* If `party[0]` has `"item": null`, the tool never even reads `item` from any
  other member — a real item there is silently dropped with no error
  (confirmed; this is the direction the old doc got backwards). Scanned all
  928 `res/trainers/data/*.json` files for this pattern; none currently hit
  it.
* The same rule applies to `moves`, independently.

**New capability, no code changes needed:** this means a party couldn't
previously have an itemless lead alongside teammates holding real items,
since `"item": null` on the lead blocks the tool from reading anyone else's
`item` field. Fixed by documenting the existing `"ITEM_NONE"` value (already
a normal member of `enum Item`, value 0) as the way to opt a specific
Pokémon out of holding an item *without* opting the whole party out of item
data — `"item": "ITEM_NONE"` satisfies the same "`party[0]` has a string"
check as any real item name, so later party members' real items are read
normally. Confirmed by deliberately building a party with `"item":
"ITEM_NONE"` on the lead and a real item on the second Pokémon. There's no
equivalent for `moves` (no "use my default level-up moves" move exists), so
a Pokémon that needs its natural moveset can't be mixed with a custom-moveset
party-mate that way.

### New player-facing doc: `TRAINER_ENCYCLOPEDIA.md`
Added `TRAINER_ENCYCLOPEDIA.md` at the repo root, a generated reference
listing every pre-Elite-Four trainer (434 of them) in the same order and
level-cap-tier grouping as `res/trainers/TRAINER_LOCATIONS.md`, with each
Pokémon's species, ability, nature, moves, and held item, plus each trainer's
AI flags and any battle items. Built by a one-off script (not part of the
build) that parses `TRAINER_LOCATIONS.md` for trainer order/grouping, reads
each referenced `res/trainers/data/*.json`, and cross-references
`res/pokemon/<species>/data.json` for ability names — deduplicating the
handful of species whose `abilities` array lists the same non-`ABILITY_NONE`
value twice (e.g. Glaceon) instead of padding the second slot with
`ABILITY_NONE`. Like `TRAINER_LOCATIONS.md`, this file goes stale if trainers
are moved, re-leveled, re-teamed, or individually curated by hand, and isn't
automatically regenerated by the build.

### Trainer Pokémon can now have a curated ability, and unset nature/ability default to something specific instead of "random"
Every Pokémon on every non-Frontier trainer can now have its ability slot
explicitly chosen, the same way `nature` already worked. Also changes what
an *unset* `nature` means: it used to leave the personality value's nature
component unconstrained (effectively random); it now defaults to a specific
neutral nature (`NATURE_HARDY`) instead, as a deliberate placeholder for the
trainers that haven't had a nature hand-picked for them yet.

* `res/trainers/data/*.json` — every party member gained a new `"ability"`
  key (`null` or an `enum Ability` string), inserted right after `"nature"`.
  `null` means "no override" and resolves to the species' first ability slot
  by default. All 928 files currently have `"ability": null` — no trainer's
  ability has been curated yet, same starting point as `nature` before it.
* `tools/dataproc/src/trainerproc.c` — a `"nature": null` party member now
  resolves to `NATURE_HARDY` instead of the `NATURE_COUNT` "leave it random"
  sentinel (the sentinel/reject-sampling machinery for it is still there in
  `TrainerData_MakePersonality`, just currently unreachable from JSON data,
  in case that's wanted again later). A new `resolve_ability` helper resolves
  `"ability"`: when set, it cross-references `res/pokemon/<species>/data.json`
  to confirm the requested ability is actually one of that species' two real
  abilities, raising a build error otherwise (confirmed by testing both a
  valid override and a deliberately wrong one, e.g. giving an Onix
  `ABILITY_LEVITATE`). The tool now takes a required third `POKEMONDIR`
  argument (`res/pokemon`) for this cross-reference, wired up in
  `res/trainers/meson.build`.
* `include/struct_defs/trainer_data.h` — all four `TrainerMon*` structs
  gained a new `ability` field, added right after `nature` and kept `u16`
  like every other field in these structs (packed field-by-field by
  `trainerproc.c`'s `pack()`, not by a raw `sizeof()` memcpy, so a new
  trailing field added consistently on both the tool and the runtime side is
  safe the same way the `nature` field itself was — see the struct-widening
  bug note above for why that distinction matters here).
* `src/trainer_data.c` — new `TrainerData_ResolveAbility(species, ability)`
  helper: returns the override directly if one was set, otherwise
  `SpeciesData_GetSpeciesValue(species, SPECIES_DATA_ABILITY_1)` (the
  species' first ability). Called once per party member in all four
  `TRDATATYPE_*` cases in `TrainerData_BuildParty`, and the result is written
  directly to `MON_DATA_ABILITY` after `Pokemon_InitWith` builds the mon —
  unlike nature/gender, ability is *not* derived from the personality value
  at all anymore, sidestepping vanilla's personality-parity-bit ability
  mechanic entirely rather than fighting it to force a specific slot.
* `TRAINER_ENCYCLOPEDIA.md` updated to reflect both defaults: an unforced
  ability shows the species' single resolved ability instead of listing both
  options, and an unforced nature shows `Hardy (default, not yet curated)`
  instead of `Random`.

### Underground disabled entirely
The Underground can no longer be entered at all. `CanUseExplorerKit`
(`src/item_use_functions.c`) — the sole entry point (its "return to the
surface" counterpart in `src/underground/menus.c` is only reachable from
inside an already-active Underground session, so gating this one function is
sufficient) — now unconditionally returns a new
`ITEM_USE_CANNOT_USE_UNDERGROUND_MAINTENANCE` result instead of running its
old checks (Mystery Zone, cycling road, Safari Game, surfing, etc. — all now
moot and removed). `include/item_use_functions.h` adds that enum value, and
`BagContext_FormatErrorMessage` (`src/bag_context.c`) prints a new message for
it — `Bag_Text_CannotUseUndergroundMaintenance`
(`res/text/bag.json`): *"Sorry, the underground is undergoing maintenance."*
— whether the Explorer Kit is used from the Bag, in the field, or registered
to Y.

### Shop purchase limits: TMs and select held items bought once, EV vitamins and PP Up capped at 10
Shops (the Veilstone Dept Store, the Battle Frontier's BP Exchange Service
Corner, and the Game Corner's Prize Corner) previously sold every item with
infinite stock. Two new lifetime purchase limits were added, both enforced
per save file and per item ID (so buying a TM from one shop marks it bought
everywhere else that sells it too):

* **TMs, Wide Lens, Zoom Lens, Metronome, and Shed Shell — one purchase, ever.**
  Every TM sold anywhere (Veilstone Dept Store 3F, the Battle Frontier's BP
  shop, and the Prize Corner) plus these four held items can each only be
  bought once per save. Once bought, the shop's item list shows **"Sold out!"**
  in place of the price, and trying to select it shows the same message
  instead of letting you buy a second one. (Shed Shell was added to this list
  later, when it replaced Poké Doll in the Veilstone Dept Store — see "Poké
  Doll replaced with Shed Shell" below.)
* **Protein, Iron, Calcium, Zinc, Carbos, HP Up, and PP Up — 10 total, ever,**
  summed across every shop that sells them (Veilstone Dept Store 2F and the
  Battle Frontier's BP shop). The shop's quantity picker caps out at however
  many of the 10 you have left, and once a type hits 10 it also shows "Sold
  out!". (Vitamins/PP Up found or won elsewhere, outside a shop purchase,
  aren't affected by this cap — only shop purchases count against it.) **Note:**
  no shop in this romhack currently sells PP Up at all (it's still obtainable
  as a fixed hidden-item pickup in several locations) — the cap is wired up and
  ready, but has nothing to enforce against until/unless PP Up is added to a
  shop's stock.
* `src/overlay007/shop_menu.c` — a new `sShopPurchaseLimits[]` table maps each
  limited item ID to its cap (1 or 10) and to the save flag/var backing its
  purchase count; `Shop_GetRemainingPurchaseAllowance`, `Shop_IsItemSoldOut`,
  and `Shop_RecordItemPurchase` (all exposed via `include/overlay007/shop_menu.h`)
  are hooked into `Shop_MenuPrintCallback` (the "Sold out!" label),
  `Shop_SelectBuyMenu` (blocking the purchase and clamping the quantity
  picker's max), and `Shop_ConfirmItemPurchase` (recording the purchase) — this
  covers every shop of `MART_TYPE_NORMAL` and `MART_TYPE_FRONTIER`, i.e. every
  money- or BP-based shop in the game, with no per-shop code needed.
* The Game Corner's Prize Corner (`res/field/scripts/scripts_veilstone_city_prize_exchange.s`,
  data in `src/scrcmd_game_corner_prize.c`) doesn't use the shared shop engine
  at all — it's a bespoke script-driven coin shop — so it got two new script
  commands instead, `CheckShopItemSoldOut`/`RecordShopItemPurchase`
  (`ScrCmd_CheckShopItemSoldOut`/`ScrCmd_RecordShopItemPurchase` in
  `src/scrcmd_shop.c`), which just call the same three C functions above. Its
  menu-building loop now shows `MenuEntries_Text_PrizeExchange_PrizeSoldOut`
  ("Sold out!") in place of the price for an already-bought prize, and
  `VeilstoneCityPrizeExchange_TryBuyPrize` rejects the purchase with the same
  message if the item is sold out.
* Save data: TMs/lenses/Metronome/Shed Shell are tracked with one save flag
  apiece (47 total — `FLAG_BOUGHT_TM04` .. `FLAG_BOUGHT_TM90`,
  `FLAG_BOUGHT_WIDE_LENS`, `FLAG_BOUGHT_ZOOM_LENS`, `FLAG_BOUGHT_METRONOME`,
  `FLAG_BOUGHT_SHED_SHELL`); the 6 vitamins plus PP Up are tracked with one
  save var apiece holding a running count 0–10 (`VAR_PROTEIN_BOUGHT_COUNT`,
  ..., `VAR_PP_UP_BOUGHT_COUNT`). All 54 were carved out of
  `generated/vars_flags.txt`'s already-existing-but-genuinely-unused
  `FLAG_UNUSED_0x*`/`VAR_UNUSED_0x*` slots (renamed in place, same numeric
  ID) rather than appended as new entries, so no existing flag/var shifts and
  no save compatibility is at risk.

### Overworld X-items replaced with treasure, Shards replaced with EV vitamins
Every hidden item (Dowsing Machine) and visible ground-item-ball placement in
the game that used to give an "X item" (X Speed/Attack/Defense/Sp. Atk/Sp.
Def/Accuracy, Guard Spec., Dire Hit) or one of the 4 Shards (Red/Blue/Yellow/Green)
was reassigned to a different item. This only covers items you find lying
around or dig up — shop stock was already handled separately (see above). At
the time, a small number of *NPC-given or NPC-traded* Shards were deliberately
left alone as out of scope; two of those have since also been converted (see
"Gifted Shards now give a Heart Scale" below).

* **X-items → treasure**, scaled roughly to when you'd first reach the spot
  (`res/field/scripts/scripts_visible_items.s`; no X-item was ever a hidden
  item, so only visible ground-item balls needed changing, and none of the 8
  are in a cave, so Nugget/Rare Bone weren't needed here): Route 203 (X
  Defense) and the Jubilife Trainers' School (X Attack) both → Tiny Mushroom;
  Oreburgh City (Dire Hit) and Route 205 South (X Sp. Def) both → Pearl;
  Route 205 North (Guard Spec.) → Big Mushroom; Team Galactic's Eterna
  building 2F (X Speed) → Stardust; its 3F (X Sp. Atk) → Big Pearl; Route 218
  (X Accuracy) → Star Piece.
* **Shards → EV vitamins**, cycled round-robin (Protein/Iron/Calcium/Zinc/
  Carbos/HP Up) with no particular progression logic, across:
  * 13 hidden-item table rows in `include/data/field/hidden_items.h` — all of
    Wayward Cave 1F's and Mt. Coronet's (1F North Rooms 1/2, 1F Tunnel Room
    ×2, B1F, 2F, 4F Rooms 1&2, Outside South) Shard entries.
  * 24 visible ground-item balls in `scripts_visible_items.s` — Oreburgh
    City, Eterna Forest, the Team Galactic Eterna building 4F, Routes 210
    North/212 North/212 South/213/214/217/225/227/230, all 4 Great Marsh
    areas, all 4 Fuego Ironworks building placements, Galactic HQ 2F, both
    Iron Island rooms, and Survival Area.
* Every change is a same-slot edit to an existing `HIDDEN_ITEM_ENTRY`'s
  `.item` field or an existing label's `SetVar VAR_0x8008, ITEM_x` line — no
  new flags, table rows, or object-event placements were added or removed, so
  there's no save-compatibility concern (the "already picked up" flags are
  unchanged; only what you receive differs).

### Gifted Shards now give a Heart Scale
Of the three NPC Shard sources left alone in the change above, two actually
*give* you a Shard outright (as opposed to trading for one) and are now
changed to give a Heart Scale instead:

* **Solaceon Ruins, Room 2** — the Hiker's one-time Green Shard reward for
  lending him HM Defog (`res/field/scripts/scripts_solaceon_ruins_room_2.s`,
  `SolaceonRuinsRoom2_LoanHMDefog`) now gives `ITEM_HEART_SCALE`. His dialogue
  never named the item, so no text changed.
* **Great Marsh, Area 6** — the daily Ace Trainer who used to hand out one
  random Shard (`res/field/scripts/scripts_great_marsh_6.s`,
  `GreatMarsh6_AceTrainerM`) now always gives a Heart Scale; the now-pointless
  4-way random branch/labels are removed rather than left dead. His two lines
  that explicitly said "Shards" (`res/text/great_marsh_6.json`) are reworded
  to say "Heart Scales" instead.
* **Mr. Fuego's Fuego Ironworks counter reworked**, since his old trade (give
  1 Star Piece, receive one of each of the 4 Shards) drew on a resource that
  no longer exists anywhere in the game after the changes above (checked and
  confirmed no species holds a Shard as a wild held item either — that part
  of the original request turned out to already be true, nothing to remove).
  He now trades **1 Star Piece for 1 Heart Scale** instead (same "trade 10 at
  once" bulk option still works) — `res/field/scripts/scripts_fuego_ironworks_building.s`,
  `FuegoIronworksBuilding_Trade1StarPiece` / `_Trade10StarPieces` now
  `AddItem ITEM_HEART_SCALE` instead of the 4 `AddItem ITEM_*_SHARD` calls.
  His dialogue (`res/text/fuego_ironworks_building.json`) was reworded to
  match — he now finds Heart Scales in the ironworks' water intake filters
  instead of Shards in the iron ore.

### Poké Doll replaced with Shed Shell; Revive/Max Revive/Revival Herb purged from hidden items
An audit of "does this item have any use once you can't use items on your own
Pokémon in battle" turned up two categories of finding:

* **Poké Doll** — its *only* function was fleeing a wild battle
  (`fieldUseFunc: ITEM_USE_FUNC_NONE`, `battlePocket: BATTLE_POCKET_MASK_...`
  battle-only), so with the battle-item block in place it could never do
  anything, ever, while still being genuinely obtainable (Veilstone Dept
  Store 1F Left, and a hidden item at Galactic HQ 2F). It's replaced with
  **Shed Shell** in both spots (`include/data/mart_items.h`,
  `include/data/field/hidden_items.h`) — a held item (`HOLD_EFFECT_SWITCH`,
  lets the holder always switch out) that works passively and isn't affected
  by the battle-item restriction at all. The shop copy is added to the
  one-purchase-ever list (see "Shop purchase limits" above); the hidden-item
  copy needs no extra limit since finding it is already a one-time pickup.
  Fluffy Tail (the same "flee battle" role) and the Red/Blue/Yellow Flutes
  (battle-only status-cure items whose `battleUseFunc` is already `0`/disabled
  in vanilla Platinum) were also checked and found to already be unobtainable
  anywhere in the game, so nothing needed doing for those.
* **Revive, Max Revive, and Revival Herb** — corrected from an earlier pass:
  these don't just get blocked in battle, they let you undo a Pokémon fainting
  at all (`itemUseParams.revive: true`), which a hardcore nuzlocke ruleset
  (fainted = dead, no exceptions) makes just as much off-limits as any
  battle-only item, regardless of whether they're used mid-battle or from the
  party screen afterward. Shops already had them removed (see "Revive, Max
  Revive, and Revival Herb removed from every shop" above); the 16 remaining
  hidden-item placements in `include/data/field/hidden_items.h` (Route 209,
  215, 207, 213, 217 ×2, 221, Floaroma Meadow ×2, Victory Road 1F, Mt. Coronet
  4F Rooms 1&2, Mt. Coronet 1F South, Mt. Coronet Outside North/South, Stark
  Mountain Room 2, and the Grand Lake/Route 213 Northeast House) are replaced
  with a mix of Full Restore, Max Potion, Hyper Potion, Full Heal, and Rare
  Candy — all of which fully heal a *living* Pokémon but, unlike the items
  they replace, can't cheat a faint.

### Silk Scarf removed from the Prize Corner
Now that the rival gives you one directly on Route 207 (see "Vs. Seeker
removed" above), Silk Scarf is no longer one of the Prize Corner's coin
prizes (`src/scrcmd_game_corner_prize.c`), and the prize count passed to the
list-menu builder (`VAR_MAP_LOCAL_0x01` in
`scripts_veilstone_city_prize_exchange.s`) was updated from 19 to 18 to match.

### Veilstone Dept Store 2F: X Items replaced with Heart Scale (10,000)
`VeilstoneDeptStoreStock_2F_UP` (`include/data/mart_items.h`) no longer sells
X Speed/Attack/Defense/Sp. Atk/Sp. Def/Accuracy, Guard Spec., or Dire Hit —
it's now a single Heart Scale listing, priced at 10,000 (`res/items/data/heart_scale.json`'s
`price` field, which also affects its sell-back price everywhere else, since
price is per-item, not per-shop). No shop in the game sells in-battle-only
stat-boost items anymore. The floor's two attendant NPCs who used to plug
Dire Hit / X Accuracy (`VeilstoneStore2F_Text_DireHitIsForYou` /
`_TreatItToXAccuracy` in `res/text/veilstone_store_2f.json`) now talk about
Heart Scales and the Pastoria move-relearner instead, so their dialogue
matches what's actually for sale.

### Revive, Max Revive, and Revival Herb removed from every shop
* `PokeMartCommonItems` (`include/data/mart_items.h`) — the badge-gated Revive
  entry shared by every regular town/city Mart is removed.
* `VeilstoneDeptStoreStock_1F_RIGHT` — its Revive entry is removed.
* `EternaHerbShopStock` — its Revival Herb entry is removed (Heal Powder,
  Energy Powder, and Energy Root are unaffected).
* Max Revive was not sold in any shop already, so nothing needed removing
  there. These items are still obtainable as fixed field items / found drops;
  only the ability to buy them changed.

### EV vitamins give 50 EV per use, capped at 4 uses per Pokémon
Protein, Iron, Calcium, Zinc, Carbos, and HP Up each now grant **50 EVs**
per use (`res/items/data/{protein,iron,calcium,zinc,carbos,hp_up}.json`'s
per-stat `*EVs` field, up from vanilla's 10). Since battling already grants no
EVs at all in this romhack (see "No EVs from battle" under Battle changes),
vitamins are now the *only* source of EVs, and two limits shape how far they
go:

* **Max 2 successful uses per stat** falls out for free from the existing
  100-EV-per-stat vitamin soft cap (`MAX_EV_VITAMIN` in
  `src/item_use_pokemon.c`) once each use gives 50 — 2×50 reaches the cap
  exactly, and a 3rd use on that stat has no effect (same "won't have any
  effect" message vanilla already shows at the cap), so no new code was
  needed for this part. This is separate from the shop's 10-per-type
  lifetime purchase cap above — that limits how many you can ever buy, not
  how many you can use on one Pokémon.
* **Max 4 successful uses per Pokémon, across all 6 vitamin types combined,**
  is new: `src/item_use_pokemon.c` repurposes the previously-fully-unused
  `MON_DATA_UNUSED_113` byte (Platinum has no Shiny Leaf feature, so this byte
  — normally used for that in HGSS — was dead weight) as a per-Pokémon
  counter. `Pokemon_CheckItemEffects` refuses to let any of the 6 vitamins be
  used once this counter reaches 4 (`MAX_VITAMIN_USES_PER_POKEMON`), and
  `Pokemon_ApplyItemEffects` increments it by 1 every time a vitamin actually
  changes a stat's EVs (not on a "no effect" use). Since this reuses an
  existing byte inside the encrypted Pokémon data blocks rather than growing
  the struct, there's no save-compatibility impact — every existing Pokémon's
  counter starts at 0.

### Link Cable — trade evolutions no longer require an actual trade
A new item, **Link Cable** (`ITEM_LINK_CABLE`, uses the Pal Pad icon), simulates
a link trade when used on a party Pokémon from the bag. Every trade evolution in
the game now happens via a used item instead of an actual trade:

* Plain trade evolutions — Kadabra, Machoke, Graveler, Haunter — had their
  evolution method changed from `EVO_TRADE` to `EVO_USE_ITEM` with
  `ITEM_LINK_CABLE` in `res/pokemon/{kadabra,machoke,graveler,haunter}/data.json`.
* Trade-while-holding-an-item evolutions had their method changed from
  `EVO_TRADE_WITH_HELD_ITEM` to `EVO_USE_ITEM` (same item, same target), so the
  existing hold item is now used directly instead of held through a trade:
  Onix (Metal Coat), Seadra (Dragon Scale), Scyther (Metal Coat), Porygon
  (Up-Grade), Porygon2 (Dubious Disc), Clamperl → Huntail (Deep Sea Tooth) /
  → Gorebyss (Deep Sea Scale), Poliwhirl (King's Rock), Slowpoke (King's Rock),
  Dusclops (Reaper Cloth), Electabuzz (Electirizer), Magmar (Magmarizer), Rhydon
  (Protector). Their `res/items/data/*.json` `fieldUseFunc` changed from
  `ITEM_USE_FUNC_NONE` to `ITEM_USE_FUNC_EVO_STONE` (the same generic,
  species-agnostic use-function evolution stones already use — it reads the
  species' evolution table for an `EVO_USE_ITEM` entry matching the item used, so
  no new code was needed) so they can now be used from the bag like a stone.
  Their old `HOLD_EFFECT_EVOLVE_*` hold effects are left in place but are now
  inert leftovers, since evolution no longer keys off holding the item.
* Both changes rely on the same evolution-table check
  (`src/pokemon.c` around `EVO_USE_ITEM`); the old `EVO_TRADE` /
  `EVO_TRADE_WITH_HELD_ITEM` cases in `src/pokemon.c` and `src/evolution.c` are
  untouched but no species reference them anymore, so trading no longer evolves
  anything in this romhack.

### PPHM — Portable Pokémon Healing Machine (`fd081bca0`)
A new Key Item (`ITEM_PPHM`, id 468).

* `generated/items.txt`, `include/constants/items.h`
  (`ITEM_USE_FUNC_PPHM`), `res/items/data/pphm.json` (uses the Poké Radar icon),
  `res/text/bag.json`.
* `src/item_use_functions.c` — using it from the bag or in the field calls
  `Party_HealAllMembers` (full HP, all status cleared, all PP restored), plays
  the healing jingle, and prints "Your Pokémon were restored to full health!".
* `res/field/scripts/scripts_common.s` — the first time Nurse Joy heals your
  party she also hands over the PPHM, gated by `FLAG_RECEIVED_PPHM` (renamed from
  `FLAG_UNUSED_0x0094`). The hand-off is written with primitive script commands,
  **not** `Common_GiveItemQuantity*`: those expand to a nested
  `CallCommonScript`, and the nurse dialogue is itself a common script running in
  the single sub-context slot, so nesting overwrites and leaks it and crashes
  right after the item is added.
* `res/text/common_strings.json` — new `CommonStrings_Text_PokecenterFirstVisitGift`.

### Repel Toggle
A new Key Item (`ITEM_REPEL_TOGGLE`, id 470) that keeps Repel running with no
step limit. Using it (from the Bag or as a registered item) flips it on/off and
pops up a message each time; while on, weak wild Pokémon never appear and the
"REPEL's effect wore off" prompt never fires. Its Bag description ends with the
current state — `(on)` or `(off)`.

* `generated/items.txt`, `include/constants/items.h` (`ITEM_USE_FUNC_REPEL_TOGGLE`),
  `res/items/data/repel_toggle.json` (reuses the Repel icon, `canRegister`).
* `generated/vars_flags.txt` — `FLAG_REPEL_TOGGLE_ON` (renamed from
  `FLAG_UNUSED_0x0095`) holds the on/off state.
* `src/item_use_functions.c` — `UseRepelToggleFromMenu` / `UseRepelToggleInField`
  toggle the flag, set the repel step counter to `0xFF` (on) or `0` (off), and
  print `Bag_Text_RepelToggleOn` / `..._Off` via `PrintRegisteredKeyItemUseMessage`
  (the same task PPHM uses).
* `src/overlay006/repel_step_update.c` — `Repel_UpdateSteps` tops the counter back
  up to `0xFF` and returns early (no decrement, no "wore off" script) whenever
  `FLAG_REPEL_TOGGLE_ON` is set.
* `src/applications/bag/windows.c` — `BagUI_PrintItemDescription` appends
  `Bag_Text_RepelToggleStateOn` / `..._Off` to this item's description.
* `res/text/bag.json` — the four new strings above.
* `res/field/scripts/scripts_route_202.s` + `res/text/route_202.json` — the
  professor's assistant (Dawn / Lucas) hands over the Repel Toggle in
  `Route202_GivePokeballs`, right after the catching tutorial and the five Poké
  Balls, saying *"Take this, I think it will be helpful for your journey."*
  (`Route202_Text_CounterpartHereTakeThis`).
* `res/field/scripts/scripts_common.s` — Nurse Joy hands it over as a fallback
  for saves that already finished the Route 202 tutorial
  (`VAR_ROUTE_202_STATE >= 1`) but don't have the item, checked on every heal in
  `CommonScript_NurseTryGiveFirstVisitGift` (inlined, ahead of the PPHM check).

### Poketch Manipulator
A new Key Item (`ITEM_POKETCH_MANIPULATOR`) that lets the player set the
current time-of-day period (Morning/Day/Night) independently of the DS's
real clock, with a 4th "Unset" option to go back to following the clock.
While a period is forced, it stays fixed — it does not drift into the next
period as real time passes — until the player picks a different option or
Unsets it.

* `generated/vars_flags.txt` — new `VAR_FORCED_TIME_OF_DAY` (0 = off/follow
  the clock, 1/2/3 = Morning/Day/Night), added by renaming the unused
  `VAR_UNUSED_0x40EA` slot in place, the same way `TRAINER_GRAVELER_GYM_PUZZLE`
  and `ITEM_REPEL_TOGGLE` reused an unused trainer/item slot rather than
  appending a new one. **This one actually matters for save compatibility,
  not just tidiness**: `NUM_VARS` is computed as `VARS_END - VARS_START`
  (no fixed-capacity ceiling to worry about, unlike the trainer-defeated-flags
  range), so simply appending a new line before `VARS_END` - which is what
  this fix originally did, and which built and ran fine on a fresh save -
  grows `sizeof(VarsFlags)` by 2 bytes. `gSaveTable` (`src/savedata/save_table.c`)
  packs every save block sequentially using each block's *current* size with
  no on-disk length/version record, so that 2-byte growth silently shifted
  the load offset of every save block after `SAVE_TABLE_ENTRY_VARS_FLAGS` -
  Poketch, field player state, **Pokédex**, Day Care, Pal Pad, PC boxes, and
  more - for any save file written before this change. This is exactly what
  caused an existing save to suddenly read as having "completed" the local
  Pokédex and trigger the vanilla Poké Radar tutorial scene on Route 202 out
  of nowhere - the Pokédex block was being read from the wrong offset.
  Renaming an existing unused slot instead keeps `sizeof(VarsFlags)`
  (and therefore every later block's offset) byte-for-byte identical to
  before this feature existed, which is why reusing a dummy/unused slot is
  the load-bearing convention for growing `trainers.txt`/`items.txt`/
  `vars_flags.txt` in this project, not just a style preference.
* `src/rtc.c` / `include/rtc.h` — the actual override. `TimeOfDayForHour(int hour)`
  is the single choke point every "what time is it" query in the game already
  goes through (`GetTimeOfDay`, `IsNight`, `FieldSystem_GetTimeOfDay`, and the
  battle-setup DTO's `timeOfDay` field all call it, directly or indirectly) -
  when `VAR_FORCED_TIME_OF_DAY` is non-zero, it returns the forced period
  immediately and ignores the real `hour` argument entirely, rather than
  merely biasing the normal lookup table. `GetSecondsSinceMidnight()` is
  similarly overridden to a representative "seconds since midnight" for the
  forced period (6am/1pm/10pm), since `AreaLightManager_UpdateActiveTemplate`
  (the map's ambient/directional lighting - `src/overlay005/area_light.c`)
  reads that function directly instead of going through `TimeOfDayForHour`,
  and would otherwise still light the map for the real time of day even with
  an override active. The override is read directly from the global
  `SaveData_Ptr()` rather than threaded through every caller as a new
  parameter (which would have meant touching over a dozen unrelated call
  sites across battle setup, evolutions, wild encounters, NPC schedules,
  and more) - this mirrors how `Pokemon_GetLevelCap`-style code elsewhere
  already reaches `SaveData_Ptr()` directly from deep gameplay code with no
  `FieldSystem` in scope.
* `include/constants/items.h` (`ITEM_USE_FUNC_POKETCH_MANIPULATOR`),
  `res/items/data/poketch_manipulator.json` (key item, `canRegister`, reuses
  the Vs. Recorder's icon rather than adding new art - fitting, since the
  Vs. Recorder is no longer given out anywhere either, see the Jubilife City
  Looker entry under Map data).
* `src/item_use_functions.c` — `UsePoketchManipulatorFromMenu`/`InField` open
  a new `TimeOfDayMenu_Task` (modeled closely on the existing
  `RegisteredItemsMenu_Task`): a small 4-choice window (Morning/Day/Night/
  Unset, B to cancel without changing anything) that calls
  `RTC_SetForcedTimeOfDay` on a choice and then prints a confirmation message
  via the same `PrintRegisteredKeyItemUseMessage` task PPHM/Repel Toggle use.
* `res/text/bag.json` — the 4 menu-choice strings and 4 confirmation
  messages (`Bag_Text_TimeOfDayMenu*`/`Bag_Text_TimeOfDaySet*`).
* `res/field/scripts/scripts_jubilife_city.s` + `res/text/jubilife_city.json` —
  the Pokétch Co-President hands it over right after the Pokétch itself,
  saying "Here's something that will allow you to set the time of day. Some
  guy with white shoes and a brown trenchcoat gave it to me. I don't know
  how it works." (`JubilifeCity_Text_PoketchManipulatorGift` - a nod to
  Looker, who was just added as a battle earlier in this same scene).

**Bug fix (crash on use):** `UsePoketchManipulatorFromMenu`'s `TimeOfDayMenu_New`
originally allocated its task struct from `HEAP_ID_FIELD1`, but that constructor
runs synchronously at Use-press time, before the `StartMenu` `NEW_TASK` state has
confirmed the field map is running and faded in - and `FIELD1` is the current
map's own working heap, which gets reset as part of that transition. The
allocation was getting invalidated out from under `menu->taskData` by the time
`TimeOfDayMenu_Task` actually ran, corrupting memory and blacking out the
screen the moment the item was used from the Bag. Every other `*FromMenu`
handler in `src/item_use_functions.c` (`UsePphmFromMenu`, `UseRepelToggleFromMenu`)
already allocated from `HEAP_ID_FIELD2` for this exact reason - `TimeOfDayMenu_New`
now does too.

**Bug fix (blank name in Bag):** the item's display name, "Poketch
Manipulator", is 19 characters - one over the Bag's fixed 18-character
per-item name buffer (`InitItemNameBuffers` in `src/applications/bag/main.c`,
used for every item in every pocket; the longest vanilla item name is 12
characters, e.g. "Premier Ball"). The string writer silently leaves the
buffer empty rather than truncate when the source doesn't fit, so the name
rendered blank in the Key Items pocket instead of the game just cutting it
off. Renamed the item to **VortexManipulator** (17 characters, safely under
the limit - the buffer needs one spare byte for the string terminator, so 17
is the true ceiling, not 18) in `res/items/data/poketch_manipulator.json`.
The internal identifiers (`ITEM_POKETCH_MANIPULATOR`,
`ITEM_USE_FUNC_POKETCH_MANIPULATOR`, the `poketch_manipulator.json` filename
itself) are unchanged - only the player-visible name and plural form moved.

### Revive → Rare Candy, Max Revive → Heart Scale
* `res/field/scripts/scripts_visible_items.s` — every field item ball that
  contained a Revive now gives a **Rare Candy**; every Max Revive ball gives a
  **Heart Scale**. (Wayward Cave, Mt. Coronet, Lost Tower, Iron Island, Stark
  Mountain, the Galactic buildings, Routes 207/209/212/213/216/225, etc. The
  internal `LOCALID_ITEM_REVIVE` / `FLAG_OBTAINED_*_REVIVE` names are left as-is;
  only the item handed out changed.)
* `res/field/scripts/scripts_jubilife_tv_1f.s` — the Jubilife TV lottery first
  prize is a Heart Scale instead of a Max Revive.
* `include/data/pickup.h` — the Pickup ability tables give Rare Candy where they
  gave Revive and Heart Scale where they gave Max Revive.

### PP Up is no longer given out
With stat-boosting moves capped at low PP, PP Up / PP Max are much less useful,
and the two places that handed them out for free now give **Heart Scales**:

* `include/data/pickup.h` — the `ITEM_PP_UP` slot in `sCommonPickupItems`
  is now `ITEM_HEART_SCALE` (so high-level Pickup can roll Heart Scale where it
  used to roll a PP Up).
* `res/field/scripts/scripts_jubilife_tv_1f.s` — the Jubilife TV lottery
  two-digit-match prize is a Heart Scale instead of a PP Up. (PP Max was never a
  lottery prize here.)

PP Ups can still be bought / found as fixed field items; only the random sources
changed.

### Move tutors teach for free
`src/overlay005/scrcmd_move_tutor.c` — `ScrCmd_CheckCanAffordMove` always reports
"can afford" and `ScrCmd_PayShardCost` deducts nothing, so the shard move tutors
(Route 212, Survival Area, Snowpoint, and any sharing `scripts_common.s`) teach
their moves without spending Red/Blue/Yellow/Green Shards. The "you don't have
enough shards" branch in those scripts is now unreachable. The Battle Frontier BP
tutors and the Heart Scale move reminder are unaffected.

### Vs. Seeker removed, along with trainer rematches and the Pokémon Center daily trainers

The Vs. Seeker (and the trainer-rematch system it exclusively powered) and the
Pokémon Center daily-rotation trainer minigame (Grace, Kinsey, Tevin, Lee, Roxy
& Oli, Ariel, Arturo) have both been removed. Investigation found that in this
codebase Vs. Seeker's "flag a defeated trainer to spin and re-battle" mechanic
*is* the only trainer-rematch system that exists — every trainer's 2nd-through-
6th fight (`gVsSeekerRematchData` in the old `vs_seeker.c`) was reachable only
through it, so removing the item removes every rematch battle in the game as a
side effect. The Pokémon Center trainers are a separate, self-contained walk-up
NPC minigame that never needed the item or its mechanic — it only checked story
flags that happened to be named after Vs. Seeker's unlock levels — but the user
asked for it gone too.

**Vs. Seeker item**: kept as a reserved-but-inert item ID rather than deleted
outright, since deleting `ITEM_VS_SEEKER` from `generated/items.txt` would
renumber every item after it and corrupt item IDs already stored in existing
saves' Bag/PC. `res/items/data/vs_seeker.json` now has `fieldUseFunc:
ITEM_USE_FUNC_NONE`, `canRegister: false`, `preventToss: false`, and a
description noting it "no longer serves any purpose"; it can no longer be
obtained (see Route 207 below) or used. `include/constants/items.h`'s
`ITEM_USE_FUNC_VS_SEEKER` slot and its three functions in
`src/item_use_functions.c` are removed since nothing points at them anymore.

**Mechanic removed entirely**: `src/overlay005/vs_seeker.c` /
`include/overlay005/vs_seeker.h` / `include/constants/vs_seeker.h` deleted
(and `src/meson.build`'s reference to the `.c` file removed). Everywhere it was
wired into shared/generic code got the Vs.-Seeker-specific piece stripped back
out, with the generic behavior left intact:

* `src/overlay005/ov5_021DFB54.c` / `.h` — the player's Vs.-Seeker-pose redraw
  function and its slot in `sPlayerAvatarRequestStateTbl` (now 9 entries, was
  10 — it was the last slot, so nothing else shifted) removed;
  `FieldSystem_StartVsSeekerTask` / `EndVsSeekerTask` (the only user of the
  `PLAYER_TRANSITION_x0200` bit) removed.
* `src/overlay005/ov5_021FAF40.c` — the 6 dedicated graphics-lookup rows and the
  `BILLBOARD_FRAME_SEQ_VS_SEEKER` enum value (last entry, safe to drop) removed;
  `PLAYER_AVATAR_VS_SEEKER` removed from `include/constants/player_avatar.h` and
  its two `src/player_avatar.c` switch cases. The now-unreferenced graphics/sound
  assets (`player_{f,m}_vs_seeker.png`, `vs_seeker.bin`, the Vs. Seeker SFX) are
  left as inert packed-but-unused data rather than touched, since their `.order`/
  `.naix` files are themselves position-significant.
* `src/system_vars.c` / `.h`, `src/system_flags.c` / `.h` — the battery/step-count
  vars and the `VsSeekerUsed`/`UnlockedVsSeekerLevel` flag helpers removed (the
  underlying `VAR_VS_SEEKER_*` / `FLAG_VS_SEEKER_USED` / `FLAG_UNLOCKED_VS_SEEKER_LVL_1-5`
  slots in `generated/vars_flags.txt` are left in place, unused, for the same
  save-numbering reason as the item).
* `src/field_map_change.c`, `src/overlay005/field_control.c` — the per-map-change
  reset and the per-step battery/rematch-timeout update call removed.
* `src/scrcmd.c` / `include/data/scripts/scrcmd.h` / `asm/macros/scrcmd.inc` —
  `GetRematchTrainerID` and `StartVsSeeker` are gone as script commands; since
  this project avoids renumbering the `SCRCMD_*` table (it's positional), their
  two slots became inert `SCRCMD_UNUSED_VSSEEKER_1/2` stubs (`ScrCmd_Unused_VsSeeker1/2`,
  matching the existing `ScrCmd_Unused_09C`-style pattern already used elsewhere
  in this file) instead of being deleted outright. `SetMoveCodeForFacingDirection`
  is **kept** — it's called after every trainer battle in the game, not just
  rematches — but its body (turning the trainer, and their double-battle partner,
  to face the player) was pulled out of `vs_seeker.c` and rewritten in place as
  static helpers in `scrcmd.c` (`SetTrainerMoveCodeForFacingDirection`,
  `GetDoubleBattlePartnerTrainer`) with no Vs.-Seeker dependency.
* `res/field/scripts/scripts_vs_seeker.s`, `res/text/vs_seeker.json` — the
  item's own use-script is now an inert one-`End` stub and its text bank has zero
  messages (both files, and their `TEXT_BANK_VS_SEEKER` / script-ID-offset
  registrations, had to stay *present* rather than be deleted, since
  `res/text/meson.build` and `res/field/scripts/scripts.order` both require
  every declared bank/script file to physically exist).
* `res/field/scripts/scripts_battles.s` — `Battles_Trainer` no longer routes a
  defeated trainer through `Battles_TryRematch`/`GetRematchTrainerID`; it goes
  straight to `Battles_PostBattleMessage` (the ordinary "we already fought"
  line), so no trainer in the game can be re-battled anymore. The now-unreachable
  `Battles_TryRematch`, `Battles_Rematch`, `Battles_StartRematchEncounter` labels
  are deleted. The ~240 base trainers' `*_REMATCH_1..5` data entries
  (`res/trainers/data/*_rematch_*.json`, `generated/trainers.txt`) are left in
  place as inert, unreferenced data rather than deleted, for the same ordinal-
  numbering reason as the item.
* `res/field/scripts/scripts_route_207.s` + `res/text/route_207.json` — Dawn/
  Lucas's coin-flip scene no longer gives the Vs. Seeker or sets
  `FLAG_UNLOCKED_VS_SEEKER_LVL_1`; the Dowsing Machine Poketch app gift is
  unaffected. In the Vs. Seeker's old spot, the rival now hands over a
  **Silk Scarf** (`Route207_GiveSilkScarf`, same `Common_GiveItemQuantity`
  pattern as the old item grant) with the line *"Here, take this. It gets cold
  in these caves!"*. The two "you want the Vs. Seeker" / "have this too"
  message slots were reworded in place (same array positions, per this
  project's append-only text-bank convention) into the new Silk Scarf line
  and the Pokétch-gift line, respectively.
* `res/field/scripts/scripts_celestic_town.s`, `scripts_spear_pillar.s`,
  `scripts_pokemon_league_hall_of_fame.s`, `scripts_stark_mountain_room_3.s` —
  the four other `SetFlag FLAG_UNLOCKED_VS_SEEKER_LVL_2/3/4/5` story-milestone
  lines removed, since nothing reads those flags anymore (Stark Mountain's
  dedicated `StarkMountainRoom3_UnlockVSSeekerLvl5` label/call is deleted
  outright, being otherwise-unreachable dead code).
* `res/field/scripts/scripts_hearthome_city.s` + `res/text/hearthome_city.json`
  — the Black Belt NPC's one Vs.-Seeker flavor line was reworded in place
  (renamed `HearthomeCity_Text_BlackBelt2Flavor`, same slot) to drop the item
  reference while keeping the rest of his anecdote.

**Pokémon Center daily trainers removed**:

* `src/script_manager.c` / `include/script_manager.h` — the
  `scripts_pokemon_center_daily_trainers` script-ID-offset registration removed
  (confirmed hand-assigned/non-sequential, safe to delete outright).
* `res/field/scripts/scripts_init_new_game.s` — now sets
  `FLAG_HIDE_POKECENTER_DAILY_TRAINER_1` / `_2` on a fresh save (mirroring the
  existing `FLAG_HIDE_ROUTE_207_COUNTERPART` pattern), since the deleted
  minigame script was the *only* place that ever set those hide flags; without
  this, the placeholder NPC objects in all 14 Pokémon Centers would spawn
  visible with no working script behind them.
* 14 maps' `scripts_init_<city>_pokecenter_1f.s` — the
  `InitScriptEntry_OnTransition` call into the now-gone daily-trainers script
  removed from each.
* `res/field/scripts/scripts_pokemon_center_daily_trainers.s` is now an inert
  one-`End` stub rather than deleted, and its text bank
  (`res/text/pokemon_center_daily_trainers.json` / `TEXT_BANK_POKEMON_CENTER_DAILY_TRAINERS`)
  and the 7 trainers' `res/trainers/data/*.json` entries are left in place as
  unreferenced data — both `scripts.order` (physical NARC packing order) and
  `generated/trainers.txt` are ordinal lists where deleting an entry outright
  breaks the build or risks renumbering, matching the precedent set everywhere
  else in this change.
* The 7 NPCs' object-event placements in each Pokémon Center's `events_*.json`
  are untouched: their visibility already keys off the same
  `FLAG_HIDE_POKECENTER_DAILY_TRAINER_1/2` flags, which now stay permanently set,
  so they stay permanently hidden and non-interactive without needing any
  per-map object edits.
* Not retroactive: a save that had already unlocked the daily trainers before
  this change (i.e. already cleared the hide flags) keeps them visible, since
  there's no flag-migration mechanism in this codebase for any one-time story
  flag. A fresh save is unaffected.

### Gift Pokémon and eggs go to the PC when the party is full
Instead of being turned away ("come back when you have room"), a gift Pokémon or
gifted egg whose party is full is deposited straight into the first PC box with a
free slot.

* `src/unk_02054884.c` — new `Pokemon_AddToPartyOrBox` (adds to party, else to a
  PC box, else returns `GIVE_MON_RESULT_NO_ROOM` without consuming anything).
  `Pokemon_GiveMonFromScript` uses it and now returns one of
  `GIVE_MON_RESULT_TO_PARTY` / `_TO_BOX` / `_NO_ROOM`
  (`include/constants/pokemon.h`); return type changed from `BOOL` to `int`
  (`include/unk_02054884.h`).
* `res/field/scripts/scripts_hearthome_city_northwest_house.s` (Bebe's Eevee),
  `scripts_veilstone_city_northeast_house.s` (the Porygon),
  `scripts_mining_museum.s` (revived Fossils) — the up-front "party full" guard
  is removed; after `GivePokemon` the script branches on the result, showing a
  new "sent to a Box in your PC" message and skipping the nickname prompt when
  the Pokémon was boxed. The old party-full message is still shown in the
  can't-fit-anywhere case (party and all 18 boxes full); the Mining Museum keeps
  `VAR_REVIVED_POKEMON_SPECIES` set for later pickup only in that case.
* Eggs: the `GiveEgg` and `GiveEggFromDaycare` script commands
  (`asm/macros/scrcmd.inc`, `src/scrcmd_party.c`, `src/scrcmd_daycare.c`) take a
  result var and route a full party to a box the same way.
  `Daycare_GiveEggFromDaycare` (`src/overlay005/daycare.c`) now takes `SaveData`,
  returns the result, and only clears the Day Care's "egg ready" state once the
  egg has actually landed somewhere. The three egg scripts —
  `scripts_eterna_city.s` (Cynthia's Togepi), `scripts_iron_island_b2f_left_room.s`
  (Riley's Riolu), `scripts_day_care_common.s` — keep a party check but fall
  through to the hand-off whenever a box has room, show a "sent to a Box"
  message, and only fall back to the "come back later" retry when the party and
  every box are full. A boxed egg does not accumulate steps toward hatching until
  it is moved into the party, so each message says so. New strings in the
  matching `res/text/*.json` files.
* Not changed: Mystery Gift has its own separate delivery path.

### Trainer location doc reordered into actual route-progression order, several locations moved tiers, and a new wild-encounters doc
`res/trainers/TRAINER_LOCATIONS.md` previously grouped every pre-Elite-Four
trainer by level-cap tier, but sorted locations *alphabetically* within each
tier rather than in the order the player actually reaches them. It's now
sorted in route-progression order end to end, and is the explicit source of
truth for that ordering going forward - `TRAINER_ENCYCLOPEDIA.md` is
regenerated from it (see below) rather than hand-maintained in parallel.

Reordering surfaced several locations whose *tier* didn't match where they
now sit in the route order, so their trainers were releveled to the new
tier's convention (regular trainer = cap−4, Galactic grunt = cap−3, boss/rival
= cap−2) at the same time:

* **Fuego Ironworks** (`worker_conrad/dillan/holden.json`): Tier 2 → Tier 6,
  Lv. 19 → Lv. 47.
* **Route 211** East + West (8 files): Tier 3 → Tier 2, Lv. 26 → Lv. 19 -
  confirmed reachable before beating Gardenia (Cut is available at Eterna
  City itself, before the gym; no other gate on that path).
* **Celestic Town** + **Celestic Town Cave** (grunt + Cyrus): Tier 3 → Tier 6,
  Lv. 27 → Lv. 48 and Lv. 28 → Lv. 49.
* **Route 208** (7 files): Tier 4 → Tier 3, Lv. 33 → Lv. 26.
* **Route 215** (8 files): Tier 5 → Tier 4, Lv. 41 → Lv. 33.
* **Route 220** (7 files) and **Route 221** (6 files): Tier 8 → Tier 6,
  Lv. 58 → Lv. 47.
* **Route 223** (13 files): Tier 8 → Tier 9, Lv. 58 → Lv. 63.
* **Cafe**, **Route 209** (+ its Gate/Lost Tower), **Route 210 South**, and
  **Solaceon Ruins Room 3** (30 files total): Tier 3 → Tier 4, Lv. 26 → Lv. 33
  (rival: Lv. 28 → Lv. 35) - these move as a direct consequence of the above,
  not a separate call.
* **Route 212** North + South (20 files) and **Pokemon Mansion** (7 files):
  Tier 4 → Tier 5, Lv. 33 → Lv. 41 - both are now gated on defeating Maylene
  (see "Map data" below for Route 212; Route 214 got the same new gate), so
  Tier 5 is the earliest either is actually reachable.
* Mt Coronet's interior is intentionally left as one Tier 3 block for now -
  not resolved whether/how it should split across an earlier/later tier pair.
  Lake Verity's and Lake Valor's late-game Team Galactic invasion trainers,
  Route 210 North, and Valor Cavern are also unchanged (kept at their
  existing tier; the route-order pass's "Lake verity"/"Lake valor" mentions
  in earlier tiers are about the wild-encounters doc's early/undrained visit,
  not these trainers). Route 218 and 219 also keep their existing (later)
  trainer tiers even though the route-order pass lists them in Tier 1 for the
  wild-encounters doc (fishing rods aren't level-gated by story progression
  the way reaching the map on foot is).
* A bug from this pass: the whole "Route 218" section (4 trainers) was
  dropped by mistake during the rewrite and had to be added back.

`TRAINER_ENCYCLOPEDIA.md` was regenerated from the reordered
`TRAINER_LOCATIONS.md` plus the trainer data files, rather than hand-edited,
using a one-off script (not checked into the repo) that resolves each
trainer's class, ability (falling back to the species' first ability slot
when unset), nature, held item, and move names from `generated/*.txt` +
`res/text/*.json` + `res/pokemon/*/data.json` + `res/moves/*/data.json`.
Cross-checking its output against the previous hand-built version surfaced a
few genuine formatting bugs in the regenerator (worth noting since it'll
presumably be regenerated again by hand next time this file goes stale):
the in-game text bank for a trainer's class is written for terse in-battle
intro text, not encyclopedia clarity, so `TRAINER_CLASS_RIVAL`,
`TRAINER_CLASS_GALACTIC_GRUNT_MALE/FEMALE`, and the `DP_PLAYER_MALE/FEMALE`
"opposite-gender rival cameo" classes all needed their display name
hand-overridden rather than looked up directly; Burmy/Wormadam/Shellos also
need their form baked into the display name by hand, since nothing else
prints it. Everything else (moves, items, abilities, natures, other classes)
matched the previous version exactly once level differences are accounted
for - a couple of remaining cosmetic mismatches ("Mime Jr." vs "Mime jr.",
"Wake-Up Slap" vs "Wake Up Slap") are the new version being more consistent,
not a regression, and one ("X Defend" vs "X Defense") is the new version
correctly picking up an unrelated item rename the old, stale doc had missed.

New: `WILD_ENCOUNTERS.md` at the repo root, covering every wild-encounter
table in `res/field/encounters/` (grass/land, Surf, Rock Smash, Old/Good/Super
Rod, swarms, and Poké Radar chains), grouped in the same route-progression
order as `TRAINER_LOCATIONS.md`. Unlike the trainer doc, Route 218/219 and all
of Mt Coronet's interior are placed by when the area/water is actually
reachable rather than by trainer tier, per the notes above. Locations with no
wild-encounter table (towns, building interiors, Pal Park, legendary-only
chambers, etc.) are omitted, and postgame-only areas are deferred to a
"Tier 10 - tbd later" placeholder, matching `TRAINER_LOCATIONS.md`'s own
scope.

---

## Map data

### Jubilife City — Looker now battles you right after his reveal, and gives up the Vs. Recorder speech
Previously, after "unmasking" Looker (the shady man following Dawn/Lucas
around) and hearing his full introduction (through the "Don't be a thief!"
Q&A, his lecture about Sinnoh's Pokémon thieves, and giving the player a
Vs. Recorder), he'd simply walk off. He now challenges the player to a
one-on-one Pokémon battle immediately after introducing himself, before any
of that.

* `res/text/jubilife_city.json` — `JubilifeCity_Text_IsSayingFamiliarToYou`
  is truncated right after "My name... Ah, no, I shall inform you only of
  my code name." and the actual reveal ("My code name, it is Looker. It is
  what they all call me.") is replaced with a challenge instead: "I will
  tell it to you if you can beat me in a Pokémon battle!" - his codename
  is only actually spoken once, in the post-battle message below, dropping
  the "Don't be a thief!" question that used to follow it. The
  `TakingFromOthersIsWrong`/`YouClaimToNotKnowIt` Yes/No
  responses, the Sinnoh-thieves lecture (`PerhapsYouCanUseThis`), the
  Vs. Recorder flavor text (`DeviceForRecordingAMatch`), and the "inform me
  of any happenings" speech (`InformMeOfAnyHappenings`) are all removed,
  replaced by one new message (`LookerPostBattleIntroduction`) said after the
  battle: "It seems like you're a strong trainer. You could be a valuable
  ally to the international police (that's me). If you see anything
  suspicious, call for codename Looker, and I'll come running. Gotta go,
  there are bad guys to catch!"
* `res/field/scripts/scripts_jubilife_city.s` — `JubilifeCity_IsSayingFamiliarToYou`
  now closes the message and starts a trainer battle right away
  (`StartTrainerBattle TRAINER_LOOKER_JUBILIFE_CITY`) instead of branching on
  a Yes/No menu; losing blacks out and ends the scene the same way the
  Celestic Town Cave Galactic grunt fight does (`CheckWonBattle`/
  `BlackOutFromBattle`). `JubilifeCity_LookerGiveVSRecorderAndLeave` no longer
  gives the player a Vs. Recorder (`Common_GiveItemQuantity` call removed
  along with it) - it now only prints the one new post-battle message before
  Looker's existing walk-away/leave staging.
* `generated/trainers.txt` — added `TRAINER_LOOKER_JUBILIFE_CITY` by
  repurposing the unused `TRAINER_DUMMY_006` slot in place (same approach as
  `TRAINER_GRAVELER_GYM_PUZZLE` for Roark's gym puzzle), rather than growing
  the trainer roster/defeated-flag range - `res/trainers/data/dummy_006.json`
  (an orphaned, never-placed vanilla debug trainer, "Mickey" the Camper) is
  replaced by `res/trainers/data/looker_jubilife_city.json`: a single level
  10 Hoothoot, `TRAINER_CLASS_POLICEMAN` (there's no dedicated Looker trainer
  class/battle sprite in this game, so this is the closest thematic stand-in
  without needing new battle-sprite assets).
* `res/trainers/TRAINER_LOCATIONS.md` and `TRAINER_ENCYCLOPEDIA.md` updated
  to include this new trainer under Tier 1 - Jubilife City.

### Route 212 and Veilstone City → Route 214 now open on defeating Maylene
Route 212 (the Hearthome↔Pastoria connector) used to open unconditionally the
moment the player first set foot in Pastoria City, by any path, with no badge
check at all (`FLAG_HIDE_ROUTE_212_BLOCKADE` was set in
`PastoriaCity_OnTransition`) - not actually gated on Fantina as it might have
appeared from normal play order. It's now gated on Maylene's defeat instead,
alongside a brand new gate: Veilstone City's south exit onto Route 214, which
was previously completely unrestricted.

* `res/field/scripts/scripts_pastoria_city.s` - `PastoriaCity_OnTransition` no
  longer sets `FLAG_HIDE_ROUTE_212_BLOCKADE`.
* `res/field/scripts/scripts_veilstone_city_gym.s` - `VeilstoneGym_Maylene`
  now sets both `FLAG_HIDE_ROUTE_212_BLOCKADE` and the new
  `FLAG_HIDE_VEILSTONE_CITY_ROUTE_214_BLOCKADE` right after awarding the
  Cobble Badge, mirroring the existing Fantina/Route 209 gate pattern
  (`HearthomeGym_FantinaMain`).
* `generated/vars_flags.txt` - added `FLAG_HIDE_VEILSTONE_CITY_ROUTE_214_BLOCKADE`
  by renaming the unused `FLAG_UNUSED_0x03C3` slot in place rather than
  appending (save-compatibility convention - see `phmode-vars-flags-save-compat`).
* `res/field/events/events_route_214_gate_to_veilstone_city.json` - added a
  new `LOCALID_POLICEMAN` object standing on the walkable tile between the
  two warps, hidden by the new flag once it's set.
* `res/field/scripts/scripts_route_214_gate_to_veilstone_city.s` /
  `res/text/route_214_gate_to_veilstone_city.json` - the Policeman's line:
  "Sorry, this way is closed off for now. Beat the Gym Leader here in
  Veilstone and we'll let you through."

### Route 221 House — "Expert M" gives all 3 items at once, one time only
This NPC used to run a daily random-level guessing minigame (`GetDailyRandomLevel`
— show me a party Pokémon at today's number and I'll reward you), cycling
through Black Belt → Expert Belt → Focus Sash → repeat, once per day forever
(`FLAG_DAILY_RECEIVED_ROUTE_221_HOUSE_REWARD`). That whole daily/guessing
mechanic is removed: talking to him now immediately gives all 3 items in one
visit, regardless of the day, and never again after that.

* `res/field/scripts/scripts_route_221_house.s` — `Route221House_ExpertM` no
  longer calls `GetDailyRandomLevel`/checks any party Pokémon's level; it
  loops giving Black Belt, Expert Belt, then Focus Sash via
  `Route221House_GiveNextReward`. The existing `VAR_ROUTE_221_HOUSE_REWARD_INDEX`
  (previously "which of the 3 is due next in the endless rotation") is reused
  as-is for its natural new meaning — "how many of the 3 have been received
  so far" — stopping at 3 instead of wrapping back to 0, so no new var was
  needed. If the bag fills up partway through, progress is preserved and the
  next visit resumes with the remaining item(s) rather than re-giving ones
  already received. `FLAG_DAILY_RECEIVED_ROUTE_221_HOUSE_REWARD` and
  `FLAG_COULD_NOT_RECEIVE_ROUTE_221_HOUSE_REWARD` are no longer referenced
  anywhere and left inert.
* `res/text/route_221_house.json` — `Route221House_Text_ShowThisLevelPokemon`
  is repurposed in place as `Route221House_Text_HereAreAllThree` (the new
  one-time greeting), and `Route221House_Text_ComeAgainTomorrow` as
  `Route221House_Text_AlreadyGaveReward` (shown on any later visit).
  `Route221House_Text_ThankWithItem` is reworded from a per-item thank-you
  into the generic "let me give you the rest of what I owe you" line shown
  only when resuming after a bag-full interruption.
  `Route221House_Text_PokemonIsCorrectLevel` is now unused and left as dead
  data. The wall sign (`Route221House_Text_WinItemsFromMe`, listing all 3
  items) already matched the new behavior and didn't need changing.

### Oreburgh Gym — quiz-rock puzzle blocking the trainer-free path
Two new rock objects (`LOCALID_PUZZLE_ROCK_1`/`_2`, `res/field/events/events_oreburgh_city_gym.json`,
using the `OBJ_EVENT_GFX_ROCK_SMASH` graphic so they look like ordinary
smashable rocks), positioned from reference screenshots of the gym's actual
layout: Rock 1 at (2, 21), the bottom of the staircase in the lower-left
(near the wooden-bridge crossing); Rock 2 at (9, 12), just below the
staircase on the middle-right (near Darius). Went through two rounds of
in-game-screenshot corrections to get here (both were one tile too far left
at first — Rock 1 hadn't even rendered, since it was inside the wall — then
Rock 1 needed 2 more left and 1 down, and Rock 2 needed 2 more down).
**Placement note:** this repo doesn't decompile compiled map tile/collision
data to text, so these coordinates are read off reference images rather than
the map's own data.

* Pressing A on either rock does **not** trigger the normal Rock Smash flow —
  each has its own script (`res/field/scripts/scripts_oreburgh_city_gym.s`,
  `OreburghGym_PuzzleRock1`/`_2`, added as script indices 4/5 in the map's
  `ScriptEntry` table) that asks a multiple-choice question instead
  (`InitGlobalTextMenu ..., FALSE` — the `FALSE` disables canceling out with
  B, so the question must be answered):
  * Rock 1: *"What item did your rival give you on Route 207?"* — Hard Stone
    / Dusk Balls / Rare Candies / **Silk Scarf** (correct — matches the Route
    207 rival-gift change earlier in this document).
  * Rock 2: *"What item did your rival give you on Route 202?"* — Rare
    Candies / TM27 / Great Balls / **Repel Toggle** (correct — matches the
    existing Repel Toggle gift on Route 202).
* **Correct answer**: the message "Correct! The rock crumbles away." plays and
  `RemoveObject VAR_LAST_TALKED` permanently removes that rock (same
  mechanism ordinary Rock Smash rocks use to stay gone), opening the path.
* **Wrong answer**: "Wrong! The rock shakes, and a wild GRAVELER bursts out to
  attack you!" plays, then a Level 25 Graveler with **exactly** Magnitude and
  Rock Tomb attacks — no other moves. The rock is *not* removed, so a wrong
  answer can be retried. Losing blacks out like any other battle.
  * Platinum's wild-encounter engine has no way to force an exact moveset on
    a genuinely wild-spawned Pokémon (`StartWildBattle`/`StartLegendaryBattle`
    always derive moves from the natural level-up learnset) — the only way to
    guarantee moves is a trainer-battle-shaped encounter with an explicit
    `moves` array. This Graveler is implemented as `StartTrainerBattle
    TRAINER_GRAVELER_GYM_PUZZLE` (`res/trainers/data/graveler_gym_puzzle.json`,
    `TRAINER_CLASS_HIKER`, no items, no dialogue) rather than a true wild
    encounter. **Caveat**: I couldn't confirm from source whether the battle
    transition/intro looks any different from a wild encounter (a "Trainer
    wants to fight" framing vs. a wild cut-in) — worth a quick in-game check.
  * This repurposes the previously-unused `TRAINER_DUMMY_005` roster slot
    (confirmed zero references anywhere in the game before this change)
    rather than appending a new trainer: `generated/trainers.txt` is capped at
    exactly one save flag per trainer with **zero spare capacity** (929
    trainers defined vs. 928 available trainer-defeated flags fails the
    build outright), so growing the roster would have meant expanding that
    flag range — a save-data change — for one puzzle encounter. Renaming an
    already-dead slot in place avoids that entirely.
* The gym's front-desk guide (`OreburghGym_GymGuide`,
  `res/text/oreburgh_city_gym.json`) now warns before the fight: *"Roark uses
  a lot of speed-lowering moves and likes to set up Stealth Rock, too. The
  rocks in this gym will ask you questions. Pay attention when you interact
  with them!"* — added to his existing pre-badge advice, same message slot.

### Verity Lakefront — tall-grass patch

`res/field/maps/data/map_data_004.bin` — the south-east quadrant of Verity
Lakefront (matrix `map_matrix_000`, cell row 26 / col 2; neighbours
`map_data_001`/`002`/`003`).

* The 3D map model was re-exported (via DSPRE) with a patch of tall grass added
  to the clearing east of the hop-west ledge, a few tiles south of the Lake
  Verity entrance. The re-export also drops this quadrant's unused `puddle` and
  `nhana` (flower) materials — the NW-corner tiles keep their
  `TILE_BEHAVIOR_PUDDLE` behaviour (splash SFX) but no longer render as a puddle;
  that spot is not reachable by the player. Textures still resolve against the
  existing `map_texture_set_006` (it already contains the `nectgr` grass
  texture).
* BDHC (ground-height collision): the DSPRE/model-tool export produced a
  degenerate table — one plate whose bounding box covered only a 2×2-tile
  corner instead of the whole map, so most tiles had no height data (which
  broke object elevation, e.g. the Trainer Tips signpost stopped blocking).
  The map is flat and at the vanilla height, so the **vanilla BDHC** (one plate
  over the full map, flat, `constant -16`) is grafted back onto the export.
* Terrain attributes: local tiles X 21–24, Z 14–18 (a 4×5 block) set to
  `TILE_BEHAVIOR_TALL_GRASS` and left walkable. No collision bits and no
  map-edge tiles were changed, so the quadrant still stitches to its neighbours.

### Verity Lakefront — dedicated encounter table

* New `res/field/encounters/encounters_verity_lakefront.json`, appended to
  `pl_enc_data_srcs` in `res/field/encounters/meson.build` and to
  `res/field/encounters/encounters.order` (appended, so no existing archive
  index shifts). It gets its own `encounters_verity_lakefront` archive ID.
* `MAP_HEADER_VERITY_LAKEFRONT.wildEncountersArchiveID` now points at
  `encounters_verity_lakefront` instead of borrowing `encounters_route_201`, so
  the location's spawns can be tuned without touching Route 201.
* `tools/scripts/make_pokedex_enc_platinum.py` — `file_2` (the Pokédex
  distribution-map field-cell table) gets a new appended entry (index 51) for
  Verity Lakefront, at Pokédex-map cell (x 27, y 3–4), between Twinleaf and
  Route 201. `encounters_verity_lakefront.json`'s `map_category.map_number` is
  set to 51 so the Pokédex "where does it live" map highlights the lakefront
  itself rather than Route 201's cell. (`map_number` only drives that map screen;
  it has no effect on which species actually spawn.)
* The table's contents are currently a straight copy of the Route 201 roster,
  which is itself placeholder/test data — see `TEST_README.md`.

### Rock Smash wild encounters (new mechanic)

Vanilla Platinum's Rock Smash rocks never generate a wild encounter — this adds
one, Gen-3-style: each rock you smash has a 50% chance to start a battle, rolled
from a dedicated 5-slot table (20% each) separate from the map's land/water
tables.

* `include/overlay006/wild_encounters.h` / `src/overlay006/wild_encounters.c` —
  the `WildEncounters` struct's old `unused` field (a `pad(44)` placeholder
  between `surfEncounters` and the rod tables) is repurposed as
  `rockSmashEncounters` (a `WaterEncounters`-shaped table). This is ROM asset
  data, not a save-file field, so there's no save-compatibility concern.
  `WildEncounters_TryRockSmashEncounter(fieldSystem, speciesOut, levelOut)` rolls
  the 50% rate, then a uniform slot via new `GetRockSmashEncounterSlot()`
  (`LCRNG_RandMod(MAX_WATER_ENCOUNTERS)` — the existing water/rod slot pickers
  are all weighted, so Rock Smash needed its own uniform picker for the "20%
  each" spec); it also honours an active Repel the same way
  `WildEncounters_TryWildEncounter` does.
* New script command `TryRockSmashEncounter successVar, speciesVar, levelVar`
  (`SCRCMD_TRYROCKSMASHENCOUNTER` in `include/data/scripts/scrcmd.h`, macro in
  `asm/macros/scrcmd.inc`, `ScrCmd_TryRockSmashEncounter` in `src/scrcmd.c`) —
  wraps the C function above for scripts. On success, it hands off to the
  pre-existing `StartWildBattle species, level` command (the same primitive
  static/scripted encounters use), so full IV/shiny/nature generation runs
  through the normal pipeline.
* `res/field/scripts/scripts_field_moves.s` — both `FieldMoves_UseRockSmash*`
  flows (from field and from the field-move menu) now call
  `TryRockSmashEncounter` after the smash animation and, on success, start and
  resolve the battle (loss blacks out, same as the honey-tree win/loss pattern).
* `tools/jsoncnv/encounter.py` — packs new `rock_smash_rate` / `rock_smash_encounters`
  JSON keys into the repurposed field; maps without the keys default to rate 0
  with 5 empty slots (the same all-zero bytes the old `pad(44)` wrote), so every
  existing encounter JSON keeps packing identically.
* Real tables (5× Geodude placeholder, `rock_smash_rate: 50`) were added to
  every map with a Rock Smash rock — 44 in total. Level ranges match each
  map's existing land-encounter spread; see `TEST_README.md` for the
  placeholder-species note.
  * 11 `MAP_TYPE_OUTDOORS` maps: Route 208, Route 210 North, Route 211 East,
    Route 211 West, Route 213, Route 214, Route 222, Route 228, Route 230,
    Mt. Coronet Outside North, Mt. Coronet Outside South.
  * 33 `MAP_TYPE_CAVE` interiors: Mt. Coronet 1F North Room 1, 1F South, 1F
    Tunnel Room, 4F Rooms 1 and 2, B1F; Oreburgh Gate 1F/B1F; Oreburgh Mine
    B2F; Ravaged Path (visually a path but coded as a cave); Snowpoint Temple
    B2F; Stark Mountain Rooms 1–3; Turnback Cave Pillar 1 Rooms 1–6, Pillar 2
    Rooms 1–6, Pillar 3 Rooms 1–6; Victory Road 2F; Wayward Cave 1F.
* Two of those cave rooms had no wild-encounter archive wired up at all
  (`.wildEncountersArchiveID = ENCOUNTERS_NONE`), so a plain JSON edit wasn't
  possible:
  * **Stark Mountain Room 3** truly had no encounter data — a brand-new,
    otherwise-all-zero `encounters_stark_mountain_room_3.json` was added
    (appended to `encounters.order` and `pl_enc_data_srcs` in
    `res/field/encounters/meson.build`, so no existing archive index shifts),
    and the map header now points at it. `map_category` reuses Stark
    Mountain's existing dungeon number (9).
  * **Turnback Cave Pillar 3 Room 6** (the antechamber right before Giratina)
    turned out to already have a fully-authored, git-tracked ambient
    encounter table identical in shape to its 5 sibling rooms
    (Haunter/Bronzong/Golbat/Chimecho/Dusclops) — it was simply never wired
    up. Per explicit direction, that ambient table is left disabled
    (`land_rate` forced to 0, matching current/vanilla behavior) and only
    Rock Smash was turned on for this room, so nothing new spawns there from
    walking around.

### Map headers

`include/data/map_headers.h`:

* `MAP_HEADER_POKEMON_LEAGUE_AARON_ROOM` — `.weather` set to `OVERWORLD_STICKY_WEB`
  so every battle in Aaron's Elite Four room starts with the player's side webbed
  (grounded party members are slowed on send-out).
* Route 201's `weather` field was used to test the custom terrains and snow, and
  is currently back at `OVERWORLD_WEATHER_CLEAR` (no net change).
  `.preloadedMapObjectsArchiveID` / `.battleBG` for Route 201 are vanilla.

---

## Build / repo housekeeping

* `generated/meson.build` + `res/items/meson.build` (`fd081bca0`) — `itemproc`
  now takes `generated/items.h` as a real `input` (via a new `c_consts_by_name`
  map exported from `generated/meson.build`) instead of a bare path string, so it
  re-runs when the item list changes. Without this a newly added item built with
  a blank name and crashed on use. (Upstream had this as a `# TODO`.)
* `subprojects/SDATTool` (`1466977b3`) — an accidental gitlink with no
  `.gitmodules` and no build reference; untracked and added to `.gitignore`.
* `/EditedMaps/` added to `.gitignore` — local staging directory for DSPRE map
  exports before they are copied into `res/field/maps/data/`; not part of the
  build.

---

## Notes for maintainers

* Line-ending churn: `git status` may show `Makefile`, various `res/**/*.order`
  files, and `tools/nitrogfx|nitrorom|nitrosfx/*` as modified. That is a
  CRLF/LF artifact of mixing Windows and WSL git clients — do not commit it.
* Canonical working tree is the WSL checkout `~/dev/phmode/phmode`. Build with
  `make` or `ninja -C build pokeplatinum.us.nds`.
