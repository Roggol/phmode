#ifndef AI_TESTS_AI_MODEL_H
#define AI_TESTS_AI_MODEL_H

/*
 * A small, hand-written model of specific decision rules from the real
 * trainer AI (src/battle/trainer_ai/trainer_ai.c and script.s), simplified
 * enough to build and run natively on a PC with a plain C compiler.
 *
 * This is NOT the ROM's own code — see ../README.md for why, and for what
 * that means when the real AI changes. Keep this file's logic in sync by
 * hand with whatever it's modeling.
 */

/* ---- Abilities relevant to the scenarios modeled here ---- */
typedef enum Ability {
    ABILITY_NONE,
    ABILITY_DRY_SKIN,
    ABILITY_LEVITATE,
    ABILITY_WATER_ABSORB,
    ABILITY_VOLT_ABSORB,
    ABILITY_MOTOR_DRIVE,
    ABILITY_FLASH_FIRE,
    ABILITY_WONDER_GUARD,
    ABILITY_STALL,
} Ability;

/* ---- Move types relevant to the scenarios modeled here ---- */
typedef enum MoveType {
    TYPE_NORMAL,
    TYPE_WATER,
    TYPE_GROUND,
    TYPE_ELECTRIC,
    TYPE_FIRE,
} MoveType;

/* ---- Held-item hold effects relevant to the scenarios modeled here ---- */
typedef enum HoldEffect {
    HOLD_EFFECT_NONE,
    HOLD_EFFECT_PRIORITY_DOWN,        /* Lagging Tail, Full Incense: holder always moves last */
    HOLD_EFFECT_SPEED_DOWN_GROUNDED,  /* Iron Ball: also forces the holder to be grounded */
} HoldEffect;

/* ---- Status conditions (bitmask, matches how the real MON_CONDITION_* flags stack) ---- */
typedef enum StatusCondition {
    STATUS_NONE      = 0,
    STATUS_SLEEP     = 1 << 0,
    STATUS_POISON    = 1 << 1,
    STATUS_BURN      = 1 << 2,
    STATUS_FREEZE    = 1 << 3,
    STATUS_PARALYSIS = 1 << 4,
    STATUS_TOXIC     = 1 << 5,
} StatusCondition;

typedef struct BattleMon {
    int curHP;
    int maxHP;
    Ability ability;
    HoldEffect heldItemEffect;
    StatusCondition status;
    int confused; /* volatile condition, tracked separately from `status` in the real game too */
} BattleMon;

/*
 * Mirrors Basic_CheckForImmunity in script.s: does this defender's ability
 * make it immune to (or, for Dry Skin, healed by) a move of this type?
 */
int AI_IsImmuneToMove(const BattleMon *defender, MoveType moveType);

/*
 * What the AI knows about the defender's ability, for the purposes of the
 * "possible but unconfirmed" soft penalty below. Mirrors CheckBattlerAbility's
 * distinction between a confirmed/revealed ability and a genuinely unknown one
 * that could still be either of the species' two possible ability slots.
 */
typedef struct OpponentAbilityKnowledge {
    int confirmed;     /* TRUE if `ability` below is confirmed; FALSE if genuinely unknown */
    Ability ability;   /* only meaningful when confirmed */
    Ability possible1; /* only meaningful when !confirmed: the species' possible ability slots */
    Ability possible2; /* ABILITY_NONE if the species only has one possible ability */
} OpponentAbilityKnowledge;

/*
 * Mirrors Basic_CheckPossibleImmunityAbility (script.s): when the defender's
 * ability isn't confirmed, but one of its two POSSIBLE abilities would grant
 * an immunity to a move of this type, this returns a small negative score
 * (-2) as a deterrent -- not the full immunity-level penalty, since the AI
 * isn't actually sure. Returns 0 when there's nothing to be cautious about
 * (ability already confirmed and not an immunity, or neither possible
 * ability would matter for this type).
 */
int AI_PossibleImmunityPenalty(const OpponentAbilityKnowledge *defenderAbility, MoveType moveType);

/*
 * Mirrors the "does this battler reliably move last" check that
 * Basic_CheckMetalBurst is supposed to make.
 */
