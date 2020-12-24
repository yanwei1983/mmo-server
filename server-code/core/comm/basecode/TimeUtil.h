#ifndef TIMEUTIL_H
#define TIMEUTIL_H

#include <cstddef>
#include <ctime>
#include <string>

#include "export_lua.h"

#ifdef WIN32

    

    #define sleep(sec)   Sleep(sec * 1000)
    #define msleep(msec) Sleep(msec)
    void usleep(unsigned long usec);
#else
    #include <unistd.h>
    #define msleep(msec) usleep(msec * 1000)
#endif


constexpr time_t ONE_MINUS_SEC = 60;
constexpr time_t ONE_HOUR_SEC  = 60 * ONE_MINUS_SEC;
constexpr time_t ONE_DAY_SEC   = 24 * ONE_HOUR_SEC;
constexpr time_t ONE_WEEK_SEC  = 7 * ONE_DAY_SEC;

void TimeGetCacheCreate();
void TimeGetCacheUpdate();

export_lua time_t TimeGetMonotonic();
export_lua time_t TimeGetMillisecond();
export_lua time_t now();
export_lua time_t TimeGetSecond();
export_lua time_t TimeGetSecondLocal();

export_lua time_t _TimeGetMonotonic();
export_lua time_t _TimeGetSecond();
export_lua time_t _TimeGetMillisecond();
export_lua time_t _TimeGetSecondLocal();

export_lua time_t _TimeGetSecondFrom2K2K();

export_lua time_t    gmt2local(time_t tNow);
export_lua time_t    local2gmt(time_t tNow);
export_lua struct tm timeToLocalTime(time_t t);
//检查是否是同一天,loclatime
export_lua bool CheckSameDay(time_t time1, time_t time2);
//返回两个时间戳之间的日期差
export_lua int32_t DayDiffLocal(time_t time1, time_t time2);
export_lua int32_t WeekDiffLocal(time_t time1, time_t time2);
export_lua int32_t MonthDiffLocal(time_t time1, time_t time2);
export_lua time_t  GetNextDayBeginTime();

export_lua time_t NextDayBeginTimeStamp(time_t time1, int32_t nDays);
export_lua time_t NextWeekBeginTimeStamp(time_t time1, int32_t nWeeks);
export_lua time_t NextMonthBeginTimeStamp(time_t time1, int32_t nMonths);

export_lua time_t GetTimeFromString(const std::string& time_str);

export_lua inline int32_t isleap(uint32_t year)
{
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

#endif /* TIMEUTIL_H */
