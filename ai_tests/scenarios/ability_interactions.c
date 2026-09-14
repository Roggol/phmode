#include "../ai_model.h"
#include "../testing.h"

/* ---- Dry Skin: fixed base-game bug, was dead code (see ai_model.c) ---- */

TEST(dry_skin_is_immune_to_water_moves)
{
    BattleMon mon = { .ability = ABILITY_DRY_SKIN };

    ASSERT_TRUE(AI_IsImmuneToMove(&mon, TYPE_WATER));
}

TEST(dry_skin_is_not_immune_to_unrelated_types)
{
    BattleMon mon = { .ability = ABILITY_DRY_SKIN };

    ASSERT_FALSE(AI_IsImmuneToMove(&mon, TYPE_NORMAL));
    ASSERT_FALSE(AI_IsImmuneToMove(&mon, TYPE_GROUND));
    ASSERT_FALSE(AI_IsImmuneToMove(&mon, TYPE_ELECTRIC));
}

TEST(levitate_is_immune_to_ground_but_not_water)
{
    /* Regression guard specifically for the bug: Levitate's own immunity
     * still works, and it must NOT also claim a Water immunity that only
     * Dry Skin should have. */
    BattleMon mon = { .ability = ABILITY_LEVITATE };

    ASSERT_TRUE(AI_IsImmuneToMove(&mon, TYPE_GROUND));
    ASSERT_FALSE(AI_IsImmuneToMove(&mon, TYPE_WATER));
}

TEST(water_absorb_is_immune_to_water_but_not_ground)
{
    BattleMon mon = { .ability = ABILITY_WATER_ABSORB };

    ASSERT_TRUE(AI_IsImmuneToMove(&mon, TYPE_WATER));
    ASSERT_FALSE(AI_IsImmuneToMove(&mon, TYPE_GROUND));
}

TEST(no_ability_means_no_immunity)
{
    BattleMon mon = { .ability = ABILITY_NONE };

    ASSERT_FALSE(AI_IsImmuneToMove(&mon, TYPE_WATER));
    ASSERT_FALSE(AI_IsImmuneToMove(&mon, TYPE_GROUND));
    ASSERT_FALSE(AI_IsImmuneToMove(&mon, TYPE_FIRE));
    ASSERT_FALSE(AI_IsImmuneToMove(&mon, TYPE_ELECTRIC));
}

/* ---- Lagging Tail / Metal Burst: fixed base-game bug, was checking Shiny Stone ---- */

TEST(lagging_tail_holder_is_treated_as_always_moving_last)
{
    BattleMon mon = { .heldItemEffect = HOLD_EFFECT_PRIORITY_DOWN };

    ASSERT_TRUE(AI_AlwaysMovesLast(&mon));
}

TEST(full_incense_holder_is_also_treated_as_always_moving_last)
{
    /* Full Incense shares the exact same hold effect as Lagging Tail. */
    BattleMon mon = { .heldItemEffect = HOLD_EFFECT_PRIORITY_DOWN };

    ASSERT_TRUE(AI_AlwaysMovesLast(&mon));
}

TEST(stall_ability_also_means_always_moving_last)
{
    BattleMon mon = { .ability = ABILITY_STALL };

    ASSERT_TRUE(AI_AlwaysMovesLast(&mon));
}

TEST(shiny_stone_holder_is_not_treated_as_moving_last)
{
    /* This is the bug in miniature: an unrelated evolution item must have
     * no bearing on turn order at all. Modeled here as "no hold effect",
     * since a Shiny Stone's real hold effect (none) has nothing to do with
     * HOLD_EFFECT_PRIORITY_DOWN. */
    BattleMon mon = { .heldItemEffect = HOLD_EFFECT_NONE };

    ASSERT_FALSE(AI_AlwaysMovesLast(&mon));
}

TEST(ordinary_mon_with_no_relevant_ability_or_item_does_not_move_last)
{
    BattleMon mon = { .ability = ABILITY_NONE, .heldItemEffect = HOLD_EFFECT_NONE };

    ASSERT_FALSE(AI_AlwaysMovesLast(&mon));
}

/* ---- Possible-but-unconfirmed immunity ability: a slight, not full, deterrent ---- */