int AI_AlwaysMovesLast(const BattleMon *mon);

/*
 * Mirrors BattlerIsGrounded (battle_lib.c), as approximated by the AI script (no way to
 * check Magnet Rise): Gravity or a held "speed down when grounded" item (Iron Ball) forces
 * grounding; absent those, Levitate or a Flying typing means airborne; otherwise grounded.
 */
typedef struct GroundednessInputs {
    int gravityActive;
    HoldEffect heldItemEffect;
    Ability ability;
    int isFlyingType;
} GroundednessInputs;

int AI_IsGroundedForTerrain(const GroundednessInputs *mon);

/*
 * Mirrors Basic_CheckCannotSleep's new Electric Terrain branch and Expert_Rest's matching
 * check: a sleep-inducing move (or Rest, checked against its own user instead of a target)
 * against a grounded battler under Electric Terrain just fails outright. Returns -10 if so,
 * 0 otherwise.
 */
int AI_ElectricTerrainSleepPenalty(int terrainActive, const GroundednessInputs *target);

/*
 * Mirrors the new check at the top of Basic_Main: a positive-priority move against a
 * grounded opponent under Psychic Terrain just fails outright. Returns -10 if so, 0
 * otherwise. (Whether the target is actually an opponent, not our own partner, is assumed
 * to already be established by the caller, same as in the real AI script.)
 */
int AI_PsychicTerrainPriorityPenalty(int terrainActive, int movePriority, const GroundednessInputs *target);

/* What kind of stat-changing move this is, for Distortion Terrain's purposes. */
typedef enum StatMoveCategory {
    STAT_MOVE_NONE,                      /* doesn't change any stats at all */
    STAT_MOVE_GUARANTEED_SELF_BOOST,     /* e.g. Swords Dance, Calm Mind, Belly Drum */
    STAT_MOVE_CHANCE_SELF_BOOST,         /* e.g. Metal Claw, Ancient Power */
    STAT_MOVE_GUARANTEED_SELF_LOWER,     /* e.g. Superpower, Close Combat, Overheat, Hammer Arm */
    STAT_MOVE_GUARANTEED_OPPONENT_LOWER, /* e.g. Growl, Leer, Tickle, Captivate */
    STAT_MOVE_CHANCE_OPPONENT_LOWER,     /* e.g. Rock Smash, Crunch, Psychic, Iron Tail */
} StatMoveCategory;

/*
 * Mirrors the checks at the top of Basic_ScoreMoveEffect (Basic_CheckDistortionStatMove):
 * Distortion Terrain inverts every stat-stage change on the field, so:
 *  - a move that GUARANTEES raising the user's own stats backfires hard (-8)
 *  - one that only has a CHANCE to raise the user's own stats backfires lightly (-1)
 *  - a move that GUARANTEES LOWERING the user's own stats (Superpower, Close Combat,
 *    Overheat/Draco Meteor/Psycho Boost, Hammer Arm) becomes a hidden benefit instead, so
 *    it's rewarded (+8) - its damage still matters on top of that, so the real AI script
 *    continues normal scoring afterward rather than terminating (not modeled here, since
 *    this function only returns the terrain-specific delta)
 *  - a move that GUARANTEES lowering the OPPONENT's stats (Growl, Leer, Tickle, Captivate)
 *    would instead raise them, directly helping the target - same heavy -8 deterrent
 *  - one that only has a CHANCE to lower the opponent's stats on a hit (Rock Smash, Crunch,
 *    Psychic, Iron Tail) is only lightly discouraged (-1)
 * `attackerHasSheerForce` mirrors the real AI's ability gate: Sheer Force suppresses any
 * move's secondary "chance" effect entirely (its effect_chance is 0 to begin with for a
 * Sheer Force attacker), so there's nothing left to invert for the two CHANCE categories -
 * but it does NOT affect the two GUARANTEED categories, since those are the move's own
 * built-in effect (effect_chance 0 in the move data), not a secondary chance Sheer Force
 * would touch. Returns 0 if the terrain isn't active or the move doesn't touch any stats
 * covered above.
 */
int AI_DistortionTerrainStatPenalty(int terrainActive, StatMoveCategory category, int attackerHasSheerForce);

