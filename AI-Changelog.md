# AI Changelog — How Trainers Think, and What Changed

This is a player-facing guide to every change made to trainer decision-making
in this hack — both the trainers fighting *against* you and any AI-controlled
ally fighting *alongside* you (a partner trainer in a double battle). It's
written to be detailed enough that you can actually predict what a trainer
is likely to do in a given situation, not just "the AI got smarter."

To make that possible, a few entries below don't just describe the *change*
in isolation — they also lay out the **full surrounding decision** it lives
inside, so you can see exactly where the change fits and what else is going
on around it. Anywhere that happens, it's called out clearly so you know
what's actually new versus what's unchanged context included for clarity.

**Who this applies to:** every rule below runs on any AI-controlled Pokémon
— every enemy trainer, full stop, and (with one deliberate exception, noted
under Items) any partner trainer fighting on your side too. The AI doesn't
have a separate "easy mode" for allies; it makes the same calculations either
way.

---

## Choosing Which Move to Use

### Dry Skin is now actually recognized as a Water-move immunity (bug fix)

**Before this fix**, trainers never factored Dry Skin into their move choice
at all — not even when they'd correctly figured out (or already knew) that's
what your Pokémon had. A line of code that was supposed to check for Dry Skin
accidentally re-checked Levitate instead, in a spot where the ability could
never actually be Levitate — so the check silently never fired. A trainer
would throw a Water-type move at your Dry-Skin Pokémon exactly as readily as
if it had no relevant ability at all.

**Now fixed:** Dry Skin is checked correctly, as one entry in a full list of
abilities the AI evaluates before choosing a damaging move. Here's that full
list, so you know exactly what triggers a heavy "don't bother" reaction
(worth roughly -10 to -12 points against a move's pick score, which is a
severe penalty — for context, most situational bonuses/penalties elsewhere
in the AI are ±1 to ±3):

| Ability | What it blocks |
| --- | --- |
| Volt Absorb | Electric-type moves |
| Motor Drive | Electric-type moves |
| Water Absorb | Water-type moves |
| Dry Skin | Water-type moves |
| Flash Fire | Fire-type moves |
| Levitate | Ground-type moves |
| Wonder Guard | Any move that isn't super-effective |

**How the AI decides whether it "knows" your ability** — this part hasn't
changed, but matters a lot for predicting behavior: if your Pokémon's
ability has already been revealed this battle (an ability that's activated,
or one you used before), the trainer knows it for certain from then on. If
it hasn't been revealed and your Pokémon's species only has one possible
ability, the trainer treats that as effectively known too. But if your
Pokémon's species has **two** possible abilities and neither has been
revealed, the trainer **guesses** — a fresh 50/50 coin flip, separately,
*every single time* it evaluates a move. This means with an unrevealed
two-ability species, you should expect the trainer's behavior to be
genuinely inconsistent turn to turn: it might avoid a Water move on one turn
and throw it right at your Water-Absorb-possible Pokémon the next, because
it guessed differently each time. See the next entry for how unconfirmed
abilities are now handled a little more sensibly.

### Trainers now hedge slightly against abilities they can't confirm (new behavior)

This is a genuinely new rule, not a bug fix. Previously, once the coin-flip
guess above came up empty (i.e., it guessed an ability that doesn't grant an
immunity), the AI treated the move as completely safe with no further
thought — even though the *other*, un-guessed possible ability might still
have been the immunity-granting one all along.

