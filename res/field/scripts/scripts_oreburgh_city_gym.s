#include "macros/scrcmd.inc"
#include "res/text/bank/menu_entries.h"
#include "res/text/bank/oreburgh_city_gym.h"


    ScriptEntry OreburghGym_Roark
    ScriptEntry OreburghGym_GymGuide
    ScriptEntry OreburghGym_GymStatue
    ScriptEntry OreburghGym_PuzzleRock1
    ScriptEntry OreburghGym_PuzzleRock2
    ScriptEntryEnd

OreburghGym_Roark:
    PlaySE SE_CONFIRM_sseq_3
    LockAll
    FacePlayer
    GoToIfBadgeAcquired BADGE_ID_COAL, OreburghGym_AlreadyHaveCoalBadge
    CreateJournalEvent LOCATION_EVENT_GYM_WAS_TOO_TOUGH, MAP_HEADER_OREBURGH_CITY_GYM
    Message OreburghGym_Text_RoarkIntro
    CloseMessage
    StartTrainerBattle TRAINER_LEADER_ROARK
    CheckWonBattle VAR_RESULT
    GoToIfEq VAR_RESULT, FALSE, OreburghGym_LostBattle
    Message OreburghGym_Text_BeatRoark
    BufferPlayerName 0
    Message OreburghGym_Text_RoarkReceiveCoalBadge
    PlayFanfare SEQ_BADGE_sseq
    WaitFanfare
    SetTrainerFlag TRAINER_YOUNGSTER_JONATHON
    SetTrainerFlag TRAINER_YOUNGSTER_DARIUS
    GiveBadge BADGE_ID_COAL
    IncrementTrainerScore2 TRAINER_SCORE_EVENT_BADGE_EARNED
    SetVar VAR_HARD_LEVEL_CAP, 23
    BufferNumber 0, VAR_HARD_LEVEL_CAP
    Message OreburghGym_Text_LevelCapRaised
    SetTrainerFlag TRAINER_YOUNGSTER_JONATHON
    SetTrainerFlag TRAINER_YOUNGSTER_DARIUS
    SetFlag FLAG_HIDE_POKECENTER_BASEMENT_BLOCKADE
    SetVar VAR_GTS_ACCESS_STATE, 1
    SetVar VAR_JUBILIFE_LOOKER_PAL_PAD_STATE, 1
    SetVar VAR_OREBURGH_CITY_STATE, 2
    CreateJournalEvent LOCATION_EVENT_BEAT_GYM_LEADER, MAP_HEADER_OREBURGH_CITY_GYM, TRAINER_LEADER_ROARK
    SetVar VAR_JUBILIFE_CITY_STATE, 3
    ClearFlag FLAG_HIDE_JUBILIFE_CITY_COUNTERPART
    ClearFlag FLAG_HIDE_JUBILIFE_ROWAN
    ClearFlag FLAG_HIDE_JUBILIFE_GALACTIC_GRUNTS
    SetFlag FLAG_HIDE_SANDGEM_TOWN_LAB_PROF_ROWAN
    Message OreburghGym_Text_RoarkExplainCoalBadge
    GoTo OreburghGym_RoarkGiveTM76
    End

OreburghGym_RoarkGiveTM76:
    SetVar VAR_0x8004, ITEM_TM76
    SetVar VAR_0x8005, 1
    GoToIfCannotFitItem VAR_0x8004, VAR_0x8005, VAR_RESULT, OreburghGym_RoarkGiveTM76BagFull
    Common_GiveItemQuantity
    SetFlag FLAG_RECEIVED_ROARK_TM76
    BufferItemName 0, VAR_0x8004
    BufferTMHMMoveName 1, VAR_0x8004
    Message OreburghGym_Text_RoarkExplainStealthRock
    WaitButton
    CloseMessage
    ReleaseAll
    End

OreburghGym_RoarkGiveTM76BagFull:
    Common_MessageBagIsFull
    CloseMessage
    ReleaseAll
    End

