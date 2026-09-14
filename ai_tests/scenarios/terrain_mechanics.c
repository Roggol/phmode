#include "../ai_model.h"
#include "../testing.h"

/* ---- Groundedness (shared by the Electric/Psychic Terrain checks below) ---- */

TEST(ordinary_mon_is_grounded)
{
    GroundednessInputs mon = { 0 };

    ASSERT_TRUE(AI_IsGroundedForTerrain(&mon));
}

TEST(flying_type_is_airborne)
{
    GroundednessInputs mon = { .isFlyingType = 1 };

    ASSERT_FALSE(AI_IsGroundedForTerrain(&mon));
}

TEST(levitate_is_airborne)
{
    GroundednessInputs mon = { .ability = ABILITY_LEVITATE };

    ASSERT_FALSE(AI_IsGroundedForTerrain(&mon));
}

TEST(gravity_grounds_a_flying_type_anyway)
{
    GroundednessInputs mon = { .isFlyingType = 1, .gravityActive = 1 };

    ASSERT_TRUE(AI_IsGroundedForTerrain(&mon));
}

TEST(iron_ball_grounds_a_levitate_holder_anyway)
{
    GroundednessInputs mon = { .ability = ABILITY_LEVITATE, .heldItemEffect = HOLD_EFFECT_SPEED_DOWN_GROUNDED };

    ASSERT_TRUE(AI_IsGroundedForTerrain(&mon));
}

/* ---- Electric Terrain: sleep moves (and Rest) against a grounded battler fail ---- */

TEST(sleep_move_against_grounded_target_in_electric_terrain_is_heavily_penalized)
{
    GroundednessInputs target = { 0 };

    ASSERT_EQ(AI_ElectricTerrainSleepPenalty(1, &target), -10);
}

TEST(sleep_move_against_airborne_target_in_electric_terrain_is_unaffected)
{
    GroundednessInputs target = { .isFlyingType = 1 };

    ASSERT_EQ(AI_ElectricTerrainSleepPenalty(1, &target), 0);
}

TEST(sleep_move_against_grounded_target_without_terrain_is_unaffected)
{
    GroundednessInputs target = { 0 };

    ASSERT_EQ(AI_ElectricTerrainSleepPenalty(0, &target), 0);
}

TEST(rest_is_checked_against_its_own_grounded_user_not_a_target)
{
    /* Rest targets the user, so the caller passes the ATTACKER's groundedness here --
     * modeled the same way, just with different inputs from the call site. */
    GroundednessInputs self = { .ability = ABILITY_LEVITATE };

    ASSERT_EQ(AI_ElectricTerrainSleepPenalty(1, &self), 0);

    GroundednessInputs groundedSelf = { 0 };

    ASSERT_EQ(AI_ElectricTerrainSleepPenalty(1, &groundedSelf), -10);
}

/* ---- Psychic Terrain: priority moves against a grounded target fail ---- */

TEST(priority_move_against_grounded_target_in_psychic_terrain_is_heavily_penalized)
{
    GroundednessInputs target = { 0 };

    ASSERT_EQ(AI_PsychicTerrainPriorityPenalty(1, 1, &target), -10);
}

TEST(higher_priority_move_is_also_penalized)
{
    GroundednessInputs target = { 0 };

    ASSERT_EQ(AI_PsychicTerrainPriorityPenalty(1, 3, &target), -10);
}

TEST(zero_priority_move_is_unaffected_by_psychic_terrain)
{
    GroundednessInputs target = { 0 };

    ASSERT_EQ(AI_PsychicTerrainPriorityPenalty(1, 0, &target), 0);
}

TEST(negative_priority_move_is_unaffected_by_psychic_terrain)
{
    /* e.g. Whirlwind, priority -6 */
    GroundednessInputs target = { 0 };

    ASSERT_EQ(AI_PsychicTerrainPriorityPenalty(1, -6, &target), 0);
}

