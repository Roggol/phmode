#include "macros/btlcmd.inc"


_000:
    // {0}’s {1} scattered Spikes! (the layer is added in battle_lib.c on switch-in)
    PrintMessage BattleStrings_Text_PokemonsAbilityScatteredSpikes_Ally, TAG_NICKNAME_ABILITY, BTLSCR_MSG_TEMP, BTLSCR_MSG_BATTLER_TEMP
    Wait
    WaitButtonABTime 30
    End
