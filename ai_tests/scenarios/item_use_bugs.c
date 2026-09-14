#include "../ai_model.h"
#include "../testing.h"

/*
 * Regression tests for "the AI uses up all its items at once" -- a base-game
 * bug in TrainerAI_ShouldUseItem (src/battle/trainer_ai/trainer_ai.c). Its
 * item-selection loop never stopped once it found a usable item, and never
 * reset its shared "should use" flag between slots, so every item slot AFTER
 * the first match also got wiped out of the trainer's inventory -- even slots
 * whose own condition was never true. Fixed by breaking out of the loop the
 * moment one item is chosen, since only one item is ever used per turn.
 *
 * AI_ShouldUseItem_PreFixBuggy is a faithful reproduction of the original bug
 * (see ai_model.c) kept specifically so these tests can demonstrate it
 * happening, and prove the real AI_ShouldUseItem no longer does it.
 */

TEST(fixed_version_only_consumes_the_one_item_it_picks)
{
    /* Critically low HP: the Full Restore in slot 0 should be picked. Neither
     * of the other two items' own conditions are true (not burned, not
     * paralyzed) -- they should be left completely untouched. */
    BattleMon mon = { .curHP = 5, .maxHP = 100 };
    AIItem items[] = {
        { "Full Restore", ITEM_KIND_FULL_RESTORE, 0, 0 },
        { "Burn Heal", ITEM_KIND_HEAL_BURN, 0, 0 },
        { "Full Heal", ITEM_KIND_HEAL_PARALYSIS, 0, 0 },
    };

    AIItemDecision decision = AI_ShouldUseItem(&mon, items, 3);

    ASSERT_TRUE(decision.shouldUse);
    ASSERT_EQ(decision.slotIndex, 0);
    ASSERT_TRUE(items[0].used);
    ASSERT_FALSE(items[1].used);
    ASSERT_FALSE(items[2].used);
}

TEST(buggy_pre_fix_version_wipes_out_every_later_item_too)
{
    /* Same exact scenario as above. Under the original bug, finding the Full
     * Restore usable in slot 0 leaves its "should use" flag stuck TRUE for
     * every later slot, so the Burn Heal and Full Heal both get marked used
     * even though the mon is neither burned nor paralyzed. This is the bug
     * report in miniature: one qualifying item silently destroys every item
     * that comes after it in the list. */
    BattleMon mon = { .curHP = 5, .maxHP = 100 };
    AIItem items[] = {
        { "Full Restore", ITEM_KIND_FULL_RESTORE, 0, 0 },
        { "Burn Heal", ITEM_KIND_HEAL_BURN, 0, 0 },
        { "Full Heal", ITEM_KIND_HEAL_PARALYSIS, 0, 0 },
    };

    AI_ShouldUseItem_PreFixBuggy(&mon, items, 3);

    ASSERT_TRUE(items[0].used);  /* correctly used */
    ASSERT_TRUE(items[1].used);  /* bug: wiped despite the mon not being burned */
    ASSERT_TRUE(items[2].used);  /* bug: wiped despite the mon not being paralyzed */
}

TEST(buggy_version_reports_the_last_scanned_item_not_the_first)
{
    /* Beyond wiping every later slot, the buggy version's returned decision
     * also points at the LAST item scanned, not the one that actually
     * qualified -- a trainer could be reported as using its Full Heal when it
     * was really the earlier Full Restore that should have been used. */
    BattleMon mon = { .curHP = 5, .maxHP = 100 };
    AIItem items[] = {
        { "Full Restore", ITEM_KIND_FULL_RESTORE, 0, 0 },
        { "Full Heal", ITEM_KIND_HEAL_PARALYSIS, 0, 0 },
    };

    AIItemDecision decision = AI_ShouldUseItem_PreFixBuggy(&mon, items, 2);

    ASSERT_EQ(decision.slotIndex, 1);

    AIItem fixedItems[] = {
        { "Full Restore", ITEM_KIND_FULL_RESTORE, 0, 0 },
        { "Full Heal", ITEM_KIND_HEAL_PARALYSIS, 0, 0 },
    };
    AIItemDecision fixedDecision = AI_ShouldUseItem(&mon, fixedItems, 2);

    ASSERT_EQ(fixedDecision.slotIndex, 0);
}

TEST(a_slot_that_never_would_have_qualified_on_its_own_survives_the_fix)
{
    /* Full HP, no status -- nothing here should ever be used. Sanity check
     * that the fix doesn't accidentally use an item when nothing qualifies,
     * which the loop-without-a-break bug could never accidentally trigger
     * anyway since it needs at least one true condition to seed `result`. */
    BattleMon mon = { .curHP = 100, .maxHP = 100 };
    AIItem items[] = {
        { "Burn Heal", ITEM_KIND_HEAL_BURN, 0, 0 },
        { "Full Heal", ITEM_KIND_HEAL_PARALYSIS, 0, 0 },
    };

    AIItemDecision decision = AI_ShouldUseItem(&mon, items, 2);

    ASSERT_FALSE(decision.shouldUse);
    ASSERT_FALSE(items[0].used);
    ASSERT_FALSE(items[1].used);
}
