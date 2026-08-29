#include "macros/btlcmd.inc"


_000:
    CheckElectricTerrain BTLSCR_SWITCHED_MON, _005
    CheckGrounded BTLSCR_SWITCHED_MON, _005
    SetHealthBoxStatusIcon BTLSCR_SWITCHED_MON, BATTLE_ANIMATION_NONE
    UpdateMonData OPCODE_FLAG_OFF, BTLSCR_SWITCHED_MON, BATTLEMON_STATUS, MON_CONDITION_SLEEP
    UpdateMonData OPCODE_FLAG_OFF, BTLSCR_SWITCHED_MON, BATTLEMON_VOLATILE_STATUS, VOLATILE_CONDITION_NIGHTMARE
    // {0} woke up!
    PrintMessage BattleStrings_Text_PokemonWokeUp_Ally, TAG_NICKNAME, BTLSCR_SWITCHED_MON
    Wait
    WaitButtonABTime 30

_005:
    CheckAbility CHECK_HAVE, BTLSCR_SWITCHED_MON, ABILITY_MAGIC_GUARD, _105
    CheckGrounded BTLSCR_SWITCHED_MON, _085

_010:
    CheckStickyWeb BTLSCR_SWITCHED_MON, _037
    UpdateVarFromVar OPCODE_SET, BTLVAR_MSG_BATTLER_TEMP, BTLVAR_SWITCHED_MON
    UpdateVar OPCODE_FLAG_ON, BTLVAR_BATTLE_CTX_STATUS, SYSCTL_SKIP_SPRITE_BLINK
    Call BATTLE_SUBSCRIPT_UPDATE_STAT_STAGE
    // {0} has its speed lowered by the sticky web!
    PrintMessage BattleStrings_Text_PokemonIsSlowedByStickyWeb_Ally, TAG_NICKNAME, BTLSCR_SWITCHED_MON
    Wait 
    WaitButtonABTime 30
    GoTo _037

_037:
    CheckToxicSpikes BTLSCR_SWITCHED_MON, _065
    CompareVarToValue OPCODE_EQU, BTLVAR_CALC_TEMP, 0x00000002, _063
    CompareVarToValue OPCODE_EQU, BTLVAR_CALC_TEMP, 0x00000001, _059
    // The poison spikes disappeared from around your team’s feet!
    PrintMessage BattleStrings_Text_ThePoisonSpikesDisappearedFromAroundYourTeamsFeet, TAG_NONE_SIDE_CONSCIOUS, BTLSCR_SWITCHED_MON
    Wait 
    WaitButtonABTime 30
    GoTo _065

_059:
    Call BATTLE_SUBSCRIPT_POISON
    GoTo _065

_063:
    Call BATTLE_SUBSCRIPT_BADLY_POISON

_065:
    CheckSpikes BTLSCR_SWITCHED_MON, _085
    UpdateVarFromVar OPCODE_SET, BTLVAR_MSG_BATTLER_TEMP, BTLVAR_SWITCHED_MON
    UpdateVar OPCODE_FLAG_ON, BTLVAR_BATTLE_CTX_STATUS, SYSCTL_SKIP_SPRITE_BLINK
    Call BATTLE_SUBSCRIPT_UPDATE_HP
    // {0} is hurt by the spikes!
    PrintMessage BattleStrings_Text_PokemonIsHurtByTheSpikes_Ally, TAG_NICKNAME, BTLSCR_SWITCHED_MON
    Wait 
    WaitButtonABTime 30

_085:
    CheckStealthRock BTLSCR_SWITCHED_MON, _105
    UpdateVarFromVar OPCODE_SET, BTLVAR_MSG_BATTLER_TEMP, BTLVAR_SWITCHED_MON
    UpdateVar OPCODE_FLAG_ON, BTLVAR_BATTLE_CTX_STATUS, SYSCTL_SKIP_SPRITE_BLINK
    Call BATTLE_SUBSCRIPT_UPDATE_HP
    // Pointed stones dug into {0}!
    PrintMessage BattleStrings_Text_PointedStonesDugIntoPokemon_Ally, TAG_NICKNAME, BTLSCR_SWITCHED_MON
    Wait 
    WaitButtonABTime 30

_105:
    End 