Now, in that situation, the AI checks directly whether the ability it *could
still* have (the one it didn't guess) would grant an immunity to the move's
type, without ever assuming it does. If so, the move gets a small **-2**
penalty — noticeably smaller than the -10/-12 penalty above, since the AI
genuinely isn't sure. It's a nudge away from a risky pick, not a wall.

This applies to the same ability/type pairs as above, with one exception:

| Move type | Abilities that make the AI hedge |
| --- | --- |
| Water | Water Absorb, Dry Skin |
| Electric | Volt Absorb, Motor Drive |
| Fire | Flash Fire |
| Ground | Levitate |

**Wonder Guard is deliberately left out of this.** Its only vanilla holder,
Shedinja, only ever has one possible ability — so there's never an actual
"might be one of two" situation for it to apply to.

**A concrete example:** send out a Pokémon whose species could have either
Water Absorb or some unrelated ability, and don't let either ability show
itself yet. A trainer choosing between a Water-type move and something else
will now be a little less likely to pick the Water move — not because it
knows anything for sure, just because it's playing a little safer against an
outcome it can't rule out. If you later reveal that your Pokémon's ability
is actually the *other* one, this hedge disappears immediately and the
trainer stops caring.

### Metal Burst now checks for the right item (bug fix)

Metal Burst (and the AI's evaluation of whether an opponent is likely to use
it against *you*) needs to know whether a battler reliably moves last, since
that's the whole point of the move. The AI was supposed to check whether
either battler holds a **Lagging Tail** or **Full Incense** — both items
that force the holder to always move last — but a copy-paste mistake had it
checking for a **Shiny Stone** instead, an unrelated evolution item with
zero effect on turn order.

**Now fixed:** it correctly checks for that "always moves last" effect
(shared by Lagging Tail and Full Incense) as well as the Stall ability
(which does the same thing). Concretely: if either you or the trainer's
Pokémon holds a Lagging Tail/Full Incense or has Stall, that now correctly
factors into whether the trainer's Metal Burst usage makes sense — a Shiny
Stone in your bag or on your Pokémon has no effect on this whatsoever
(as it never should have).

### Weather-setting moves also value overriding an active terrain (partially new)

This hack added three new battlefield terrains (Electric, Psychic, and
Distortion) that work like an extra weather slot — see the main changelog for
what each one does. Trainers already valued using Rain Dance, Sunny Day, or
Hail to overwrite an *existing weather* they didn't like; that same
"overwriting is good" bonus now **also** applies when a terrain (rather than
a weather) is currently active. That's the only actual change here — but
since it's one clause inside a larger, mostly-unchanged decision, here's the
full picture for each move so you can predict the whole thing, not just the
new part:

**Rain Dance** — the trainer scores it more favorably when:
- It's slower than its target *and* has Swift Swim (an automatic bonus,
  skipping the rest of the checks below).
- The current weather is Hail, harsh Sun, or a Sandstorm, **or a terrain is
  active** *(the new part)* — all four are treated as "worth overwriting."
- It has Rain Dish, or is already statused and has Hydration (rain cures
  status for Hydration holders).

It's scored *less* favorably (a small penalty) if its HP is below 40% and
none of the above applied — using a turn on weather when you're that hurt
and not benefiting from it isn't a great use of the turn.

**Sunny Day** — the same shape of logic: bonus for the current weather being
Hail, Rain, or Sandstorm, **or a terrain being active** *(the new part)*;
bonus for Flower Gift, or for being statused with Leaf Guard (this specific
combination is itself a long-standing vanilla oddity being preserved as-is —
Leaf Guard doesn't actually cure existing status, so this bonus is a little
looser than it sounds); penalty if HP is below 40%.

**Hail** — bonus for the current weather being Sun, Rain, or Sandstorm, **or
a terrain being active** *(the new part)* — plus a further bonus on top if
it also knows Blizzard (since Blizzard becomes far more reliable in Hail),
and another bonus if it has Ice Body; penalty if HP is below 40%.

Practical upshot: if you've set up any of the three new terrains, expect
trainers who know Rain Dance, Sunny Day, or Hail to be just as eager to
"clear it out" with their own weather move as they already were against
ordinary weather — this used to be a gap where a terrain was effectively
invisible to that particular calculation.

### Trainers now react to what each terrain actually *does*, not just that one is up (new)

Everything above is about the AI valuing terrain as a thing to set or
overwrite. This entry is different: it's about the AI recognizing each
terrain's actual battlefield rule and adjusting its move choice accordingly,
so it doesn't pick a move that terrain would simply cancel out.

All three checks use the same idea of "grounded" — a Pokémon is treated as
grounded if Gravity is active, or it holds an Iron Ball, and otherwise as
airborne if it's a Flying type or has Levitate. (This doesn't check for
Magnet Rise, since the AI has no way to see that — a very minor gap in an
otherwise accurate check.)

**Electric Terrain — sleep moves against a grounded target.** Electric
Terrain keeps grounded Pokémon too alert to fall asleep. Trainers now treat
any move that tries to put a *grounded* target to sleep — Spore, Sleep
Powder, Hypnosis, Sing, Grass Whistle, Lovely Kiss, and Yawn — as a move
that will simply fail, applying the same **-10** penalty used for "this
move flatly won't work" elsewhere (an already-statused target, Safeguard,
Insomnia, etc.). The same logic applies to **Rest**: a grounded trainer
Pokémon won't be able to fall asleep to use it either, so Rest gets the
same treatment when *its own user* is grounded (Rest targets yourself, not
the opponent). None of this applies to an airborne target or user — a
Flying-type or Levitate Pokémon can still be put to sleep, or use Rest,
completely normally even with the terrain up.