OreburghGym_AlreadyHaveCoalBadge:
    GoToIfUnset FLAG_RECEIVED_ROARK_TM76, OreburghGym_RoarkGiveTM76
    Message OreburghGym_Text_RoarkGymBeaten
    WaitButton
    CloseMessage
    ReleaseAll
    End

OreburghGym_LostBattle:
    BlackOutFromBattle
    ReleaseAll
    End

OreburghGym_GymGuide:
    PlaySE SE_CONFIRM_sseq_3
    LockAll
    FacePlayer
    GoToIfBadgeAcquired BADGE_ID_COAL, OreburghGym_GymGuideAfterBadge
    Message OreburghGym_Text_GymGuideBeforeBadge
    WaitButton
    CloseMessage
    ReleaseAll
    End

OreburghGym_GymGuideAfterBadge:
    BufferPlayerName 0
    Message OreburghGym_Text_GymGuideAfterBadge
    WaitButton
    CloseMessage
    ReleaseAll
    End

OreburghGym_GymStatue:
    PlaySE SE_CONFIRM_sseq_3
    LockAll
    GoToIfBadgeAcquired BADGE_ID_COAL, OreburghGym_GymStatueAfterBadge
    BufferRivalName 0
    BufferRivalName 1
    Message OreburghGym_Text_GymStatueBeforeBadge
    WaitButton
    CloseMessage
    ReleaseAll
    End

OreburghGym_GymStatueAfterBadge:
    BufferRivalName 0
    BufferPlayerName 1
    BufferRivalName 2
    Message OreburghGym_Text_GymStatueAfterBadge
    WaitButton
    CloseMessage
    ReleaseAll
    End

OreburghGym_PuzzleRock1:
    PlaySE SE_CONFIRM_sseq_3
    LockAll
    FacePlayer
    Message OreburghGym_Text_PuzzleQuestion1
    InitGlobalTextMenu 20, 8, 0, VAR_RESULT, FALSE
    AddMenuEntry MenuEntries_Text_HardStone, 0
    AddMenuEntry MenuEntries_Text_DuskBalls, 1
    AddMenuEntry MenuEntries_Text_RareCandies, 2
    AddMenuEntry MenuEntries_Text_SilkScarf, 3
    ShowMenu
    GoToIfEq VAR_RESULT, 3, OreburghGym_PuzzleCorrect
    GoTo OreburghGym_PuzzleWrong
    End

OreburghGym_PuzzleRock2:
    PlaySE SE_CONFIRM_sseq_3
    LockAll
    FacePlayer
    Message OreburghGym_Text_PuzzleQuestion2
    InitGlobalTextMenu 20, 8, 0, VAR_RESULT, FALSE
    AddMenuEntry MenuEntries_Text_RareCandies, 0
    AddMenuEntry MenuEntries_Text_TM27, 1
    AddMenuEntry MenuEntries_Text_GreatBalls, 2
    AddMenuEntry MenuEntries_Text_RepelToggle, 3
    ShowMenu
    GoToIfEq VAR_RESULT, 3, OreburghGym_PuzzleCorrect
    GoTo OreburghGym_PuzzleWrong
    End

OreburghGym_PuzzleCorrect:
    Message OreburghGym_Text_PuzzleCorrect
    WaitButton
    CloseMessage
    RemoveObject VAR_LAST_TALKED
    ReleaseAll
    End

OreburghGym_PuzzleWrong:
    Message OreburghGym_Text_PuzzleWrong
    WaitButton
    CloseMessage
    StartTrainerBattle TRAINER_GRAVELER_GYM_PUZZLE
    CheckWonBattle VAR_RESULT
    GoToIfEq VAR_RESULT, FALSE, OreburghGym_PuzzleLostBattle
    ReleaseAll
    End

OreburghGym_PuzzleLostBattle:
    BlackOutFromBattle
    ReleaseAll
    End

    .balign 4, 0
