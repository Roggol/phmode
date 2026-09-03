#include "macros/btlcmd.inc"


_000:
    // Space Warp (Palkia) reuses this subscript only for the "knock airborne
    // Pokemon down" loop below: gravity is already permanently active, so skip
    // the announcement and the turn-counter setup.
    CompareVarToValue OPCODE_FLAG_SET, BTLVAR_FIELD_CONDITIONS, FIELD_CONDITION_GRAVITY_PERM, _009
    // Gravity intensified!
    PrintMessage BattleStrings_Text_GravityIntensified, TAG_NONE
    Wait
    WaitButtonABTime 30
    UpdateVar OPCODE_FLAG_ON, BTLVAR_FIELD_CONDITIONS, FIELD_CONDITION_GRAVITY_INIT

_009:
    UpdateVar OPCODE_SET, BTLVAR_BATTLER_SPEED_TEMP, 0

_013:
    GetMonBySpeedOrder BTLVAR_MSG_BATTLER_TEMP
    CompareMonDataToValue OPCODE_EQU, BTLSCR_MSG_TEMP, BATTLEMON_CUR_HP, 0, _085
    CompareMonDataToValue OPCODE_NEQ, BTLSCR_MSG_TEMP, BATTLEMON_MAGNET_RISE_TURNS, 0, _065
    CompareMonDataToValue OPCODE_FLAG_SET, BTLSCR_MSG_TEMP, BATTLEMON_MOVE_EFFECTS_MASK, MOVE_EFFECT_AIRBORNE, _072
    CompareMonDataToValue OPCODE_FLAG_SET, BTLSCR_MSG_TEMP, BATTLEMON_MOVE_EFFECTS_MASK, MOVE_EFFECT_ABILITY_SUPPRESSED, _085
    CompareMonDataToValue OPCODE_FLAG_SET, BTLSCR_MSG_TEMP, BATTLEMON_MOVE_EFFECTS_MASK, MOVE_EFFECT_INGRAIN, _085
    CompareMonDataToValue OPCODE_EQU, BTLSCR_MSG_TEMP, BATTLEMON_ABILITY, ABILITY_LEVITATE, _078
    CompareMonDataToValue OPCODE_EQU, BTLSCR_MSG_TEMP, BATTLEMON_TYPE_1, TYPE_FLYING, _078
    CompareMonDataToValue OPCODE_EQU, BTLSCR_MSG_TEMP, BATTLEMON_TYPE_2, TYPE_FLYING, _078
    GoTo _085

_065:
    UpdateMonData OPCODE_SET, BTLSCR_MSG_TEMP, BATTLEMON_MAGNET_RISE_TURNS, 0
    GoTo _078

_072:
    UnlockMoveChoice BTLSCR_MSG_TEMP
    ToggleVanish BTLSCR_MSG_TEMP, FALSE
    Wait 

_078:
    // {0} couldn’t stay airborne because of gravity!
    PrintMessage BattleStrings_Text_PokemonCouldntStayAirborneBecauseOfGravity_Ally, TAG_NICKNAME, BTLSCR_MSG_TEMP
    Wait 
    WaitButtonABTime 30

_085:
    UpdateVar OPCODE_ADD, BTLVAR_BATTLER_SPEED_TEMP, 1
    GoToIfValidMon BTLVAR_BATTLER_SPEED_TEMP, _013
    End 
