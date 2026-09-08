#ifndef POKEPLATINUM_REGISTERED_ITEMS_H
#define POKEPLATINUM_REGISTERED_ITEMS_H

#include "vars_flags.h"

// phmode: the Y-button key-item list. Stored in VAR_REGISTERED_KEY_ITEM_0..5
// (renamed unused game vars) so the save layout is untouched. A slot holding
// ITEM_NONE (0) is empty; the list is kept packed from slot 0.
#define MAX_REGISTERED_KEY_ITEMS 6

u16 RegisteredKeyItems_GetSlot(VarsFlags *varsFlags, u32 slot);
u32 RegisteredKeyItems_Count(VarsFlags *varsFlags);
BOOL RegisteredKeyItems_IsRegistered(VarsFlags *varsFlags, u16 item);
void RegisteredKeyItems_Register(VarsFlags *varsFlags, u16 item);
BOOL RegisteredKeyItems_Unregister(VarsFlags *varsFlags, u16 item);

#endif // POKEPLATINUM_REGISTERED_ITEMS_H
