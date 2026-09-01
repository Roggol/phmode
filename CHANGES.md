# phmode — changes from the base game

This file tracks every intentional deviation this romhack makes from the
`pokeplatinum` (Pokémon Platinum, US rev 1) decompilation it is built on. It is
grouped by area rather than by commit. When you change gameplay, add an entry
here in the same commit.

Upstream baseline: `pokeplatinum` `main` at commit `064f2900e` ("tools: Sync
nitrorom from upstream (#1276)"). Everything below is on top of that.

---

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

### Battle style locked to "Set"
* `src/game_options.c` — `Options_Init` defaults `battleStyle` to
  `OPTIONS_BATTLE_STYLE_SET` on a new game.
* `src/applications/options_menu.c` — `ProcessMainInput` ignores left/right on
  the Battle Style row, so the player cannot switch it back to "Switch".

### Summary screen: EV / IV view and nature colours
`src/applications/pokemon_summary_screen/` (`main.c`, `window.c`, `main.h`) — on
the Skills page:

* Hold **L** to show effort values in place of the six stats (drawn blue).
* Hold **R** to show individual values (drawn red).
* With neither held, each stat number is tinted by the Pokémon's nature: red for
  the boosted stat, blue for the hindered stat, default colour for a neutral
  nature.

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

---

## Battle changes

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
* **Distortion Terrain** (`de20f3c30`) — every stat-stage change on the field is
  inverted, Contrary-style, for all battlers whether grounded or not.

---

## Level cap (`5c436cf12`)

A hard cap on Pokémon level, stored in `VAR_HARD_LEVEL_CAP`.

* Seeded to `DEFAULT_HARD_LEVEL_CAP` (14) by `scripts_init_new_game.s`. A stored
  0 (pre-feature save) is treated as 14. Scripts only ever raise it — currently
  the starter-selection scene on Route 201 and the Twinleaf Town guitarist/rival
  both set it to 14. Add `SetVar VAR_HARD_LEVEL_CAP, <level>` at later badge
  points to open the cap up.
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
* Twinleaf Town guitarist (`scripts_twinleaf_town.s`,
  `res/text/twinleaf_town.json`) now gives a one-time Rare Candy.

---

## Items

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

---

## Map data

`include/data/map_headers.h` — Route 201's `weather` field was used to test the
custom terrains and snow, and is currently back at `OVERWORLD_WEATHER_CLEAR`
(no net change). `.preloadedMapObjectsArchiveID` / `.battleBG` for Route 201 are
vanilla.

---

## Build / repo housekeeping

* `generated/meson.build` + `res/items/meson.build` (`fd081bca0`) — `itemproc`
  now takes `generated/items.h` as a real `input` (via a new `c_consts_by_name`
  map exported from `generated/meson.build`) instead of a bare path string, so it
  re-runs when the item list changes. Without this a newly added item built with
  a blank name and crashed on use. (Upstream had this as a `# TODO`.)
* `subprojects/SDATTool` (`1466977b3`) — an accidental gitlink with no
  `.gitmodules` and no build reference; untracked and added to `.gitignore`.

---

## Notes for maintainers

* Line-ending churn: `git status` may show `Makefile`, various `res/**/*.order`
  files, and `tools/nitrogfx|nitrorom|nitrosfx/*` as modified. That is a
  CRLF/LF artifact of mixing Windows and WSL git clients — do not commit it.
* Canonical working tree is the WSL checkout `~/dev/phmode/phmode`. Build with
  `make` or `ninja -C build pokeplatinum.us.nds`.
