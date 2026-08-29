#include "macros/btlanimcmd.inc"

.data

L_0:
    Func_FadeBg FADE_BG_TYPE_BASE, 1, 0, 12, 0x4D6E
    PlaySoundEffectC SEQ_SE_DP_W185_sseq
    WaitForAnimTasks
    Delay 20
    Func_FadeBg FADE_BG_TYPE_BASE, 1, 12, 0, 0x4D6E
    WaitForAnimTasks
    End
