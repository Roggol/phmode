#include "registered_items.h"

#include <nitro.h>

#include "constants/items.h"
#include "constants/savedata/vars_flags.h"

#include "vars_flags.h"

static const u16 sRegisteredKeyItemVars[MAX_REGISTERED_KEY_ITEMS] = {
    VAR_REGISTERED_KEY_ITEM_0,
    VAR_REGISTERED_KEY_ITEM_1,
    VAR_REGISTERED_KEY_ITEM_2,
    VAR_REGISTERED_KEY_ITEM_3,
    VAR_REGISTERED_KEY_ITEM_4,
    VAR_REGISTERED_KEY_ITEM_5,
};

static u16 GetSlotValue(VarsFlags *varsFlags, u32 slot)
{
    u16 *var = VarsFlags_GetVarAddress(varsFlags, sRegisteredKeyItemVars[slot]);
    return (var != NULL) ? *var : ITEM_NONE;
}

static void SetSlotValue(VarsFlags *varsFlags, u32 slot, u16 item)
{
    u16 *var = VarsFlags_GetVarAddress(varsFlags, sRegisteredKeyItemVars[slot]);

    if (var != NULL) {
        *var = item;
    }
}

u16 RegisteredKeyItems_GetSlot(VarsFlags *varsFlags, u32 slot)
{
    if (slot >= MAX_REGISTERED_KEY_ITEMS) {
        return ITEM_NONE;
    }

    return GetSlotValue(varsFlags, slot);
}

u32 RegisteredKeyItems_Count(VarsFlags *varsFlags)
{
    u32 count = 0;

    for (u32 i = 0; i < MAX_REGISTERED_KEY_ITEMS; i++) {
        if (GetSlotValue(varsFlags, i) != ITEM_NONE) {
            count++;
        }
    }

    return count;
}

BOOL RegisteredKeyItems_IsRegistered(VarsFlags *varsFlags, u16 item)
{
    if (item == ITEM_NONE) {
        return FALSE;
    }

    for (u32 i = 0; i < MAX_REGISTERED_KEY_ITEMS; i++) {
        if (GetSlotValue(varsFlags, i) == item) {
            return TRUE;
        }
    }

    return FALSE;
}

void RegisteredKeyItems_Register(VarsFlags *varsFlags, u16 item)
{
    if (item == ITEM_NONE || RegisteredKeyItems_IsRegistered(varsFlags, item) == TRUE) {
        return;
    }

    for (u32 i = 0; i < MAX_REGISTERED_KEY_ITEMS; i++) {
        if (GetSlotValue(varsFlags, i) == ITEM_NONE) {
            SetSlotValue(varsFlags, i, item);
            return;
        }
    }
}

BOOL RegisteredKeyItems_Unregister(VarsFlags *varsFlags, u16 item)
{
    if (item == ITEM_NONE) {
        return FALSE;
    }

    for (u32 i = 0; i < MAX_REGISTERED_KEY_ITEMS; i++) {
        if (GetSlotValue(varsFlags, i) != item) {
            continue;
        }

        // Drop the entry and pull the rest of the list down so it stays packed.
        for (u32 j = i; j < MAX_REGISTERED_KEY_ITEMS - 1; j++) {
            SetSlotValue(varsFlags, j, GetSlotValue(varsFlags, j + 1));
        }

        SetSlotValue(varsFlags, MAX_REGISTERED_KEY_ITEMS - 1, ITEM_NONE);
        return TRUE;
    }

    return FALSE;
}
