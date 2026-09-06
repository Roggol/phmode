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
their bit.

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

---

## Abilities

Newly implemented abilities, added to `generated/abilities.txt` (127-142) with
`res/text/ability_names.json` / `ability_names_uppercase.json` /
`ability_descriptions.json` entries. The creation-trio abilities (Time Warp /
Space Warp / Distortion Surge, 124-126) are covered under "Battle changes".

* **Sheer Force** (127) — `src/battle/battle_lib.c`. A move with a non-zero
  `effectChance` deals ×1.3 damage in `BattleSystem_CalcMoveDamage`, and its
  chance-based added effect is suppressed (`BattleSystem_TriggerSecondaryEffect`
  skips the roll).
* **Defiant** (128) / **Competitive** (129) — `src/battle/battle_script.c`,
  `BtlCmd_ChangeStatStage`. When an opposing battler lowers one of this Pokémon's
  stats, Defiant raises its Attack and Competitive raises its Sp. Atk by two
  stages. Applied silently — the stat bar updates and the triggering "stat fell!"
  line still shows, but there is no dedicated animated subscript. Defiant is
  fully implemented but currently unused by any species.
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
survives any otherwise-lethal hit at 1 HP. It reuses the Endure code path, so the
existing "{mon} endured the hit!" message is shown. Mold Breaker ignores it
(`Battler_IgnorableAbility`).

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
side-conscious message tag.

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
modern "power becomes 120 in terrain". The modern doubles-only spread effect is
not reproduced.

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

---

## Notes for maintainers

* Line-ending churn: `git status` may show `Makefile`, various `res/**/*.order`
  files, and `tools/nitrogfx|nitrorom|nitrosfx/*` as modified. That is a
  CRLF/LF artifact of mixing Windows and WSL git clients — do not commit it.
* Canonical working tree is the WSL checkout `~/dev/phmode/phmode`. Build with
  `make` or `ninja -C build pokeplatinum.us.nds`.
