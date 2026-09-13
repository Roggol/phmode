#include "macros/scrcmd.inc"
#include "res/text/bank/great_marsh_6.h"


    ScriptEntry GreatMarsh6_AceTrainerM
    ScriptEntry GreatMarsh6_SignboardArea6
    ScriptEntryEnd

GreatMarsh6_AceTrainerM:
    PlaySE SE_CONFIRM_sseq_3
    LockAll
    FacePlayer
    GoToIfSet FLAG_DAILY_RECEIVED_GREAT_MARSH_6_RANDOM_SHARD, GreatMarsh6_IllTryFindingMore
    Message GreatMarsh6_Text_IPickedThisUp
    SetVar VAR_0x8004, ITEM_HEART_SCALE
    GoTo GreatMarsh6_TryGiveShard
    End

GreatMarsh6_TryGiveShard:
    SetVar VAR_0x8005, 1
    GoToIfCannotFitItem VAR_0x8004, VAR_0x8005, VAR_RESULT, GreatMarsh6_BagIsFull
    SetFlag FLAG_DAILY_RECEIVED_GREAT_MARSH_6_RANDOM_SHARD
    Common_GiveItemQuantityNoLineFeed
    CloseMessage
    ReleaseAll
    End

GreatMarsh6_IllTryFindingMore:
    Message GreatMarsh6_Text_IllTryFindingMore
    WaitButton
    CloseMessage
    ReleaseAll
    End

GreatMarsh6_BagIsFull:
    Common_MessageBagIsFull
    CloseMessage
    ReleaseAll
    End

GreatMarsh6_SignboardArea6:
    ShowLandmarkSign GreatMarsh6_Text_Area6
    End
