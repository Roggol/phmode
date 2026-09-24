# Test-Only Changes

This file tracks changes made purely to make manual testing easier - they are
**not** part of the intended final game and should be reverted before release.
Unlike `CHANGES.md` and `Changelog-prettier.md`, nothing here describes a real
design decision; it's a checklist of exactly what to undo.

---

## Early wild Altaria and Pinsir on Route 202

To make it easy to test the **Cotton Down** ability (new, on Altaria) and the
**Hone Claws** move (new, learned by Pinsir at level 13) without playing deep
into the game first, two of Route 202's land encounter slots were swapped out
- specifically the two **20%-rate** slots (the highest in the table), so they
show up quickly:

* `res/field/encounters/encounters_route_202.json` - the first two
  `land_encounters` entries (previously a level 3 Shinx and a level 3 Bidoof,
  each 20%) now read:
  * Level 45 Pinsir (already knows Hone Claws from level 13, and Megahorn
    from level 42)
  * Level 55 Altaria (Cotton Down is passive and needs no level; also already
    knows its new Wing Attack/Dragon Claw/Outrage learnset from this pass)
  The displaced Shinx and Bidoof were moved down into the two 1%-rate slots
  at the end of the list (previously a level 2 Starly and a level 2 Bidoof)
  rather than dropped, so the route's normal species pool is otherwise intact.

**To revert:** put slots back in their original order/rates - level 3 Shinx
and level 3 Bidoof in the first two (20%) slots, and level 2 Starly / level 2
Bidoof in the last two (1%) slots.

---

## Twinleaf Town guitarist hands out 99 Master Balls

The first time you talk to the guitarist NPC in Twinleaf Town
(`TwinleafTown_Guitarist` in `res/field/scripts/scripts_twinleaf_town.s`), he
now gives you **99 Master Balls** instead of his normal dialogue, to make it
easy to test-catch the Route 202 Altaria/Pinsir above (or anything else)
without fighting the Pokéball economy while testing. Every conversation after
the first goes back to his normal line.

* `res/field/scripts/scripts_twinleaf_town.s` - `TwinleafTown_Guitarist` now
  checks a new flag before falling through to the renamed
  `TwinleafTown_GuitaristDefaultDialogue` (previously the tail end of
  `TwinleafTown_Guitarist` itself); on a false read it gives the Master
  Balls (guarded by `GoToIfCannotFitItem`, matching how Route 201's Rare
  Candy gift is guarded) and sets the flag so it only happens once.
* `res/text/twinleaf_town.json` - new `TwinleafTown_Text_GuitaristMasterBallGift`
  entry (placeholder text, per the no-AI-writing policy - irrelevant here
  since this whole feature is getting reverted anyway).
* `generated/vars_flags.txt` - `FLAG_UNUSED_0x0114` renamed to
  `FLAG_RECEIVED_TWINLEAF_TOWN_GUITARIST_MASTER_BALLS` (this is literally the
  same slot freed up the last time this same kind of test scaffolding was
  removed - see the "Leftover testing scaffolding removed" entry in
  `CHANGES.md`).

**To revert:** delete the `GoToIfSet FLAG_RECEIVED_TWINLEAF_TOWN_GUITARIST_MASTER_BALLS
.../SetFlag .../Message TwinleafTown_Text_GuitaristMasterBallGift/Common_GiveItemQuantityNoLineFeed`
block, merge `TwinleafTown_GuitaristDefaultDialogue`'s body back into
`TwinleafTown_Guitarist` directly, remove the `TwinleafTown_Text_GuitaristMasterBallGift`
entry from `res/text/twinleaf_town.json`, and rename
`FLAG_RECEIVED_TWINLEAF_TOWN_GUITARIST_MASTER_BALLS` back to
`FLAG_UNUSED_0x0114` in `generated/vars_flags.txt`.

---

## New moves/abilities added this pass (for reference while testing)

Everything below is a real, permanent change already recorded in `CHANGES.md`
/ `Changelog-prettier.md` - listed here only as a checklist of what to go
verify in-battle while the test encounters above are in place:

* **Hone Claws** (new move) - Pinsir, level 13. Dark-type status, 3 PP,
  raises the user's Attack and Accuracy one stage each.
* **Cotton Down** (new ability) - Altaria, both ability slots. Whenever the
  holder is hit by a damaging move (contact not required), every other
  Pokémon on the field - allies included - has its Speed lowered one stage.
