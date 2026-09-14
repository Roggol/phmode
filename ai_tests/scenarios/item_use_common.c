#include "../ai_model.h"
#include "../testing.h"

/* ---- Common scenarios: the everyday cases the AI has to get right ---- */

TEST(full_hp_mon_does_not_use_any_item)
{
    BattleMon mon = { .curHP = 100, .maxHP = 100 };
    AIItem items[] = {
        { "Full Restore", ITEM_KIND_FULL_RESTORE, 0, 0 },
        { "Full Heal", ITEM_KIND_HEAL_PARALYSIS, 0, 0 },
    };

    AIItemDecision decision = AI_ShouldUseItem(&mon, items, 2);

    ASSERT_FALSE(decision.shouldUse);
}

TEST(below_quarter_hp_triggers_full_restore)
{
    BattleMon mon = { .curHP = 20, .maxHP = 100 };
    AIItem items[] = {
        { "Full Restore", ITEM_KIND_FULL_RESTORE, 0, 0 },
    };

    AIItemDecision decision = AI_ShouldUseItem(&mon, items, 1);

    ASSERT_TRUE(decision.shouldUse);
    ASSERT_EQ(decision.slotIndex, 0);
    ASSERT_TRUE(items[0].used);
}

TEST(fainted_mon_does_not_trigger_full_restore)
{
    /* curHP == 0 is a fainted battler, not a low-HP one -- an item can't save it. */
    BattleMon mon = { .curHP = 0, .maxHP = 100 };
    AIItem items[] = {
        { "Full Restore", ITEM_KIND_FULL_RESTORE, 0, 0 },
    };

    AIItemDecision decision = AI_ShouldUseItem(&mon, items, 1);

    ASSERT_FALSE(decision.shouldUse);
}

TEST(sleeping_mon_uses_awakening_even_at_full_hp)
{
    BattleMon mon = { .curHP = 100, .maxHP = 100, .status = STATUS_SLEEP };
    AIItem items[] = {
        { "Awakening", ITEM_KIND_HEAL_SLEEP, 0, 0 },
    };

    AIItemDecision decision = AI_ShouldUseItem(&mon, items, 1);

    ASSERT_TRUE(decision.shouldUse);
    ASSERT_TRUE(items[0].used);
}

TEST(toxic_is_cured_by_an_antidote_style_item)
{
    BattleMon mon = { .curHP = 100, .maxHP = 100, .status = STATUS_TOXIC };
    AIItem items[] = {
        { "Antidote", ITEM_KIND_HEAL_POISON, 0, 0 },
    };

    AIItemDecision decision = AI_ShouldUseItem(&mon, items, 1);

    ASSERT_TRUE(decision.shouldUse);
}

TEST(confusion_is_cured_by_the_right_item)
{
    BattleMon mon = { .curHP = 100, .maxHP = 100, .confused = 1 };
    AIItem items[] = {
        { "Persim Berry (confusion)", ITEM_KIND_HEAL_CONFUSION, 0, 0 },
    };

    AIItemDecision decision = AI_ShouldUseItem(&mon, items, 1);

    ASSERT_TRUE(decision.shouldUse);
}

/* ---- Less obvious scenarios: don't waste a big potion on chip damage ---- */

TEST(big_potion_is_not_wasted_on_small_chip_damage)
{
    /* 90/100 HP -- only missing 10, well above the 25% critical line. A Hyper
     * Potion (restores 200) would fully heal with tons to spare, so the AI
     * should hold onto it rather than "waste" it here. */
    BattleMon mon = { .curHP = 90, .maxHP = 100 };
    AIItem items[] = {
        { "Hyper Potion", ITEM_KIND_HP_RESTORE, 200, 0 },
    };

    AIItemDecision decision = AI_ShouldUseItem(&mon, items, 1);

    ASSERT_FALSE(decision.shouldUse);
}

TEST(small_potion_is_used_even_above_quarter_hp_if_it_would_not_fully_heal)
{
    /* 60/100 HP -- above the 25% critical line, but missing 40 HP. A plain
     * Potion (+20) wouldn't fully heal that, so using it now wastes nothing,
     * and the AI should go ahead and use it rather than holding out for later. */
    BattleMon mon = { .curHP = 60, .maxHP = 100 };
    AIItem items[] = {
        { "Potion", ITEM_KIND_HP_RESTORE, 20, 0 },
    };

    AIItemDecision decision = AI_ShouldUseItem(&mon, items, 1);

    ASSERT_TRUE(decision.shouldUse);
}

TEST(item_used_from_a_later_slot_when_earlier_slots_do_not_apply)
{
    BattleMon mon = { .curHP = 100, .maxHP = 100, .status = STATUS_BURN };
    AIItem items[] = {
        { "Full Restore", ITEM_KIND_FULL_RESTORE, 0, 0 }, /* full HP: doesn't apply */
        { "Burn Heal", ITEM_KIND_HEAL_BURN, 0, 0 },       /* burned: applies */
    };

    AIItemDecision decision = AI_ShouldUseItem(&mon, items, 2);

    ASSERT_TRUE(decision.shouldUse);
    ASSERT_EQ(decision.slotIndex, 1);
    ASSERT_FALSE(items[0].used);
    ASSERT_TRUE(items[1].used);
}

TEST(empty_item_slots_are_skipped)
{
    BattleMon mon = { .curHP = 20, .maxHP = 100 };
    AIItem items[] = {
        { "(empty)", ITEM_KIND_NONE, 0, 0 },
        { "Full Restore", ITEM_KIND_FULL_RESTORE, 0, 0 },
    };

    AIItemDecision decision = AI_ShouldUseItem(&mon, items, 2);

    ASSERT_TRUE(decision.shouldUse);
    ASSERT_EQ(decision.slotIndex, 1);
}