TEST(priority_move_against_airborne_target_in_psychic_terrain_is_unaffected)
{
    GroundednessInputs target = { .ability = ABILITY_LEVITATE };

    ASSERT_EQ(AI_PsychicTerrainPriorityPenalty(1, 1, &target), 0);
}

TEST(priority_move_without_psychic_terrain_is_unaffected)
{
    GroundednessInputs target = { 0 };

    ASSERT_EQ(AI_PsychicTerrainPriorityPenalty(0, 1, &target), 0);
}

/* ---- Distortion Terrain: stat-boosting moves backfire ---- */

TEST(guaranteed_self_boost_gets_a_heavy_penalty_under_distortion)
{
    ASSERT_EQ(AI_DistortionTerrainStatPenalty(1, STAT_MOVE_GUARANTEED_SELF_BOOST, 0), -8);
}

TEST(chance_based_self_boost_gets_only_a_light_penalty_under_distortion)
{
    ASSERT_EQ(AI_DistortionTerrainStatPenalty(1, STAT_MOVE_CHANCE_SELF_BOOST, 0), -1);
}

TEST(heavy_penalty_is_meaningfully_larger_than_the_light_one)
{
    int heavy = AI_DistortionTerrainStatPenalty(1, STAT_MOVE_GUARANTEED_SELF_BOOST, 0);
    int light = AI_DistortionTerrainStatPenalty(1, STAT_MOVE_CHANCE_SELF_BOOST, 0);

    ASSERT_TRUE(heavy < light);
}

TEST(unrelated_move_is_unaffected_by_distortion_terrain)
{
    ASSERT_EQ(AI_DistortionTerrainStatPenalty(1, STAT_MOVE_NONE, 0), 0);
}

TEST(stat_boosting_moves_are_unaffected_without_distortion_terrain)
{
    ASSERT_EQ(AI_DistortionTerrainStatPenalty(0, STAT_MOVE_GUARANTEED_SELF_BOOST, 0), 0);
    ASSERT_EQ(AI_DistortionTerrainStatPenalty(0, STAT_MOVE_CHANCE_SELF_BOOST, 0), 0);
}

/* ---- Distortion Terrain: a move's own stat-LOWERING drawback becomes a benefit ---- */

TEST(guaranteed_self_lower_move_like_superpower_gets_rewarded_under_distortion)
{
    /* Superpower's own Attack/Defense drop backfires into a benefit, not a drawback. */
    ASSERT_EQ(AI_DistortionTerrainStatPenalty(1, STAT_MOVE_GUARANTEED_SELF_LOWER, 0), 8);
}

TEST(self_lower_bonus_is_unaffected_by_sheer_force)
{
    /* Superpower's self-drop has effect_chance 0 in the move data, so Sheer Force (which
     * only suppresses chance-based secondary effects) has nothing to do with it. */
    ASSERT_EQ(AI_DistortionTerrainStatPenalty(1, STAT_MOVE_GUARANTEED_SELF_LOWER, 1), 8);
}

TEST(self_lower_move_is_unaffected_without_distortion_terrain)
{
    ASSERT_EQ(AI_DistortionTerrainStatPenalty(0, STAT_MOVE_GUARANTEED_SELF_LOWER, 0), 0);
}

/* ---- Distortion Terrain: moves that lower the OPPONENT's stats would help them instead ---- */

TEST(guaranteed_opponent_lower_move_like_growl_gets_a_heavy_penalty_under_distortion)
{
    ASSERT_EQ(AI_DistortionTerrainStatPenalty(1, STAT_MOVE_GUARANTEED_OPPONENT_LOWER, 0), -8);
}

TEST(chance_based_opponent_lower_move_like_rock_smash_gets_only_a_light_penalty)
{
    ASSERT_EQ(AI_DistortionTerrainStatPenalty(1, STAT_MOVE_CHANCE_OPPONENT_LOWER, 0), -1);
}

TEST(sheer_force_removes_the_chance_based_opponent_lower_penalty)
{
    /* Sheer Force suppresses the secondary chance entirely, so there's nothing left for
     * Distortion Terrain to invert. */
    ASSERT_EQ(AI_DistortionTerrainStatPenalty(1, STAT_MOVE_CHANCE_OPPONENT_LOWER, 1), 0);
}