/*
 * Mirrors the guaranteed part of BtlCmd_CheckHoldOnWith1HP (battle_script.c), as newly
 * accounted for in AICmd_IfCurrentMoveKills/IfCurrentMoveDoesNotKill (trainer_ai.c): a
 * defender at full HP with Sturdy (unless the attacker's ability is Mold Breaker) or a
 * held Focus Sash survives an otherwise-lethal hit at 1 HP instead of fainting, so the AI
 * should not treat such a hit as a kill. Doesn't model Focus Band (HOLD_EFFECT_MAYBE_ENDURE),
 * since that's a random per-hit chance rather than something the AI can rely on.
 */
typedef enum SurvivalItemOrAbility {
    SURVIVAL_NONE,
    SURVIVAL_STURDY,
    SURVIVAL_FOCUS_SASH,
} SurvivalItemOrAbility;

int AI_DefenderSurvivesLethalHitAtOneHP(int defenderAtFullHP, SurvivalItemOrAbility defenderSurvivalTrait, int attackerHasMoldBreaker);

/* Mirrors the real AI script's precomputed IfSpeedCompareEqualTo result. */
typedef enum SpeedComparison {
    SPEED_COMPARE_FASTER,
    SPEED_COMPARE_SLOWER,
    SPEED_COMPARE_TIE,
} SpeedComparison;

/*
 * Mirrors Expert_StatusParalyze (script.s): the score adjustment for using a
 * pure paralysis status move (Thunder Wave, Stun Spore, Glare).
 *
 * If the attacker is slower than its target, there's a ~92.2% chance of +3
 * (paralysis halves the target's Speed, which is most valuable when you're
 * the slower one). Otherwise, a hurt attacker (<=70% HP) gets -1 for wasting
 * a turn on a status move instead of attacking or switching.
 *
 * `randomRoll` mirrors the real IfRandomLessThan 20 check (0-255): rolls
 * below 20 land in the ~7.8% chance of no bonus; everything else scores +3.
 */
int AI_ScoreStatusParalyzeMove(SpeedComparison speed, int attackerHPPercent, int randomRoll);

/*
 * Mirrors the new Electric-type check at the top of Basic_CheckCannotParalyze (script.s):
 * modern Electric-types are immune to paralysis entirely, regardless of what move or effect
 * would have caused it (Thunder Wave, Stun Spore, Glare, a damaging move's paralysis chance,
 * ...). This is a status immunity tied to the target's own typing, not a type-chart
 * interaction, so it needs its own explicit check rather than falling out of the normal
 * move-effectiveness lookup. Returns -10 if the defender is Electric-type (either slot), 0
 * otherwise. The other pre-existing paralysis-immunity checks (Limber, Magic Guard, an
 * already-statused target, Safeguard, Motor Drive/Volt Absorb specifically against Thunder
 * Wave) are unchanged by this and not modeled here.
 */
int AI_ElectricTypeParalysisImmunityPenalty(int defenderType1IsElectric, int defenderType2IsElectric);

/*
 * Mirrors the new Grass-type powder-move immunity check added to Basic_CheckCannotSleep,
 * Basic_CheckCannotPoison, Basic_CheckCannotParalyze, and Basic_CheckLowStatStage_Speed
 * (script.s): Grass-types are immune to any move flagged MOVE_FLAG_POWDER (Sleep Powder,
 * Spore, Poison Powder, Stun Spore, Cotton Spore) entirely, regardless of what status or
 * stat-drop it would otherwise inflict. Each of those four dispatch points gates this behind
 * a check of the CURRENT move's own flags (via the new LoadCurrentMoveFlags AI command), not
 * just its effect ID, so a non-powder move that happens to share the same effect (Growl's
 * ATK_DOWN vs. an unrelated guaranteed-power move, Yawn vs. Sleep Powder, String Shot vs.
 * Cotton Spore) is correctly left unaffected. Returns -10 if both are true, 0 otherwise.
 */
int AI_PowderMoveImmunityPenalty(int moveIsPowder, int targetIsGrassType);

