#include "macros/btlcmd.inc"


_000:
    PlayBattleAnimation BTLSCR_PLAYER, BATTLE_ANIMATION_WEATHER_ELECTRIC_TERRAIN
    Wait
    WaitButtonABTime 15
    // {0}’s {1} electrified the terrain!
    PrintMessage BattleStrings_Text_PokemonsAbilityChargedTheTerrain_Ally, TAG_NICKNAME_ABILITY, BTLSCR_MSG_TEMP, BTLSCR_MSG_BATTLER_TEMP
    Wait
    WaitButtonABTime 30
    UpdateVar OPCODE_FLAG_OFF, BTLVAR_FIELD_CONDITIONS, FIELD_CONDITION_ATMOSPHERE
    UpdateVar OPCODE_FLAG_ON, BTLVAR_FIELD_CONDITIONS, FIELD_CONDITION_ELECTRIC_TERRAIN
    End
