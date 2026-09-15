#include "ai_model.h"

int AI_IsImmuneToMove(const BattleMon *defender, MoveType moveType)
{
    switch (defender->ability) {
    case ABILITY_VOLT_ABSORB:
        return moveType == TYPE_ELECTRIC;
    case ABILITY_WATER_ABSORB:
        return moveType == TYPE_WATER;
    case ABILITY_FLASH_FIRE:
        return moveType == TYPE_FIRE;
    case ABILITY_LEVITATE:
        return moveType == TYPE_GROUND;
    case ABILITY_DRY_SKIN:
        /* phmode fix: Basic_CheckForImmunity used to check ABILITY_LEVITATE a second
         * time here instead of ABILITY_DRY_SKIN, so this branch was dead code and the
         * AI never recognized Dry Skin's Water immunity at all. */
        return moveType == TYPE_WATER;
    case ABILITY_WONDER_GUARD:
        /* Modeled elsewhere via move effectiveness in the real game; not exercised here. */
    default:
        return 0;
    }
}

static int IsPossibleUnconfirmedAbility(const OpponentAbilityKnowledge *k, Ability ability)
{
    if (k->confirmed) {
        return 0;
    }

    return k->possible1 == ability || k->possible2 == ability;
}

int AI_PossibleImmunityPenalty(const OpponentAbilityKnowledge *defenderAbility, MoveType moveType)
{
    switch (moveType) {
    case TYPE_ELECTRIC:
        if (IsPossibleUnconfirmedAbility(defenderAbility, ABILITY_VOLT_ABSORB)
            || IsPossibleUnconfirmedAbility(defenderAbility, ABILITY_MOTOR_DRIVE)) {
            return -2;
        }
        break;
    case TYPE_WATER:
        if (IsPossibleUnconfirmedAbility(defenderAbility, ABILITY_WATER_ABSORB)
            || IsPossibleUnconfirmedAbility(defenderAbility, ABILITY_DRY_SKIN)) {
            return -2;
        }
        break;
    case TYPE_FIRE:
        if (IsPossibleUnconfirmedAbility(defenderAbility, ABILITY_FLASH_FIRE)) {
            return -2;
        }
        break;
    case TYPE_GROUND:
        if (IsPossibleUnconfirmedAbility(defenderAbility, ABILITY_LEVITATE)) {
            return -2;
        }
        break;
    case TYPE_NORMAL:
    default:
        break;
    }

    return 0;
}

int AI_IsGroundedForTerrain(const GroundednessInputs *mon)
{
    if (mon->gravityActive) {
        return 1;
    }

    if (mon->heldItemEffect == HOLD_EFFECT_SPEED_DOWN_GROUNDED) {
        return 1;
    }

    if (mon->ability == ABILITY_LEVITATE || mon->isFlyingType) {
        return 0;
    }

    return 1;
}

int AI_ElectricTerrainSleepPenalty(int terrainActive, const GroundednessInputs *target)
{
    if (terrainActive && AI_IsGroundedForTerrain(target)) {
        return -10;
    }

    return 0;
}

int AI_PsychicTerrainPriorityPenalty(int terrainActive, int movePriority, const GroundednessInputs *target)
{
    if (terrainActive && movePriority > 0 && AI_IsGroundedForTerrain(target)) {
        return -10;
    }

    return 0;
}

int AI_DistortionTerrainStatPenalty(int terrainActive, StatMoveCategory category, int attackerHasSheerForce)
{
    if (!terrainActive) {
        return 0;
    }

    switch (category) {
    case STAT_MOVE_GUARANTEED_SELF_BOOST:
        return -8;
    case STAT_MOVE_GUARANTEED_SELF_LOWER:
        return 8;
    case STAT_MOVE_GUARANTEED_OPPONENT_LOWER:
        return -8;
    case STAT_MOVE_CHANCE_SELF_BOOST:
        return attackerHasSheerForce ? 0 : -1;
    case STAT_MOVE_CHANCE_OPPONENT_LOWER:
        return attackerHasSheerForce ? 0 : -1;
    case STAT_MOVE_NONE:
    default:
        return 0;
    }
}

int AI_DefenderSurvivesLethalHitAtOneHP(int defenderAtFullHP, SurvivalItemOrAbility defenderSurvivalTrait, int attackerHasMoldBreaker)
{
    if (!defenderAtFullHP) {
        return 0;
    }

    if (defenderSurvivalTrait == SURVIVAL_FOCUS_SASH) {
        return 1;
    }

    if (defenderSurvivalTrait == SURVIVAL_STURDY) {
        return !attackerHasMoldBreaker;
    }

    return 0;
}

int AI_AlwaysMovesLast(const BattleMon *mon)
{
    /* phmode fix: Basic_CheckMetalBurst used to check for a held Shiny Stone here,
     * which has nothing to do with turn order, instead of this hold effect. */
    return mon->ability == ABILITY_STALL || mon->heldItemEffect == HOLD_EFFECT_PRIORITY_DOWN;
}

int AI_ScoreStatusParalyzeMove(SpeedComparison speed, int attackerHPPercent, int randomRoll)
{
    if (speed == SPEED_COMPARE_SLOWER) {
        if (randomRoll < 20) {
            return 0;
        }
        return 3;
    }

    if (attackerHPPercent > 70) {
        return 0;
    }

    return -1;
}

int AI_HazardMovePenalty(int targetSideAlreadyHasHazard, int targetOnLastMon)
{
    /* Mirrors the real script.s control flow: the first matching check jumps straight to
     * ScoreMinus10 and terminates (PopOrEnd), so at most one -10 applies -- these are two
     * separate reasons to distrust the move, not stacking penalties. */
    if (targetSideAlreadyHasHazard) {
        return -10;
    }

    if (targetOnLastMon) {
        return -10;
    }

    return 0;
}