/*
 * Mirrors the new Ghost-type trapping immunity added to Basic_CheckMeanLook (script.s):
 * Ghost-types can't be trapped by Mean Look/Block/Spider Web at all (modern mechanic, along
 * with binding moves, Arena Trap, Shadow Tag, and Magnet Pull - Ghost-types were already
 * made immune to being trapped battle-engine-side, in Battler_IsTrapped/Battler_IsTrappedMsg
 * in battle_lib.c and subscript_mean_look.s/subscript_bind_start.s). Returns -10 if the
 * target is Ghost-type, 0 otherwise. Expert_BindingMove's separate "lock them in for the
 * kill" utility bonus is skipped entirely against a Ghost-type target (rather than
 * penalized) - not modeled here as its own function, since "don't apply a bonus" doesn't
 * need a return-value contract the way a score delta does.
 */
int AI_GhostTrappingImmunityPenalty(int targetIsGhostType);

/* How effective a move is against its target, for Sucker Punch's purposes. */
typedef enum MoveEffectiveness {
    EFFECTIVENESS_NORMAL_OR_SUPER,
    EFFECTIVENESS_RESISTED, /* half or quarter damage */
    EFFECTIVENESS_IMMUNE,
} MoveEffectiveness;

/*
 * Mirrors Expert_SuckerPunch (script.s): if the target resists or is immune, a flat -1 and
 * nothing else is checked (Sucker Punch is a weak choice regardless of whether it lands
 * first). Otherwise, two independent bonuses can both apply:
 *  - +2 if the target's last used move dealt damage (i.e. wasn't a status move, and they
 *    weren't just switched in with no move history yet) - Sucker Punch only works if the
 *    target attacks this turn, and a target who just attacked is more likely to keep
 *    attacking than switch to a status move. This is genuinely known information (their own
 *    last move, already visible on screen), not a guess at what they're about to choose this
 *    turn.
 *  - a further ~75% chance of +1 regardless (the same baseline optimism any decent priority
 *    move gets). `randomRoll` mirrors the real IfRandomLessThan 64 check (0-255): rolls below
 *    64 miss this chance.
 */
int AI_ScoreSuckerPunch(MoveEffectiveness effectiveness, int targetLastMoveDealtDamage, int randomRoll);

/*
 * Mirrors Basic_CheckStealthRock's pattern, shared verbatim by the new Basic_CheckStickyWebMove
 * (script.s): an entry hazard move scores -10 if the target's side already has that hazard set
 * (setting it again would do nothing), or -10 if the target is down to their last Pokemon
 * (nothing left for the hazard to matter against) -- these are two separate reasons for the
 * same -10, not stacking penalties, matching the real script's terminate-on-first-match control
 * flow. Used for both Stealth Rock and Sticky Web.
 */
int AI_HazardMovePenalty(int targetSideAlreadyHasHazard, int targetOnLastMon);

/*
 * What a move's effect would lose if the target is behind a Substitute, mirroring the new
 * check at the top of Basic_ScoreMoveEffect (script.s). Verified against the actual battle
 * scripts rather than assumed: subscript_paralyze.s, subscript_poison.s, subscript_confuse.s,
 * subscript_leech_seed_start.s, subscript_yawn.s, subscript_mean_look.s,
 * subscript_embargo_start.s, subscript_heal_block_start.s,
 * subscript_suppress_target_ability.s, subscript_nightmare_start.s, subscript_fall_asleep.s,
 * and BtlCmd_ChangeStatStage's jumpBlockedBySubstitute branch (battle_script.c) all block
 * their effect outright when the target has an active Substitute.
 */
typedef enum SubstituteEffectCategory {
    SUBSTITUTE_EFFECT_NONE,          /* unaffected by the target's Substitute either way */
    SUBSTITUTE_EFFECT_PURE_STATUS,   /* e.g. Thunder Wave, Toxic, Growl - the whole move does nothing */
    SUBSTITUTE_EFFECT_CHANCE_ON_HIT, /* e.g. Rock Smash's Defense-lowering chance - the move still deals damage */
} SubstituteEffectCategory;

