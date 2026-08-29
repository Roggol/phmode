#include "macros/btlcmd.inc"


_000:
    PlayBattleAnimation BTLSCR_PLAYER, BATTLE_ANIMATION_WEATHER_PSYCHIC_TERRAIN
    Wait
    // The terrain is weird!
    PrintMessage BattleStrings_Text_TheTerrainIsPsychic, TAG_NONE
    Wait
    WaitButtonABTime 30
    UpdateVar OPCODE_FLAG_ON, BTLVAR_FIELD_CONDITIONS, FIELD_CONDITION_PSYCHIC_TERRAIN
    End
