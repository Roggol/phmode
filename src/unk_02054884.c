#include "unk_02054884.h"

#include <nitro.h>
#include <string.h>

#include "constants/battle/condition.h"
#include "constants/field_poison.h"
#include "constants/heap.h"
#include "constants/items.h"

#include "overlay005/daycare.h"

#include "heap.h"
#include "party.h"
#include "pc_boxes.h"
#include "pokemon.h"
#include "save_catchrecords.h"
#include "save_player.h"
#include "savedata.h"
#include "savedata/save_table.h"
#include "trainer_info.h"
#include "unk_02017038.h"

BOOL Pokemon_CanBattle(Pokemon *mon)
{
    // this can be simplified further, but it won't match
    if (Pokemon_GetValue(mon, MON_DATA_HP, NULL) == 0) {
        return FALSE;
    }

    return !Pokemon_GetValue(mon, MON_DATA_IS_EGG, NULL);
}

// Adds a freshly built Pokemon (or egg) to the party, or, if the party is full,
// to the first PC box with a free slot. Gift scripts read the result to decide
// whether to announce "sent to a Box" and skip the nickname prompt. Returns
// GIVE_MON_RESULT_NO_ROOM (without taking ownership of anything) only when the
// party and every box are full.
int Pokemon_AddToPartyOrBox(SaveData *saveData, Pokemon *mon)
{
    if (Party_AddPokemon(SaveData_GetParty(saveData), mon)) {
        return GIVE_MON_RESULT_TO_PARTY;
    }

    if (PCBoxes_TryStoreBoxMon(SaveData_GetPCBoxes(saveData), Pokemon_GetBoxPokemon(mon))) {
        return GIVE_MON_RESULT_TO_BOX;
    }

    return GIVE_MON_RESULT_NO_ROOM;
}

int Pokemon_GiveMonFromScript(enum HeapID heapID, SaveData *saveData, u16 species, u8 level, u16 heldItem, int metLocation, int metTerrain)
{
    int result;
    Pokemon *mon;
    u32 item;
    TrainerInfo *trainerInfo = SaveData_GetTrainerInfo(saveData);

    mon = Pokemon_New(heapID);

    Pokemon_Init(mon);
    Pokemon_InitWith(mon, species, level, INIT_IVS_RANDOM, FALSE, 0, OTID_NOT_SET, 0);
    Pokemon_SetCatchData(mon, trainerInfo, ITEM_POKE_BALL, metLocation, metTerrain, heapID);

    item = heldItem;
    Pokemon_SetValue(mon, MON_DATA_HELD_ITEM, &item);

    result = Pokemon_AddToPartyOrBox(saveData, mon);

    if (result != GIVE_MON_RESULT_NO_ROOM) {
        SaveData_UpdateCatchRecords(saveData, mon);
    }

    Heap_Free(mon);

    return result;
}

BOOL sub_02054930(int unused, SaveData *saveData, u16 param2, u8 param3, int param4, int param5)
{
    int v0;
    BOOL result;
    TrainerInfo *trainerInfo = SaveData_GetTrainerInfo(saveData);
    Party *party = SaveData_GetParty(saveData);
    Pokemon *mon = Pokemon_New(HEAP_ID_FIELD3);

    Pokemon_Init(mon);

    v0 = SpecialMetLoc_GetId(param4, param5);
    Egg_CreateEgg(mon, param2, param3, trainerInfo, 4, v0);

    result = Party_AddPokemon(party, mon);
    Heap_Free(mon);

    return result;
}

void Party_ResetMonMoveSlot(Party *party, int partySlot, int moveSlot, u16 moveID)
{
    Pokemon_ResetMoveSlot(Party_GetPokemonBySlotIndex(party, partySlot), moveID, moveSlot);
}

// In many of the functions below, C99-style iterator declaration doesn't match