**Psychic Terrain — priority moves against a grounded target.** Psychic
Terrain blocks any move with positive priority from hitting a grounded
opposing Pokémon at all. Trainers now apply the same -10 "won't work"
penalty to a priority move — Quick Attack, Extreme Speed, Fake Out, Sucker
Punch, Aqua Jet, Bullet Punch, Shadow Sneak, and so on — whenever the
*target* is grounded and Psychic Terrain is active. An airborne target is
unaffected, and priority moves remain perfectly normal choices against it.

**Distortion Terrain — stat-changing moves backfire.** Distortion Terrain
flips every stat-stage change on the field, for every Pokémon, grounded or
not. Trainers now split their reaction into two tiers:

- Any move that **guarantees** raising the user's own stat(s) — Swords
  Dance, Calm Mind, Bulk Up, Dragon Dance, Nasty Plot, Cosmic Power,
  Acupressure, Belly Drum, and the rest of the moves this hack already
  singled out for very low PP because of how strong a guaranteed boost is —
  gets a heavy **-8** penalty, full stop, regardless of the user's current
  stat stages. Under Distortion Terrain these moves guarantee a stat *drop*
  instead, so the AI treats them as actively bad rather than situationally
  good.
- A damaging move with only a **chance** to raise the user's stat on a hit
  — Metal Claw, Steel Wing, Ancient Power, Charge Beam — gets a much
  lighter **-1** penalty instead. These moves still do their normal job
  most of the time regardless of the stat chance, so the AI barely
  hesitates over them; it's a nudge, not a wall, and doesn't extend to
  their usual bonuses for the hit itself.

Practical upshot: don't expect a trainer to walk into your Distortion
Terrain and immediately Swords Dance — it now understands that would hurt
it, not help it. A Metal Claw user, on the other hand, will barely
hesitate, since the move's own damage is unaffected and the stat chance was
always a minor bonus either way.

**Distortion Terrain — the flip side: a move's own drawback can become a
benefit, and hurting the opponent's stats can backfire too.** The two-tier
split above also runs in reverse, covering three more cases:

- A move whose own drawback **guarantees** lowering the user's stat(s) —
  Superpower, Close Combat, Overheat, Draco Meteor, Psycho Boost, Hammer
  Arm — has that drawback flipped into a genuine **benefit** under
  Distortion Terrain. Trainers now give these a **+8** bonus on top of their
  normal damage scoring, specifically because the usual downside just isn't
  real right now.
- A status move that **guarantees** lowering the *target's* stat(s) — Growl,
  Leer, Tail Whip, String Shot, Sand Attack, Screech, Tickle, Captivate, and
  the rest — would actually raise them instead, directly helping the
  opponent. Trainers now treat this as a heavy **-8** mistake, the same
  weight as the self-boost case above.
- A damaging move with only a **chance** to lower the target's stat on a hit
  — Rock Smash, Crunch, Psychic, Shadow Ball, Acid, Iron Tail, and similar —
  gets a lighter **-1** nudge away, for the same reason the chance-based
  self-boost case only gets a light nudge: the move still does its normal
  job most of the time regardless.
- **Sheer Force exception:** a Sheer Force attacker's secondary chance-based
  effects are already completely suppressed (that's what Sheer Force does —
  it trades the chance away for extra power), so there's no chance left for
  Distortion Terrain to invert in the first place. Trainers correctly skip
  *both* chance-based penalties above for a Sheer Force user, but still
  apply the two *guaranteed* cases normally, since those aren't the kind of
  secondary chance Sheer Force affects.

Practical upshot: a hurt Close Combat or Superpower user isn't being
reckless under Distortion Terrain — the AI knows the self-drop won't
actually happen, so it leans into the move rather than away from it. And a
trainer that would normally Leer or Growl to soften you up will hold back,
because right now that move would soften *itself* up instead.

### The AI now evaluates Sticky Web the same way as other hazards (new)

