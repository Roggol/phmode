#include "../ai_model.h"
#include "../testing.h"

/*
 * Expert_StatusParalyze (script.s): scoring for a pure paralysis status move
 * like Thunder Wave, Stun Spore, or Glare.
 */

TEST(slower_attacker_knowing_thunder_wave_gets_a_strong_bonus)
{
    /* The requested scenario: the AI is slower than its target and knows
     * Thunder Wave. Paralysis halves the target's Speed, which is most
     * valuable to the slower side of the matchup, so a high random roll
     * (i.e. not landing in the ~7.8% miss chance) should score +3. */
    int roll = 255; /* comfortably above the 20/256 miss threshold */

    int score = AI_ScoreStatusParalyzeMove(SPEED_COMPARE_SLOWER, 100, roll);

    ASSERT_EQ(score, 3);
}

TEST(slower_attacker_can_still_roll_into_the_rare_miss_chance)
{
    /* Uncommon but real: ~7.8% of the time (roll < 20 out of 256) the bonus
     * doesn't apply even though the attacker is slower. */
    int roll = 19; /* just inside the miss threshold */

    int score = AI_ScoreStatusParalyzeMove(SPEED_COMPARE_SLOWER, 100, roll);

    ASSERT_EQ(score, 0);
}

TEST(slower_attacker_bonus_does_not_depend_on_its_own_hp)
{
    /* The HP check only applies on the "not slower" branch -- a slower
     * attacker at critically low HP should still get the same +3 shot. */
    int score = AI_ScoreStatusParalyzeMove(SPEED_COMPARE_SLOWER, 5, 255);

    ASSERT_EQ(score, 3);
}

TEST(faster_healthy_attacker_gets_no_bonus_or_penalty)
{
    int score = AI_ScoreStatusParalyzeMove(SPEED_COMPARE_FASTER, 100, 255);

    ASSERT_EQ(score, 0);
}

TEST(faster_hurt_attacker_is_penalized_for_wasting_the_turn)
{
    /* Above the speed check, being faster and badly hurt (<=70% HP) makes a
     * non-damaging status move a worse choice than attacking or switching. */
    int score = AI_ScoreStatusParalyzeMove(SPEED_COMPARE_FASTER, 50, 255);

    ASSERT_EQ(score, -1);
}

TEST(faster_attacker_right_at_the_seventy_percent_line_is_still_penalized)
{
    /* The real check is "greater than 70", so exactly 70% falls on the
     * penalized side, not the safe side. */
    int score = AI_ScoreStatusParalyzeMove(SPEED_COMPARE_FASTER, 70, 255);

    ASSERT_EQ(score, -1);
}

TEST(speed_tie_is_treated_the_same_as_faster)
{
    /* Only COMPARE_SPEED_SLOWER gets the paralysis-synergy bonus; a tied
     * matchup falls through to the same HP-based check a faster attacker gets. */
    int score = AI_ScoreStatusParalyzeMove(SPEED_COMPARE_TIE, 100, 255);

    ASSERT_EQ(score, 0);

    int hurtScore = AI_ScoreStatusParalyzeMove(SPEED_COMPARE_TIE, 50, 255);

    ASSERT_EQ(hurtScore, -1);
}

/*
 * Expert_SuckerPunch (script.s): scoring for Sucker Punch, which only works if the target
 * uses a damaging move this turn.
 */

TEST(sucker_punch_against_a_resisted_target_is_penalized_regardless_of_anything_else)
{
    /* Even with a favorable last-move signal and a winning roll, resisted/immune always
     * short-circuits to a flat -1 -- nothing else is checked. */
    int score = AI_ScoreSuckerPunch(EFFECTIVENESS_RESISTED, 1, 255);

    ASSERT_EQ(score, -1);
}

TEST(sucker_punch_against_an_immune_target_is_penalized_the_same_way)
{
    int score = AI_ScoreSuckerPunch(EFFECTIVENESS_IMMUNE, 1, 255);

    ASSERT_EQ(score, -1);
}

TEST(sucker_punch_gets_a_bonus_when_the_target_last_used_a_damaging_move)
{
    /* +2 for the last-move signal, plus the usual +1 from a winning roll. */
    int score = AI_ScoreSuckerPunch(EFFECTIVENESS_NORMAL_OR_SUPER, 1, 255);

    ASSERT_EQ(score, 3);
}

TEST(sucker_punch_does_not_get_the_last_move_bonus_after_a_status_move_or_switch_in)
{
    /* No last-move signal (a status move, or nothing yet on the very first turn) -- just the
     * usual +1 from a winning roll. */
    int score = AI_ScoreSuckerPunch(EFFECTIVENESS_NORMAL_OR_SUPER, 0, 255);

    ASSERT_EQ(score, 1);
}

TEST(sucker_punch_last_move_bonus_still_applies_even_on_a_losing_roll)
{
    /* The two bonuses are independent -- a missed 75% roll doesn't cancel the +2. */
    int score = AI_ScoreSuckerPunch(EFFECTIVENESS_NORMAL_OR_SUPER, 1, 0);

    ASSERT_EQ(score, 2);
}

TEST(sucker_punch_scores_zero_with_no_signal_and_a_losing_roll)
{
    int score = AI_ScoreSuckerPunch(EFFECTIVENESS_NORMAL_OR_SUPER, 0, 0);

    ASSERT_EQ(score, 0);
}
