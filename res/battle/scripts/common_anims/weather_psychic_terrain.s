#include "macros/btlanimcmd.inc"

.data

L_0:
    Func_FadeBg FADE_BG_TYPE_BASE, 1, 0, 12, 0x6D9F
    PlaySoundEffectC SEQ_SE_DP_480_sseq
    WaitForAnimTasks
    Delay 20
    Func_FadeBg FADE_BG_TYPE_BASE, 1, 12, 0, 0x6D9F
    WaitForAnimTasks
    End
