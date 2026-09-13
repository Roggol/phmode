#include "macros/scrcmd.inc"
#include "res/text/bank/route_221_house.h"


    ScriptEntry Route221House_ExpertM
    ScriptEntry Route221House_BgSign
    ScriptEntryEnd

Route221House_ExpertM:
    PlaySE SE_CONFIRM_sseq_3
    LockAll
    FacePlayer
    GoToIfEq VAR_ROUTE_221_HOUSE_REWARD_INDEX, 3, Route221House_AlreadyGaveReward
    GoToIfNe VAR_ROUTE_221_HOUSE_REWARD_INDEX, 0, Route221House_ResumeReward
    Message Route221House_Text_HereAreAllThree
    GoTo Route221House_GiveNextReward
    End

Route221House_ResumeReward:
    Message Route221House_Text_ThankWithItem
    GoTo Route221House_GiveNextReward
    End

Route221House_GiveNextReward:
    CallIfEq VAR_ROUTE_221_HOUSE_REWARD_INDEX, 0, Route221House_SetRewardBlackBelt
    CallIfEq VAR_ROUTE_221_HOUSE_REWARD_INDEX, 1, Route221House_SetRewardExpertBelt
    CallIfEq VAR_ROUTE_221_HOUSE_REWARD_INDEX, 2, Route221House_SetRewardFocusSash
    SetVar VAR_0x8005, 1
    GoToIfCannotFitItem VAR_0x8004, VAR_0x8005, VAR_RESULT, Route221House_NoRoomInBag
    Common_GiveItemQuantityNoLineFeed
    AddVar VAR_ROUTE_221_HOUSE_REWARD_INDEX, 1
    GoToIfNe VAR_ROUTE_221_HOUSE_REWARD_INDEX, 3, Route221House_GiveNextReward
    CloseMessage
    ReleaseAll
    End

Route221House_SetRewardBlackBelt:
    SetVar VAR_0x8004, ITEM_BLACK_BELT
    Return

Route221House_SetRewardExpertBelt:
    SetVar VAR_0x8004, ITEM_EXPERT_BELT
    Return

Route221House_SetRewardFocusSash:
    SetVar VAR_0x8004, ITEM_FOCUS_SASH
    Return

Route221House_NoRoomInBag:
    Message Route221House_Text_NoRoomInBag
    WaitButton
    CloseMessage
    ReleaseAll
    End

Route221House_AlreadyGaveReward:
    Message Route221House_Text_AlreadyGaveReward
    WaitButton
    CloseMessage
    ReleaseAll
    End

Route221House_BgSign:
    EventMessage Route221House_Text_WinItemsFromMe
    End

    .balign 4, 0
