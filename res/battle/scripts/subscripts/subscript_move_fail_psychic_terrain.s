#include "macros/btlcmd.inc"


_000:
    // {0} is protected by the psychic terrain!
    PrintMessage BattleStrings_Text_ProtectedByPsychicTerrain, TAG_NICKNAME, BTLSCR_MSG_BATTLER_TEMP
    Wait
    WaitButtonABTime 30
    UnlockMoveChoice BTLSCR_ATTACKER
    End
