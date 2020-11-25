#include "TimeUtil.h"

#include "BaseCode.h"

time_t timeGetTime()
{
    // 	timeval cTime;
    // 	gettimeofday(&cTime,NULL);
    // 	return (1000*(time_t)cTime.tv_sec + cTime.tv_usec/1000);

    /*
    struct timespec {
    time_t   tv_sec;        // seconds
    long     tv_nsec;       // nanoseconds
    };

    int32_t clock_gettime(clockid_t clk_id, struct timespec *tp);
    clk_id:
    CLOCK_REALTIME				系统实时时间
    CLOCK_MONOTONIC				从系统启动到这一刻所经过的时间，不受系统时间被用户改变的影响
    CLOCK_PROCESS_CPUTIME_ID	本进程到当前所花费的系统CPU时间
    CLOCK_THREAD_CPUTIME_ID		本线程到当前所花费的系统CPU时间
    */
    timespec _ts;
    if(clock_gettime(CLOCK_MONOTONIC, &_ts) != 0)
    {
        // error
        return 0;
    }
    time_t _tick = (time_t)_ts.tv_sec * 1000 + _ts.tv_nsec / 1000000;
    return _tick;
}

time_t _TimeGetMonotonic()
{
    return timeGetTime();
}

time_t _TimeGetSecond()
{
    return ::time(NULL);
}

time_t _TimeGetSecondFrom2K2K()
{
    static struct tm tm2020
    {
        .tm_year = 2020
    };
    static const time_t time_2020 = mktime(&tm2020);
    auto                cur_time  = _TimeGetSecond();
    if(cur_time > time_2020)
        return 0;
    else
        return cur_time - time_2020;
}

time_t _TimeGetMillisecond()
{
    timespec _ts;
    if(clock_gettime(CLOCK_REALTIME, &_ts) != 0)
    {
        return 0;
    }
    time_t _tick = (time_t)_ts.tv_sec * 1000 + _ts.tv_nsec / 1000000;
    return _tick;
}

time_t _TimeGetSecondLocal()
{
    return gmt2local(_TimeGetSecond());
}

struct TimeGetCacheData
{
    bool   bUserCache        = false;
    time_t LastMono          = 0;
    time_t Now               = 0;
    time_t NowOffset         = 0;
    time_t NowOffsetSec      = 0;
    time_t NowOffsetLocalSec = 0;
};
thread_local static TimeGetCacheData g_TimeGetCacheData;
static std::atomic<time_t>           g_TimeOffset = 0;

void TimeOffset(time_t offset)
{
    g_TimeOffset += offset;
}

void TimeGetCacheCreate()
{
    g_TimeGetCacheData.bUserCache = true;
    g_TimeGetCacheData.LastMono   = _TimeGetMonotonic();   //取得MONO时间戳
    g_TimeGetCacheData.Now        = _TimeGetMillisecond(); //取得系统时间戳
}

void TimeGetCacheUpdate()
{
    if(g_TimeGetCacheData.bUserCache)
    {
        time_t now_mono = _TimeGetMonotonic();
        if(now_mono > g_TimeGetCacheData.LastMono)
        {
            g_TimeGetCacheData.Now += (now_mono - g_TimeGetCacheData.LastMono);
        }
        g_TimeGetCacheData.LastMono          = now_mono;
        g_TimeGetCacheData.NowOffset         = g_TimeGetCacheData.Now + g_TimeOffset;
        g_TimeGetCacheData.NowOffsetSec      = g_TimeGetCacheData.NowOffset / 1000;
        g_TimeGetCacheData.NowOffsetLocalSec = gmt2local(g_TimeGetCacheData.NowOffsetSec);
    }
}

time_t TimeGetMonotonic()
{
    if(g_TimeGetCacheData.bUserCache)
        return g_TimeGetCacheData.NowOffset;
    else
        return _TimeGetMonotonic();
}

time_t now()
{
    return TimeGetSecond();
}

time_t TimeGetSecond()
{
    if(g_TimeGetCacheData.bUserCache)
        return g_TimeGetCacheData.NowOffsetSec;
    else
        return _TimeGetSecond();
}

time_t TimeGetMillisecond()
{
    if(g_TimeGetCacheData.bUserCache)
        return g_TimeGetCacheData.NowOffset;
    else
        return _TimeGetMillisecond();
}

time_t TimeGetSecondLocal()
{
    if(g_TimeGetCacheData.bUserCache)
        return g_TimeGetCacheData.NowOffsetLocalSec;
    else
        return _TimeGetSecondLocal();
}

time_t gmt2local(time_t tNow)
{
#if defined(WIN32)
    return tNow + _timezone;
#else
    return tNow - timezone;
#endif
}

time_t local2gmt(time_t tNow)
{
#if defined(WIN32)
    return tNow - _timezone;
#else
    return tNow + timezone;
#endif
}

struct tm timeToLocalTime(time_t t)
{
#ifdef WIN32
#if _MSC_VER < 1400 // VS2003
    return *localtime(&t);
#else // vs2005->vs2013->
    struct tm tt = {0};
    localtime_s(&tt, &t);
    return tt;
#endif
#else // linux
    struct tm tt = {0};
    localtime_r(&t, &tt);
    return tt;
#endif
}

/////////////////////////////////////////////////////////////////////////////
bool CheckSameDay(time_t time1, time_t time2)
{
    struct tm tm1;
    if(0 != localtime_r(&time1, &tm1)) /* Convert to local time. */
        return false;

    struct tm tm2;
    if(0 != localtime_r(&time2, &tm2)) /* Convert to local time. */
        return false;

    return (tm1.tm_yday == tm2.tm_yday) && (tm1.tm_mon == tm2.tm_mon) && (tm1.tm_year == tm2.tm_year);
}

