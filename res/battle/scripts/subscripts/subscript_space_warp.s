#include "macros/btlcmd.inc"


_000:
    // {0}’s {1} intensified gravity!
    PrintMessage BattleStrings_Text_PokemonsAbilityIntensifiedGravity_Ally, TAG_NICKNAME_ABILITY, BTLSCR_MSG_TEMP, BTLSCR_MSG_BATTLER_TEMP
    Wait
    WaitButtonABTime 30
    UpdateVar OPCODE_FLAG_ON, BTLVAR_FIELD_CONDITIONS, FIELD_CONDITION_GRAVITY_PERM
    // Same one-time grounding as the Gravity move (knock down airborne Pokemon,
    // cancel Magnet Rise); it detects the PERM flag and skips its own message.
    Call BATTLE_SUBSCRIPT_GRAVITY_START
    End