/*
 * Returns -10 for a pure status effect fully blocked by an active Substitute (the move does
 * nothing at all), -1 for a damaging move's secondary chance-based effect being blocked (the
 * move still deals its normal damage - only the bonus is lost), or 0 if the target has no
 * Substitute up or the move's effect isn't affected by one either way.
 *
 * Deliberately does NOT cover Disable, Taunt, Torment, Encore, or Attract: verified against
 * their actual implementations (BtlCmd_TryDisable/TryEncore/TryAttract in battle_script.c,
 * and subscript_taunt_start.s/subscript_torment_start.s) that none of them check the
 * target's Substitute at all in this decomp -- a pre-existing bug, unrelated to this check,
 * that means those five moves currently still work against a Substitute. Scoring them as
 * blocked here would make the AI wrongly avoid a move that actually functions fine today.
 */
int AI_SubstituteBlockPenalty(int targetHasSubstitute, SubstituteEffectCategory category);

/*
 * Mirrors the new AI_ShouldSwitchForHazards (trainer_ai.c): a small nudge (1-in-6, i.e.
 * BattleSystem_RandNext() % 6 == 0 in the real AI) toward voluntarily switching out to bring
 * in a benched Pokemon that knows Defog or Rapid Spin, but only when our own side already has
 * an entry hazard (Spikes/Toxic Spikes/Stealth Rock/Sticky Web) up. `randomRollMod6` mirrors
 * that roll directly (0 = the 1-in-6 chance hit).
 */
int AI_ShouldSwitchForHazards(int ownSideHasHazard, int benchHasHazardRemovalMove, int randomRollMod6);

/*
 * Mirrors the new Stage 0 preference at the top of BattleAI_PostKOSwitchIn (battle_lib.c):
 * when choosing which benched Pokemon to switch in (after a KO, Perish Song, or any other
 * switch that didn't already pick a specific target), a benched Defog/Rapid Spin user is
 * preferred 2-in-3 of the time over the usual matchup-based picks, but only when our own side
 * already has an entry hazard up. `randomRollMod3` mirrors BattleSystem_RandNext() % 3 (0 or 1
 * = the 2-in-3 chance hit).
 */
int AI_PrefersHazardRemovalSwitchIn(int ownSideHasHazard, int benchHasHazardRemovalMove, int randomRollMod3);

/* ---- Item categories (mirrors the ITEM_AI_CATEGORY_* groups trainer_ai.c sorts items into) ---- */
typedef enum AIItemKind {
    ITEM_KIND_NONE,
    ITEM_KIND_FULL_RESTORE,
    ITEM_KIND_HP_RESTORE,
    ITEM_KIND_HEAL_SLEEP,
    ITEM_KIND_HEAL_POISON,
    ITEM_KIND_HEAL_BURN,
    ITEM_KIND_HEAL_FREEZE,
    ITEM_KIND_HEAL_PARALYSIS,
    ITEM_KIND_HEAL_CONFUSION,
} AIItemKind;

typedef struct AIItem {
    const char *name;
    AIItemKind kind;
    int hpRestored; /* only meaningful for ITEM_KIND_HP_RESTORE, e.g. Hyper Potion = 200 */
    int used;       /* set by AI_ShouldUseItem[_PreFixBuggy] when this slot is picked */
} AIItem;

typedef struct AIItemDecision {
    int shouldUse;
    int slotIndex; /* -1 if shouldUse is false */
} AIItemDecision;

/*
 * The FIXED version of TrainerAI_ShouldUseItem: stops checking further item
 * slots as soon as one is chosen, since only one item is ever used per turn.
 */
AIItemDecision AI_ShouldUseItem(const BattleMon *mon, AIItem *items, int itemCount);

/*
 * A faithful reproduction of the ORIGINAL base-game bug: keeps scanning after
 * finding a usable item, with no `break` and no reset of the shared "should
 * use" flag, so every later slot's `used` flag gets set too — even slots
 * whose own condition was never actually met. Kept around specifically so a
 * regression test can demonstrate the bug and prove the fixed version above
 * no longer does this. Do not "fix" this function — that's the point of it.
 */
AIItemDecision AI_ShouldUseItem_PreFixBuggy(const BattleMon *mon, AIItem *items, int itemCount);

#endif /* AI_TESTS_AI_MODEL_H */