TEST(unconfirmed_water_absorb_possibility_gives_water_moves_a_small_penalty)
{
    /* E.g. a Quagsire whose ability hasn't been revealed yet: Damp or Water
     * Absorb. The AI doesn't know which -- just that Water Absorb is possible. */
    OpponentAbilityKnowledge knowledge = {
        .confirmed = 0,
        .possible1 = ABILITY_WATER_ABSORB,
        .possible2 = ABILITY_NONE, /* stands in for "Damp" here, which isn't modeled */
    };

    int penalty = AI_PossibleImmunityPenalty(&knowledge, TYPE_WATER);

    ASSERT_EQ(penalty, -2);
}

TEST(unconfirmed_dry_skin_possibility_also_gives_water_moves_a_small_penalty)
{
    OpponentAbilityKnowledge knowledge = {
        .confirmed = 0,
        .possible1 = ABILITY_NONE,
        .possible2 = ABILITY_DRY_SKIN,
    };

    int penalty = AI_PossibleImmunityPenalty(&knowledge, TYPE_WATER);

    ASSERT_EQ(penalty, -2);
}

TEST(unconfirmed_immunity_possibility_does_not_affect_unrelated_move_types)
{
    /* Water Absorb is only relevant to Water-type moves. */
    OpponentAbilityKnowledge knowledge = {
        .confirmed = 0,
        .possible1 = ABILITY_WATER_ABSORB,
        .possible2 = ABILITY_NONE,
    };

    ASSERT_EQ(AI_PossibleImmunityPenalty(&knowledge, TYPE_ELECTRIC), 0);
    ASSERT_EQ(AI_PossibleImmunityPenalty(&knowledge, TYPE_GROUND), 0);
}

TEST(confirmed_ability_never_triggers_the_small_penalty)
{
    /* Once the ability is actually confirmed, this soft-uncertainty rule
     * doesn't apply at all -- the confirmed/guessed full-immunity check
     * (AI_IsImmuneToMove) is what handles this case instead, elsewhere. */
    OpponentAbilityKnowledge knowledge = {
        .confirmed = 1,
        .ability = ABILITY_WATER_ABSORB,
    };

    ASSERT_EQ(AI_PossibleImmunityPenalty(&knowledge, TYPE_WATER), 0);
}

TEST(species_with_no_possible_immunity_ability_gets_no_penalty)
{
    OpponentAbilityKnowledge knowledge = {
        .confirmed = 0,
        .possible1 = ABILITY_STALL,
        .possible2 = ABILITY_NONE,
    };

    ASSERT_EQ(AI_PossibleImmunityPenalty(&knowledge, TYPE_WATER), 0);
    ASSERT_EQ(AI_PossibleImmunityPenalty(&knowledge, TYPE_ELECTRIC), 0);
    ASSERT_EQ(AI_PossibleImmunityPenalty(&knowledge, TYPE_FIRE), 0);
    ASSERT_EQ(AI_PossibleImmunityPenalty(&knowledge, TYPE_GROUND), 0);
}

TEST(unconfirmed_motor_drive_possibility_gives_electric_moves_a_small_penalty)
{
    OpponentAbilityKnowledge knowledge = {
        .confirmed = 0,
        .possible1 = ABILITY_MOTOR_DRIVE,
        .possible2 = ABILITY_NONE,
    };

    ASSERT_EQ(AI_PossibleImmunityPenalty(&knowledge, TYPE_ELECTRIC), -2);
}

TEST(unconfirmed_levitate_possibility_gives_ground_moves_a_small_penalty)
{
    OpponentAbilityKnowledge knowledge = {
        .confirmed = 0,
        .possible1 = ABILITY_LEVITATE,
        .possible2 = ABILITY_NONE,
    };

    ASSERT_EQ(AI_PossibleImmunityPenalty(&knowledge, TYPE_GROUND), -2);
}

TEST(small_penalty_is_deliberately_gentler_than_a_confirmed_immunity)
{
    /* The whole point of this rule: uncertainty gets a nudge, not a wall. A
     * confirmed immunity (modeled by AI_IsImmuneToMove) is a hard stop for
     * the move entirely; this is just a light deterrent alongside it. */
    OpponentAbilityKnowledge knowledge = {
        .confirmed = 0,
        .possible1 = ABILITY_WATER_ABSORB,
        .possible2 = ABILITY_NONE,
    };

    int penalty = AI_PossibleImmunityPenalty(&knowledge, TYPE_WATER);

    ASSERT_TRUE(penalty < 0);
    ASSERT_TRUE(penalty > -10); /* nowhere near Basic_CheckForImmunity's -10/-12 */
}
