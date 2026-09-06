#include "macros/btlcmd.inc"


_000:
    TryStickyWeb _020
    // Your / The foe's side of the field is covered in sticky webs!
    BufferMessage BattleStrings_Text_YourSideStickyWeb, TAG_NONE_SIDE_CONSCIOUS, BTLSCR_ATTACKER_ENEMY
    UpdateVar OPCODE_SET, BTLVAR_SIDE_EFFECT_FLAGS_INDIRECT, MOVE_SIDE_EFFECT_ON_HIT|MOVE_SUBSCRIPT_PTR_PRINT_MESSAGE_AND_PLAY_ANIMATION
    End

_020:
    UpdateVar OPCODE_FLAG_ON, BTLVAR_MOVE_STATUS_FLAGS, MOVE_STATUS_FAILED
    End