int Party_HasMonWithMove(Party *party, u16 moveID)
{
    int i;
    int partyCount = Party_GetCurrentCount(party);

    for (i = 0; i < partyCount; i++) {
        Pokemon *mon = Party_GetPokemonBySlotIndex(party, i);

        if (Pokemon_GetValue(mon, MON_DATA_IS_EGG, NULL) != FALSE) {
            continue;
        }

        if (Pokemon_GetValue(mon, MON_DATA_MOVE1, NULL) == moveID
            || Pokemon_GetValue(mon, MON_DATA_MOVE2, NULL) == moveID
            || Pokemon_GetValue(mon, MON_DATA_MOVE3, NULL) == moveID
            || Pokemon_GetValue(mon, MON_DATA_MOVE4, NULL) == moveID) {
            return i;
        }
    }

    return PARTY_SLOT_NONE;
}

int Party_AliveMonsCount(const Party *party)
{
    int i;
    int partyCount = Party_GetCurrentCount(party);
    int count = 0;

    for (i = 0; i < partyCount; i++) {
        if (Pokemon_CanBattle(Party_GetPokemonBySlotIndex(party, i))) {
            count++;
        }
    }

    return count;
}

Pokemon *Party_FindFirstEligibleBattler(const Party *party)
{
    int i;
    int partyCount = Party_GetCurrentCount(party);

    for (i = 0; i < partyCount; i++) {
        Pokemon *mon = Party_GetPokemonBySlotIndex(party, i);

        if (Pokemon_CanBattle(mon)) {
            return mon;
        }
    }

    GF_ASSERT(FALSE);
    return NULL;
}

Pokemon *Party_FindFirstHatchedMon(const Party *party)
{
    u16 i;
    u16 partyCount = Party_GetCurrentCount(party);

    for (i = 0; i < partyCount; i++) {
        Pokemon *mon = Party_GetPokemonBySlotIndex(party, i);

        if (Pokemon_GetValue(mon, MON_DATA_IS_EGG, NULL) == FALSE) {
            return mon;
        }
    }

    return NULL;
}

BOOL Party_HasTwoAliveMons(const Party *party)
{
    return Party_AliveMonsCount(party) >= 2;
}

void Party_GiveChampionRibbons(Party *party)
{
    int i;
    u8 championRibbon = TRUE;
    int partyCount = Party_GetCurrentCount(party);

    for (i = 0; i < partyCount; i++) {
        Pokemon *mon = Party_GetPokemonBySlotIndex(party, i);

        if (Pokemon_GetValue(mon, MON_DATA_IS_EGG, NULL) == FALSE) {
            Pokemon_SetValue(mon, MON_DATA_SINNOH_CHAMP_RIBBON, &championRibbon);
        }
    }
}

int Pokemon_DoPoisonDamage(Party *party, u16 mapLabelTextID)
{
    int numPoisoned = 0;
    int numFainted = 0;
    int i, partyCount = Party_GetCurrentCount(party);
    Pokemon *mon;

    for (i = 0; i < partyCount; i++) {
        mon = Party_GetPokemonBySlotIndex(party, i);

        if (Pokemon_CanBattle(mon)
            && (Pokemon_GetValue(mon, MON_DATA_STATUS, NULL) & (MON_CONDITION_TOXIC | MON_CONDITION_POISON))) {
            u32 hp = Pokemon_GetValue(mon, MON_DATA_HP, NULL);

            if (hp > 1) {
                hp--;
            }

            Pokemon_SetValue(mon, MON_DATA_HP, &hp);

            if (hp == 1) {
                numFainted++;
                Pokemon_UpdateFriendship(mon, FRIENDSHIP_EVENT_POISON_SURVIVE, mapLabelTextID);
            }

            numPoisoned++;
        }
    }

    if (numFainted) {
        return FLDPSN_FAINTED;
    } else if (numPoisoned) {
        return FLDPSN_POISONED;
    } else {
        return FLDPSN_NONE;
    }
}

BOOL Pokemon_TrySurvivePoison(Pokemon *mon)
{
    if (Pokemon_GetValue(mon, MON_DATA_STATUS, NULL) & (MON_CONDITION_TOXIC | MON_CONDITION_POISON)
        && Pokemon_GetValue(mon, MON_DATA_HP, NULL) == 1) {
        u32 condition = MON_CONDITION_NONE;

        Pokemon_SetValue(mon, MON_DATA_STATUS, &condition);
        return TRUE;
    }

    return FALSE;
}
