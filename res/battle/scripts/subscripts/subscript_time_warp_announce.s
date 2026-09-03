#include "macros/btlcmd.inc"


_000:
    // {0} is warping time around it!
    PrintMessage BattleStrings_Text_PokemonIsWarpingTime_Ally, TAG_NICKNAME, BTLSCR_MSG_BATTLER_TEMP
    Wait
    WaitButtonABTime 30
    End
