#ifndef POKEPLATINUM_SCRCMD_SHOP_H
#define POKEPLATINUM_SCRCMD_SHOP_H

#include "field_script_context.h"

BOOL ScrCmd_PokeMartCommon(ScriptContext *ctx);
BOOL ScrCmd_PokeMartSpecialties(ScriptContext *ctx);
BOOL ScrCmd_PokeMartDecor(ScriptContext *ctx);
BOOL ScrCmd_PokeMartSeal(ScriptContext *ctx);
BOOL ScrCmd_ShowAccessoryShop(ScriptContext *ctx);
BOOL ScrCmd_CheckShopItemSoldOut(ScriptContext *ctx);
BOOL ScrCmd_RecordShopItemPurchase(ScriptContext *ctx);

#endif // POKEPLATINUM_SCRCMD_SHOP_H
