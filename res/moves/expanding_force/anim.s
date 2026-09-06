#include "macros/btlanimcmd.inc"

L_0:
    PlaySoundEffectR SEQ_SE_DP_030_sseq
    Func_FadeBg FADE_BG_TYPE_BASE, 1, 0, 8, BATTLE_COLOR_PURPLE
    Func_Shake 1, 0, 2, 4, BATTLE_ANIM_BATTLER_SPRITE_DEFENDER
    Func_Shake 1, 0, 2, 4, BATTLE_ANIM_BATTLER_SPRITE_DEFENDER_PARTNER
    WaitForAnimTasks
    Func_FadeBg FADE_BG_TYPE_BASE, 1, 8, 0, BATTLE_COLOR_PURPLE
    WaitForAnimTasks
    End
