#include "macros/btlcmd.inc"


_000:
    PlayBattleAnimation BTLSCR_PLAYER, BATTLE_ANIMATION_WEATHER_DISTORTION_TERRAIN
    Wait
    // The terrain is distorted!
    PrintMessage BattleStrings_Text_TheTerrainIsDistorted, TAG_NONE
    Wait
    WaitButtonABTime 30
    UpdateVar OPCODE_FLAG_ON, BTLVAR_FIELD_CONDITIONS, FIELD_CONDITION_DISTORTION_TERRAIN
    End