/////////////////////////////////////////////////////////////////////////////
int32_t DayDiffLocal(time_t time1, time_t time2)
{
    __ENTER_FUNCTION
    struct tm tm1;
    if(0 != localtime_r(&time1, &tm1)) /* Convert to local time. */
        return 0;

    struct tm tm2;
    if(0 != localtime_r(&time2, &tm2)) /* Convert to local time. */
        return 0;

    // int32_t nLeapYear = isleap(tm2.tm_year) ? 1 : 0;

    int32_t nDays = tm2.tm_yday - tm1.tm_yday;
    if(tm2.tm_year != tm1.tm_year)
    {
        //不同一年
        int32_t nDayDiffYear = 0;
        int32_t year1        = std::min<int32_t>(tm1.tm_year, tm2.tm_year);
        int32_t year2        = std::max<int32_t>(tm1.tm_year, tm2.tm_year);
        for(int32_t i = year1; i < year2; i++)
        {
            if(isleap(1900 + i))
            {
                //闰年
                nDayDiffYear += 366;
            }
            else
            {
                //不是闰年
                nDayDiffYear += 365;
            }
        }
        return nDayDiffYear + (nDays);
    }
    else
    {
        //同年
        return nDays;
    }

    __LEAVE_FUNCTION
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
int32_t WeekDiffLocal(time_t time1, time_t time2)
{
    __ENTER_FUNCTION

    struct tm tm1;
    if(0 != localtime_r(&time1, &tm1)) /* Convert to local time. */
        return false;

    struct tm tm2;
    if(0 != localtime_r(&time2, &tm2)) /* Convert to local time. */
        return false;

    time_t time1_weekstart_time = time1 - (tm1.tm_wday * 86400);
    time_t time2_weekstart_time = time2 - (tm2.tm_wday * 86400);

    int32_t daydiff = DayDiffLocal(time1_weekstart_time, time2_weekstart_time);

    return daydiff / 7;
    __LEAVE_FUNCTION

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
int32_t MonthDiffLocal(time_t time1, time_t time2)
{
    __ENTER_FUNCTION

    struct tm tm1;
    if(0 != localtime_r(&time1, &tm1)) /* Convert to local time. */
        return false;

    struct tm tm2;
    if(0 != localtime_r(&time2, &tm2)) /* Convert to local time. */
        return false;

    int32_t nYears  = tm2.tm_year - tm1.tm_year;
    int32_t nMonths = tm2.tm_mon - tm1.tm_mon;

    return nYears * 12 + nMonths;

    __LEAVE_FUNCTION

    return 0;
}

time_t NextDayBeginTimeStamp(time_t time1, int32_t nDays)
{
    __ENTER_FUNCTION
    struct tm tm1;
    if(0 != localtime_r(&time1, &tm1)) /* Convert to local time. */
    {
        tm1.tm_hour = 0;
        tm1.tm_min  = 0;
        tm1.tm_sec  = 0;
        return mktime(&tm1) + nDays * ONE_DAY_SEC; // 得到次日0点时间戳
    }

    __LEAVE_FUNCTION
    return time1 + nDays * ONE_DAY_SEC;
}

time_t NextWeekBeginTimeStamp(time_t time1, int32_t nWeeks)
{
    __ENTER_FUNCTION
    struct tm tm1;
    if(0 != localtime_r(&time1, &tm1)) /* Convert to local time. */
    {
        time_t time1_weekstart_time = time1 - (tm1.tm_wday * 86400);
        return time1_weekstart_time + nWeeks * ONE_WEEK_SEC;
    }
    __LEAVE_FUNCTION
    return time1 + nWeeks * ONE_WEEK_SEC;
}

time_t NextMonthBeginTimeStamp(time_t time1, int32_t nMonths)
{
    __ENTER_FUNCTION
    struct tm tm1;
    if(0 != localtime_r(&time1, &tm1)) /* Convert to local time. */
    {
        tm1.tm_hour = 0;
        tm1.tm_min  = 0;
        tm1.tm_sec  = 0;
        tm1.tm_mday = 1;
        tm1.tm_mon += nMonths;
        return mktime(&tm1); // 得到次日0点时间戳
    }
    __LEAVE_FUNCTION
    constexpr time_t ONE_MONTH_SEC = 30 * ONE_WEEK_SEC;
    return time1 + nMonths * ONE_MONTH_SEC;
}

time_t GetTimeFromString(const std::string& time_str)
{
    struct tm _tm;
    memset(&_tm, 0, sizeof(_tm));
    strptime(time_str.c_str(), "%Y-%m-%d %H:%M:%S", &_tm);

    time_t t = mktime(&_tm);
    // t = local2gmt(t);
    return t;
}

time_t GetNextDayBeginTime()
{
    static thread_local time_t s_NextDayBeginTime = 0;

    time_t now = TimeGetSecond();
    if(now < s_NextDayBeginTime)
        return s_NextDayBeginTime;

    struct tm curtime;
    if(0 != localtime_r(&now, &curtime)) /* Convert to local time. */
    {
        curtime.tm_hour    = 0;
        curtime.tm_min     = 0;
        curtime.tm_sec     = 0;
        s_NextDayBeginTime = mktime(&curtime) + 86400; // 得到次日0点时间戳
        return s_NextDayBeginTime;
    }
    return now;
}