Sticky Web (added earlier as a new move — see the main changelog) previously
had no dedicated AI evaluation at all, unlike Stealth Rock, Spikes, and Toxic
Spikes, which each get their own common-sense check. Trainers now apply the
same logic to it: a **-10** penalty if the target's side of the field
already has Sticky Web set (laying it again would do nothing), and a
separate **-10** if the target is down to their last Pokémon (there's no one
left for the web to matter against once this Pokémon faints). Previously a
trainer could waste a turn re-setting Sticky Web that was already active, or
setting it up against a team with nothing left to switch into it.

### Sticky Web's Speed drop was already fully accounted for (verified, no change needed)

Worth confirming directly, since it wasn't obvious from the outside: every
speed comparison the AI makes (deciding who moves first, whether a move
like Metal Burst or a paralysis move is a good idea, and so on) reads a
battler's *current* Speed stat stage directly — and Sticky Web's Speed drop
is applied through that exact same stat-stage system, the moment a grounded
Pokémon switches in on the webbed side. By the time the AI is choosing a
move for that Pokémon, the drop has already happened and is already baked
into every speed calculation the AI makes. There was nothing to fix here —
it already worked correctly, automatically, as a side effect of how stat
stages and speed comparisons are wired together generally.

### The AI now knows Sturdy and Focus Sash can save a target from a "lethal" hit (bug fix)

