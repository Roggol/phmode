#include "macros/btlcmd.inc"

    .data

_000:
    UpdateVar OPCODE_SET, BTLVAR_MSG_MOVE_TEMP, MOVE_SPIDER_WEB
    PlayMoveAnimation BTLSCR_MSG_TEMP
    Wait 
    UpdateVar OPCODE_SET, BTLVAR_MOVE_EFFECT_CHANCE, 0
    UpdateVar OPCODE_FLAG_OFF, BTLVAR_BATTLE_CTX_STATUS, SYSCTL_PLAYED_MOVE_ANIMATION
    // Your side of the field is sticky with webs!
    PrintMessage BattleStrings_Text_YourSideStickyWeb, TAG_NICKNAME, BTLSCR_DEFENDER
    Wait 
    WaitButtonABTime 30
    UpdateVar OPCODE_FLAG_ON, BTLVAR_FIELD_CONDITIONS, FIELD_CONDITION_STICKY_WEB
    CheckAbility CHECK_HAVE, BTLSCR_SWITCHED_MON, ABILITY_MAGIC_GUARD, _037
    CheckGrounded BTLSCR_SWITCHED_MON, _037

_019:
    CheckStickyWeb BTLSCR_SWITCHED_MON, _037
    UpdateVarFromVar OPCODE_SET, BTLVAR_MSG_BATTLER_TEMP, BTLVAR_SWITCHED_MON
    UpdateVar OPCODE_FLAG_ON, BTLVAR_BATTLE_CTX_STATUS, SYSCTL_SKIP_SPRITE_BLINK
    Call BATTLE_SUBSCRIPT_UPDATE_STAT_STAGE
    // {0} has its speed lowered by the sticky web!
    PrintMessage BattleStrings_Text_PokemonIsSlowedByStickyWeb_Ally, TAG_NICKNAME, BTLSCR_SWITCHED_MON
    Wait 
    WaitButtonABTime 30

_037:
    End