#include "macros/btlcmd.inc"


_000:
    UpdateVar OPCODE_SET, BTLVAR_MSG_MOVE_TEMP, MOVE_SPIDER_WEB
    PlayMoveAnimation BTLSCR_MSG_TEMP
    Wait
    UpdateVar OPCODE_SET, BTLVAR_MOVE_EFFECT_CHANCE, 0
    UpdateVar OPCODE_FLAG_OFF, BTLVAR_BATTLE_CTX_STATUS, SYSCTL_PLAYED_MOVE_ANIMATION
    // Your side of the field is sticky with webs!
    PrintMessage BattleStrings_Text_YourSideStickyWeb, TAG_NONE
    Wait
    WaitButtonABTime 30
    UpdateVar OPCODE_SET, BTLVAR_BATTLER_SPEED_TEMP, 0

_030:
    GetMonBySpeedOrder BTLVAR_MSG_BATTLER_TEMP
    CheckAbility CHECK_HAVE, BTLSCR_MSG_BATTLER_TEMP, ABILITY_MAGIC_GUARD, _070
    CheckGrounded BTLSCR_MSG_BATTLER_TEMP, _070
    CheckStickyWeb BTLSCR_MSG_BATTLER_TEMP, _070
    UpdateVar OPCODE_FLAG_ON, BTLVAR_BATTLE_CTX_STATUS, SYSCTL_SKIP_SPRITE_BLINK
    // {0} was caught in a sticky web! (message is buffered by ChangeStatStage)
    Call BATTLE_SUBSCRIPT_UPDATE_STAT_STAGE

_070:
    UpdateVar OPCODE_ADD, BTLVAR_BATTLER_SPEED_TEMP, 1
    GoToIfValidMon BTLVAR_BATTLER_SPEED_TEMP, _030
    End