int AI_SubstituteBlockPenalty(int targetHasSubstitute, SubstituteEffectCategory category)
{
    if (!targetHasSubstitute) {
        return 0;
    }

    switch (category) {
    case SUBSTITUTE_EFFECT_PURE_STATUS:
        return -10;
    case SUBSTITUTE_EFFECT_CHANCE_ON_HIT:
        return -1;
    case SUBSTITUTE_EFFECT_NONE:
    default:
        return 0;
    }
}

int AI_ShouldSwitchForHazards(int ownSideHasHazard, int benchHasHazardRemovalMove, int randomRollMod6)
{
    return ownSideHasHazard && benchHasHazardRemovalMove && randomRollMod6 == 0;
}

int AI_PrefersHazardRemovalSwitchIn(int ownSideHasHazard, int benchHasHazardRemovalMove, int randomRollMod3)
{
    return ownSideHasHazard && benchHasHazardRemovalMove && randomRollMod3 < 2;
}

int AI_ElectricTypeParalysisImmunityPenalty(int defenderType1IsElectric, int defenderType2IsElectric)
{
    if (defenderType1IsElectric || defenderType2IsElectric) {
        return -10;
    }

    return 0;
}

int AI_PowderMoveImmunityPenalty(int moveIsPowder, int targetIsGrassType)
{
    if (moveIsPowder && targetIsGrassType) {
        return -10;
    }

    return 0;
}

int AI_GhostTrappingImmunityPenalty(int targetIsGhostType)
{
    if (targetIsGhostType) {
        return -10;
    }

    return 0;
}

int AI_ScoreSuckerPunch(MoveEffectiveness effectiveness, int targetLastMoveDealtDamage, int randomRoll)
{
    int score;

    if (effectiveness == EFFECTIVENESS_RESISTED || effectiveness == EFFECTIVENESS_IMMUNE) {
        return -1;
    }

    score = 0;

    if (targetLastMoveDealtDamage) {
        score += 2;
    }

    if (randomRoll >= 64) {
        score += 1;
    }

    return score;
}

static AIItemKind ClassifyAndCheck(const BattleMon *mon, const AIItem *item)
{
    switch (item->kind) {
    case ITEM_KIND_FULL_RESTORE:
        if (mon->curHP > 0 && mon->curHP < mon->maxHP / 4) {
            return ITEM_KIND_FULL_RESTORE;
        }
        break;
    case ITEM_KIND_HP_RESTORE:
        /* Use an HP restore item if the battler is critically low, or if the item
         * wouldn't fully heal it anyway (so nothing goes to waste). */
        if (mon->curHP > 0 && item->hpRestored > 0
            && (mon->curHP < mon->maxHP / 4 || (mon->maxHP - mon->curHP) > item->hpRestored)) {
            return ITEM_KIND_HP_RESTORE;
        }
        break;
    case ITEM_KIND_HEAL_SLEEP:
        if (mon->status & STATUS_SLEEP) {
            return ITEM_KIND_HEAL_SLEEP;
        }
        break;
    case ITEM_KIND_HEAL_POISON:
        if (mon->status & (STATUS_POISON | STATUS_TOXIC)) {
            return ITEM_KIND_HEAL_POISON;
        }
        break;
    case ITEM_KIND_HEAL_BURN:
        if (mon->status & STATUS_BURN) {
            return ITEM_KIND_HEAL_BURN;
        }
        break;
    case ITEM_KIND_HEAL_FREEZE:
        if (mon->status & STATUS_FREEZE) {
            return ITEM_KIND_HEAL_FREEZE;
        }
        break;
    case ITEM_KIND_HEAL_PARALYSIS:
        if (mon->status & STATUS_PARALYSIS) {
            return ITEM_KIND_HEAL_PARALYSIS;
        }
        break;
    case ITEM_KIND_HEAL_CONFUSION:
        if (mon->confused) {
            return ITEM_KIND_HEAL_CONFUSION;
        }
        break;
    case ITEM_KIND_NONE:
    default:
        break;
    }

    return ITEM_KIND_NONE;
}

AIItemDecision AI_ShouldUseItem(const BattleMon *mon, AIItem *items, int itemCount)
{
    AIItemDecision decision = { 0, -1 };

    for (int i = 0; i < itemCount; i++) {
        if (items[i].kind == ITEM_KIND_NONE) {
            continue;
        }

        if (ClassifyAndCheck(mon, &items[i]) != ITEM_KIND_NONE) {
            items[i].used = 1;
            decision.shouldUse = 1;
            decision.slotIndex = i;
            break; /* phmode fix: stop as soon as one item is picked. */
        }
    }

    return decision;
}

AIItemDecision AI_ShouldUseItem_PreFixBuggy(const BattleMon *mon, AIItem *items, int itemCount)
{
    AIItemDecision decision = { 0, -1 };
    int result = 0;

    for (int i = 0; i < itemCount; i++) {
        if (items[i].kind == ITEM_KIND_NONE) {
            continue;
        }

        if (ClassifyAndCheck(mon, &items[i]) != ITEM_KIND_NONE) {
            result = 1;
        }

        /* Original bug: `result` is never reset between iterations, and the loop
         * never stops, so once any earlier slot sets it, every later slot's `used`
         * flag gets set too here -- even slots whose own condition was false. */
        if (result) {
            items[i].used = 1;
            decision.shouldUse = 1;
            decision.slotIndex = i;
        }
    }

    return decision;
}