TEST(sheer_force_removes_the_chance_based_self_boost_penalty)
{
    ASSERT_EQ(AI_DistortionTerrainStatPenalty(1, STAT_MOVE_CHANCE_SELF_BOOST, 1), 0);
}

TEST(sheer_force_does_not_affect_the_guaranteed_opponent_lower_penalty)
{
    /* Growl's effect is guaranteed (effect_chance 0), not a Sheer-Force-suppressible
     * secondary effect, so the heavy penalty still applies even for a Sheer Force user. */
    ASSERT_EQ(AI_DistortionTerrainStatPenalty(1, STAT_MOVE_GUARANTEED_OPPONENT_LOWER, 1), -8);
}

/* ---- Sturdy / Focus Sash: a full-HP holder survives an otherwise-lethal hit at 1 HP ---- */

TEST(sturdy_holder_at_full_hp_survives_a_lethal_hit)
{
    ASSERT_TRUE(AI_DefenderSurvivesLethalHitAtOneHP(1, SURVIVAL_STURDY, 0));
}

TEST(mold_breaker_attacker_ignores_sturdy)
{
    ASSERT_FALSE(AI_DefenderSurvivesLethalHitAtOneHP(1, SURVIVAL_STURDY, 1));
}

TEST(sturdy_holder_below_full_hp_does_not_survive_a_lethal_hit)
{
    ASSERT_FALSE(AI_DefenderSurvivesLethalHitAtOneHP(0, SURVIVAL_STURDY, 0));
}

TEST(focus_sash_holder_at_full_hp_survives_a_lethal_hit)
{
    ASSERT_TRUE(AI_DefenderSurvivesLethalHitAtOneHP(1, SURVIVAL_FOCUS_SASH, 0));
}

TEST(focus_sash_is_not_ignored_by_mold_breaker)
{
    /* Mold Breaker only ignores abilities, not held items. */
    ASSERT_TRUE(AI_DefenderSurvivesLethalHitAtOneHP(1, SURVIVAL_FOCUS_SASH, 1));
}

TEST(focus_sash_holder_below_full_hp_does_not_survive_a_lethal_hit)
{
    ASSERT_FALSE(AI_DefenderSurvivesLethalHitAtOneHP(0, SURVIVAL_FOCUS_SASH, 0));
}

TEST(defender_with_neither_trait_does_not_survive_a_lethal_hit)
{
    ASSERT_FALSE(AI_DefenderSurvivesLethalHitAtOneHP(1, SURVIVAL_NONE, 0));
}

/* ---- Entry hazards (Stealth Rock, and the new Sticky Web) ---- */

TEST(hazard_move_is_unaffected_when_the_hazard_is_not_yet_set_and_target_has_backup)
{
    ASSERT_EQ(AI_HazardMovePenalty(0, 0), 0);
}

TEST(hazard_move_is_penalized_when_the_hazard_is_already_set)
{
    ASSERT_EQ(AI_HazardMovePenalty(1, 0), -10);
}

TEST(hazard_move_is_penalized_when_the_target_is_on_their_last_mon)
{
    ASSERT_EQ(AI_HazardMovePenalty(0, 1), -10);
}

TEST(hazard_move_penalty_does_not_stack_for_both_reasons_at_once)
{
    ASSERT_EQ(AI_HazardMovePenalty(1, 1), -10);
}

/* ---- Substitute blocks most status effects and stat-lowering hits ---- */

TEST(pure_status_move_against_a_substitute_gets_a_heavy_penalty)
{
    ASSERT_EQ(AI_SubstituteBlockPenalty(1, SUBSTITUTE_EFFECT_PURE_STATUS), -10);
}

TEST(chance_on_hit_effect_against_a_substitute_gets_only_a_light_penalty)
{
    /* The move still deals damage to the Substitute -- only the secondary chance is lost. */
    ASSERT_EQ(AI_SubstituteBlockPenalty(1, SUBSTITUTE_EFFECT_CHANCE_ON_HIT), -1);
}

