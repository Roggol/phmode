#include "macros/btlcmd.inc"


_000:
    // Time Warp (Dialga on the field) removes the recharge turn entirely.
    GoToSubscriptIfAbilityOnField ABILITY_TIME_WARP, _020
    UpdateVar OPCODE_SET, BTLVAR_SIDE_EFFECT_FLAGS_INDIRECT, MOVE_SIDE_EFFECT_ON_HIT|MOVE_SUBSCRIPT_PTR_RECHARGE_TURN

_020:
    CalcCrit
    CalcDamage
    End