Every part of the AI that asks "would this move kill the target?" — used for
things like preferring a finishing blow, or deciding a move is safe to use
because the opponent won't get to retaliate — used to compare the target's
current HP against the predicted damage and nothing else. It had no idea
that a target at **full HP** with **Sturdy**, or holding a **Focus Sash**,
survives an otherwise-fatal hit with exactly 1 HP left instead of fainting
(the same rule as the modernized Sturdy behavior described in the main
changelog, and Focus Sash's usual effect). That meant a trainer could
misjudge a matchup as a guaranteed knockout when the target was actually
about to survive and hit back or switch out.

Trainers now check for this before treating any hit as lethal: the "survives
at 1 HP" exception applies only when the target is at **exactly full HP**,
and only for **Sturdy** (unless the attacker's ability is **Mold Breaker**,
which ignores it) or a held **Focus Sash**. **Focus Band** is deliberately
*not* covered by this — its "hang on" effect is a random per-hit chance
rather than something guaranteed, so the AI still treats a lethal hit against
a Focus Band holder as a genuine kill; it just won't always be right, the
same as a human player gambling on the same odds.

### Trainers now recognize when your Substitute blocks their move entirely (bug fix)

A Substitute intercepts almost every status effect and stat-lowering hit
aimed at whoever's hiding behind it — that part of the actual battle
mechanics was already correct and unchanged. The problem was purely on the
AI's side: it never checked for this at all, so a trainer would use Thunder
Wave, Toxic, Will-O-Wisp, Confuse Ray, Leech Seed, Growl, or similar moves
completely normally against a target it can plainly see is behind a
Substitute — wasting the turn on a move that was always going to do nothing.
This was double-checked against the actual battle mechanics before being
called a bug, not assumed.

Trainers now split their reaction the same way real damage is affected:

- A move whose **entire effect** is blocked — Thunder Wave, Toxic, Poison
  Powder, Will-O-Wisp, Confuse Ray, Leech Seed, Nightmare, Mean Look,
  Embargo, Heal Block, Gastro Acid, Growl, Leer, Screech, Captivate, and the
  rest of the moves that guarantee lowering your stats — gets the same heavy
  **-10** "this will just fail" treatment used for other guaranteed-failure
  cases, since the move genuinely accomplishes nothing against a
  Substitute-user.
- A damaging move with only a **chance** to lower your stat on a hit — Rock
  Smash, Crunch, Psychic, Shadow Ball, Acid, Iron Tail, and similar — still
  deals its normal damage to the Substitute itself, so only the bonus effect
  is lost. These get a much lighter **-1** nudge instead, the same weight as
  other "the chance-based part won't happen" cases elsewhere.

**Update: Disable, Taunt, Torment, Encore, and Attract are now covered too.**
The gap described above has been closed at the source — all five moves now
correctly fail against a Substitute (a genuine, pre-existing base-game bug,
now fixed), and trainers now apply the same -10 "this will just fail"
avoidance to them as every other status move in this section. Taunt in
particular had *no* AI scoring at all before this, for any reason — this is
the first time trainers give it any consideration whatsoever, even if only
this one narrow case.

### Trainers now weigh entry hazards on their own side when deciding whether — and where — to switch

Two related additions, both centered on the same trigger: Stealth Rock,
Spikes, Toxic Spikes, or Sticky Web already active on the trainer's own side
of the field.

**Voluntarily switching out to clear hazards.** A trainer with a benched
Pokemon that knows Defog or Rapid Spin now has a small, **1-in-6** chance
each turn to switch to it specifically to clear the hazards — but only as a
last-resort nudge. It's checked after every other switch reason, and after
the two "don't switch, we're winning" gates (already has a super-effective
move, or is heavily stat-boosted), so a trainer in a clearly good position
won't give that up just to go clear hazards. Think of it as a trainer who's
otherwise content to stay in occasionally deciding "well, since I don't have
a pressing reason not to, let's go clear these hazards while I have the
chance."

**Preferring a hazard-clearer whenever picking a replacement.** This is the
bigger, more frequently-relevant change: *any* time a trainer needs to pick
a switch-in without already having a specific one in mind — after a
Pokemon faints, after Perish Song forces a switch, after the "asleep with
Natural Cure" checks decide to switch without a clear target, or after the
voluntary hazard-switch above defers the actual pick — the same
selection routine runs. If the trainer's side has hazards up and a benched
Pokemon knows Defog or Rapid Spin, that Pokemon is now preferred **2 times
out of 3**, ahead of the usual type-matchup and damage-based picks. The
remaining 1-in-3 of the time (or if no such Pokemon exists), selection falls
back to the normal logic exactly as before.

**Practical upshot:** don't be surprised if a trainer brings in their Defog
or Rapid Spin user specifically when your side (or theirs) has hazards
piled up, even off a fainted Pokemon that had nothing to do with hazards in
particular. It's a genuine preference now, not just incidental type-matchup
luck.

### A documented switch-in scoring bug is now fixed (bug fix, mechanical accuracy)

While reading the switch-in selection code above, a bug that was already
known and documented (in this project's own `docs/bugs_and_glitches.md`, but
not actually applied here) was also fixed: when scoring benched Pokemon by
type matchup after a KO, a quad-effective matchup (4×, e.g. a pure Fighting
attacker against a Rock/Steel target) was being silently miscounted as if it
were worth much less than it should — an integer storage bug, not an AI
decision — occasionally causing the AI to undervalue its best possible
switch-in relative to a merely double-effective one. This has no bearing on
predicting AI *intent*, just on making sure the numbers it computes
internally are correct.

### Sucker Punch now reads a real clue about whether you'll attack again (new)

Sucker Punch only works if the target uses a damaging move that same turn —
if you use a status move or switch out, it just fails. The AI has no way to
see your pending choice for the current turn (confirmed while working on
this — it genuinely doesn't peek), so instead of guessing blindly, trainers
now look at something they *do* already know: **whether your last move dealt
damage.** If your last move was an attack (not a status move, and not "no
history yet" on the very first turn), a trainer considering Sucker Punch now
gets a solid **+2** bonus toward using it, on top of its existing baseline
75% chance of a smaller +1. If your last move was a status move — or this is
the very start of the battle — that +2 simply doesn't apply, and Sucker
Punch falls back to just the baseline chance.

This doesn't change when Sucker Punch is a *bad* idea: a target that
resists or is immune to it still gets a flat -1, no matter what your last
move was.

**Practical upshot:** if a trainer's Pokémon knows Sucker Punch and you've
been throwing out attacks, expect it to lean into using Sucker Punch more
readily. Break that pattern with a status move or a switch, and it goes back
to a coin-flip-ish choice for the trainer.

---

## Using Items (Enemy Trainers Only)

**Important context that hasn't changed:** an AI-controlled *partner* (an
ally fighting on your side) never uses items in battle, period — that's an
existing, deliberate restriction, not something this hack touched. Everything
in this section is about trainers fighting *against* you.

### Trainers could accidentally empty their entire item pool in one go (bug fix)

This is the headline fix, and it's worth understanding precisely because the
old behavior was easy to misread as "this trainer is out of items" when it
actually wasn't true in the way it looked.

When a trainer's turn comes up, the AI scans through its remembered list of
up to 4 held battle items (see below for when each slot is even considered)
looking for one it should use — critically low HP, a status condition it can
cure, a stat drop it should reverse, and so on. The moment it found **one**
item worth using, the game's bookkeeping was supposed to record just that one
item as spent. Instead, a bug meant that **every item slot after that one**
also got wiped from the trainer's remembered inventory in the same check —
even slots holding items whose own condition was never true that turn. Only
the *last* item scanned actually got correctly recorded as the one "used";
everything between the real pick and that last slot just silently vanished
from the trainer's future options.

Practically, this meant a trainer could look like it had permanently burned
through several items in a single turn, and behave for the rest of the fight
as if it had far fewer items left than its trainer data actually gave it —
with no visible cause, since only one item use is ever shown on screen.

**Now fixed:** exactly one item slot is used and recorded per decision, and
every other item slot is left completely untouched for future turns, no
matter how many of them happen to sit after the chosen one in the list.

### The rest of the item-use decision (unchanged, included for context)

None of this changed, but you need it to actually predict when a trainer
will reach for an item at all:

- **A Full Restore is used once HP drops below 25%** (and the Pokémon isn't
  already fainted).
- **Any other HP-restoring item** (Potions, Super Potions, Hyper Potions,
  etc.) is used if HP is below 25%, **or** if using it wouldn't fully heal
  the missing HP anyway — the second clause exists specifically so a trainer
  doesn't "waste" a big Hyper Potion healing a Pokémon that's only lightly
  scratched; a small Potion is fair game even above 25% HP as long as it
  wouldn't overheal.
- **Status-curing items** are used the instant the matching condition is
  present: sleep, poison/toxic, burn, freeze, paralysis, or confusion each
  have their own check, all independent of HP.
- **Stat-boosting items and Guard Spec.** are only even considered *after*
  the Pokémon's first turn on the field (so it won't burn one immediately
  on switch-in), and Guard Spec. specifically won't be used if Mist is
  already up on that side.

- **Not every item slot is available on every turn.** A trainer's *first*
  listed item can be considered at any time, but each item *after* that only
  becomes available once the trainer's own remaining Pokémon count drops low
  enough — later items in a trainer's kit are effectively held in reserve for
  when they're running out of Pokémon. With a full 4-item loadout, the
  thresholds work out to:

  | Item slot | Needs this many (or fewer) of the trainer's own Pokémon still alive |
  | --- | --- |
  | 1st | any time |
  | 2nd | 4 |
  | 3rd | 3 |
  | 4th (last) | 2 |

  So against a trainer with a full team and a full item kit, don't expect
  to see anything but their first item until their team starts thinning out
  — and their very last item might not show up at all until they're down to
  their final one or two Pokémon.

---

## Quick Reference

| Change | Type | Affects |
| --- | --- | --- |
| Dry Skin recognized as a Water immunity | Bug fix | Any AI battler |
| Small penalty for an unconfirmed-but-possible immunity ability | New | Any AI battler |
| Metal Burst checks Lagging Tail/Full Incense, not Shiny Stone | Bug fix | Any AI battler |
| Rain Dance/Sunny Day/Hail value overriding terrain, not just weather | New (small addition to existing logic) | Any AI battler |
| Electric Terrain: avoids sleep moves/Rest against a grounded (self-)target | New | Any AI battler |
| Psychic Terrain: avoids priority moves against a grounded target | New | Any AI battler |
| Distortion Terrain: heavy penalty for guaranteed self-stat-boosts, light penalty for chance-based ones | New | Any AI battler |
| Distortion Terrain: bonus for a move's own guaranteed self-stat-*lower*, penalty for lowering the opponent's stats (guaranteed or chance-based), both gated by Sheer Force | New | Any AI battler |
| Sticky Web now evaluated as a hazard move (already-set / target's-last-mon checks) | New | Any AI battler |
| Sticky Web's Speed drop already fully factored into every speed check | Verified, no change needed | Any AI battler |
| "Would this kill?" now accounts for Sturdy/Focus Sash surviving at 1 HP from full HP | Bug fix | Any AI battler |
| Trainers recognize a Substitute blocks status/stat-lowering moves entirely, including Disable/Taunt/Torment/Encore/Attract now that their own bug is also fixed | Bug fix | Any AI battler |
| Small (1-in-6) chance to voluntarily switch to a Defog/Rapid Spin user when own side has hazards up | New | Any AI battler |
| Switch-in selection prefers a Defog/Rapid Spin user 2-in-3 of the time when own side has hazards up | New | Any AI battler |
| Post-KO switch-in type-matchup scoring no longer overflows on a quad-effective matchup | Bug fix | Any AI battler |
| Sucker Punch gets a +2 bonus when the target's last move dealt damage | New | Any AI battler |
| Item use no longer wipes out every item after the one chosen | Bug fix | Enemy trainers only (partners never use items) |
