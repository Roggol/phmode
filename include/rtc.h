#ifndef POKEPLATINUM_RTC_H
#define POKEPLATINUM_RTC_H

#include <nitro/rtc.h>

#include "constants/rtc.h"

static inline BOOL IsLeapYear(u32 year)
{
    if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void InitRTC(void);
void UpdateRTC(void);
void GetCurrentDateTime(RTCDate *date, RTCTime *time);
void RTC_GetCurrentTime(RTCTime *time);
void GetCurrentDate(RTCDate *date);
int GetSecondsSinceMidnight(void);
s64 GetTimestamp(void);
int DayNumberForDate(const RTCDate *date);
BOOL IsNight(void);
enum TimeOfDay GetTimeOfDay(void);
enum TimeOfDay TimeOfDayForHour(int hour);
s64 TimeElapsed(s64 since, s64 until);

// phmode: player-facing time-of-day override, set via the Poketch Manipulator key item
// (see item_use_functions.c). FORCED_TIMEOFDAY_OFF means "follow the real clock as usual".
enum ForcedTimeOfDay {
    FORCED_TIMEOFDAY_OFF = 0,
    FORCED_TIMEOFDAY_MORNING,
    FORCED_TIMEOFDAY_DAY,
    FORCED_TIMEOFDAY_NIGHT,
};

enum ForcedTimeOfDay RTC_GetForcedTimeOfDay(void);
void RTC_SetForcedTimeOfDay(enum ForcedTimeOfDay forced);

#endif // POKEPLATINUM_RTC_H