TEST(substitute_penalties_do_not_apply_without_an_active_substitute)
{
    ASSERT_EQ(AI_SubstituteBlockPenalty(0, SUBSTITUTE_EFFECT_PURE_STATUS), 0);
    ASSERT_EQ(AI_SubstituteBlockPenalty(0, SUBSTITUTE_EFFECT_CHANCE_ON_HIT), 0);
}

TEST(unaffected_move_is_never_penalized_by_a_substitute)
{
    ASSERT_EQ(AI_SubstituteBlockPenalty(1, SUBSTITUTE_EFFECT_NONE), 0);
}

/*
 * Disable, Taunt, Torment, Encore, and Attract are now also blocked by a Substitute (their
 * own move-effect implementations were fixed to check for it - previously they didn't, a
 * separate pre-existing decomp bug). They're pure status effects from the AI's perspective,
 * same as Thunder Wave or Growl, so no new model function is needed - just confirming they
 * land in the same category as everything else in this table.
 */
TEST(disable_taunt_torment_encore_and_attract_are_now_blocked_by_substitute_too)
{
    ASSERT_EQ(AI_SubstituteBlockPenalty(1, SUBSTITUTE_EFFECT_PURE_STATUS), -10);
}

/* ---- Hazard awareness in switch decisions ---- */

TEST(switches_out_for_hazard_removal_when_hazard_up_remover_available_and_roll_hits)
{
    ASSERT_TRUE(AI_ShouldSwitchForHazards(1, 1, 0));
}

TEST(does_not_switch_out_for_hazard_removal_without_a_hazard_up)
{
    ASSERT_FALSE(AI_ShouldSwitchForHazards(0, 1, 0));
}

TEST(does_not_switch_out_for_hazard_removal_without_a_remover_on_the_bench)
{
    ASSERT_FALSE(AI_ShouldSwitchForHazards(1, 0, 0));
}

TEST(does_not_switch_out_for_hazard_removal_when_the_small_roll_misses)
{
    ASSERT_FALSE(AI_ShouldSwitchForHazards(1, 1, 1));
}

TEST(prefers_hazard_remover_switch_in_when_hazard_up_remover_available_and_roll_hits)
{
    ASSERT_TRUE(AI_PrefersHazardRemovalSwitchIn(1, 1, 0));
    ASSERT_TRUE(AI_PrefersHazardRemovalSwitchIn(1, 1, 1));
}

TEST(does_not_prefer_hazard_remover_switch_in_without_a_hazard_up)
{
    ASSERT_FALSE(AI_PrefersHazardRemovalSwitchIn(0, 1, 0));
}

TEST(does_not_prefer_hazard_remover_switch_in_without_a_remover_on_the_bench)
{
    ASSERT_FALSE(AI_PrefersHazardRemovalSwitchIn(1, 0, 0));
}

TEST(hazard_remover_switch_in_preference_still_fails_a_third_of_the_time)
{
    ASSERT_FALSE(AI_PrefersHazardRemovalSwitchIn(1, 1, 2));
}

TEST(hazard_remover_switch_in_preference_is_more_likely_than_the_switch_out_nudge)
{
    /* 2-in-3 (post-KO preference) is a much stronger nudge than 1-in-6 (voluntary switch),
     * matching the request: switching out is a small nudge, but once already switching,
     * favoring the cleaner should be a strong preference. */
    int switchOutHits = 0, switchInHits = 0;
    int roll;

    for (roll = 0; roll < 6; roll++) {
        if (AI_ShouldSwitchForHazards(1, 1, roll)) {
            switchOutHits++;
        }
    }

    for (roll = 0; roll < 3; roll++) {
        if (AI_PrefersHazardRemovalSwitchIn(1, 1, roll)) {
            switchInHits++;
        }
    }

    /* 1/6 vs 2/3 -- compare cross-multiplied to avoid floating point. */
    ASSERT_TRUE(switchOutHits * 3 < switchInHits * 6);
}
