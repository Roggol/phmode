#include "macros/btlcmd.inc"


_000:
    CheckSubstitute BTLSCR_SIDE_EFFECT_MON, _043
    // phmode: Ghost-types are immune to binding moves entirely (modern mechanic) - the move
    // still deals its initial hit of damage, but never traps or racks up end-of-turn damage.
    CompareMonDataToValue OPCODE_EQU, BTLSCR_DEFENDER, BATTLEMON_TYPE_1, TYPE_GHOST, _043
    CompareMonDataToValue OPCODE_EQU, BTLSCR_DEFENDER, BATTLEMON_TYPE_2, TYPE_GHOST, _043
    CompareMonDataToValue OPCODE_FLAG_SET, BTLSCR_DEFENDER, BATTLEMON_VOLATILE_STATUS, VOLATILE_CONDITION_BIND, _043
    PrintBufferedMessage 
    Wait 
    WaitButtonABTime 30
    Random 3, 3
    CheckItemHoldEffect CHECK_NOT_HAVE, BTLSCR_ATTACKER, HOLD_EFFECT_EXTEND_TRAPPING, _024
    UpdateVar OPCODE_SET, BTLVAR_CALC_TEMP, 0x00000006

_024:
    UpdateVar OPCODE_LEFT_SHIFT, BTLVAR_CALC_TEMP, 0x0000000D
    UpdateMonDataFromVar OPCODE_FLAG_ON, BTLSCR_DEFENDER, BATTLEMON_VOLATILE_STATUS, BTLVAR_CALC_TEMP
    UpdateMonDataFromVar OPCODE_SET, BTLSCR_DEFENDER, BATTLEMON_BIND_TARGET, BTLVAR_ATTACKER
    UpdateMonDataFromVar OPCODE_SET, BTLSCR_DEFENDER, BATTLEMON_BINDING_MOVE, BTLVAR_CURRENT_MOVE

_043:
    End 
