
#include "time_utils.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <string.h>
#endif
#include <time.h>
#include "../thread/sal_thread.h"

ECCS_BEGIN


void msleep(u32 ms)
{
#if 1

#ifdef _WIN32
    Sleep(ms);
#else
    timespec req, rem;
    req.tv_sec = ms / 1000;
    req.tv_nsec = (ms % 1000) * 1000000;
    while (true) {
        if (nanosleep(&req, &rem) == 0) {
            break;
        }
        if (errno != EINTR) {
            break;
        }
        req = rem;
    }
#endif

#else

    // Linux: will be interrupted by signals
    sleep_for(duration_ms(ms));

#endif
}
u64 cpuCounter()
{
#ifdef _WIN32
    LARGE_INTEGER cnt;
    QueryPerformanceCounter(&cnt);
    return cnt.QuadPart;
#else
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now)){
        return 0;
    }
    return (now.tv_sec*1000000000L + now.tv_nsec);
#endif
}
u64 cpuFrequency()
{
#ifdef _WIN32
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return freq.QuadPart;
#else
    return 1000000000;
#endif
}


double operator - (const DateTime& l, const DateTime& r)
{
    struct tm ltm = l.toTM(), rtm = r.toTM();
    time_t ltime = timegm(&ltm);
    time_t rtime = timegm(&rtm);

    return (ltime + l.ms/1000.0) - (rtime + r.ms/1000.0);
}
bool operator == (const DateTime& l, const DateTime& r)
{
    if (l.ms != r.ms) {
        return false;
    }

    struct tm ltm = l.toTM(), rtm = r.toTM();
    time_t ltime = timegm(&ltm);
    time_t rtime = timegm(&rtm);

    return (ltime == rtime);
}
bool operator > (const DateTime& l, const DateTime& r)
{
    struct tm ltm = l.toTM(), rtm = r.toTM();
    time_t ltime = timegm(&ltm);
    time_t rtime = timegm(&rtm);

    if (ltime > rtime) {
        return true;
    }
    if (ltime == rtime && l.ms > r.ms) {
        return true;
    }
    return false;
}
bool operator < (const DateTime& l, const DateTime& r)
{
    struct tm ltm = l.toTM(), rtm = r.toTM();
    time_t ltime = timegm(&ltm);
    time_t rtime = timegm(&rtm);

    if (ltime < rtime) {
        return true;
    }
    if (ltime == rtime && l.ms < r.ms) {
        return true;
    }
    return false;
}
bool operator >= (const DateTime& l, const DateTime& r)
{
    struct tm ltm = l.toTM(), rtm = r.toTM();
    time_t ltime = timegm(&ltm);
    time_t rtime = timegm(&rtm);

    if (ltime > rtime) {
        return true;
    }
    if (ltime == rtime && l.ms >= r.ms) {
        return true;
    }
    return false;
}
bool operator <= (const DateTime& l, const DateTime& r)
{
    struct tm ltm = l.toTM(), rtm = r.toTM();
    time_t ltime = timegm(&ltm);
    time_t rtime = timegm(&rtm);

    if (ltime < rtime) {
        return true;
    }
    if (ltime == rtime && l.ms <= r.ms) {
        return true;
    }
    return false;
}
char* stringPTime(const char* s,struct tm& t, const char* fmt)
{
#if __linux
    char* ret = strptime(s,  fmt, &t);
#elif WIN32
    char* ret;
#endif
    return ret;
}
DateTime now()
{
    DateTime dt;

    // get datetime
    struct tm t;
    auto tp = system_clock::now();
    auto secs = system_clock::to_time_t(tp);
    localtime_s(&t, &secs);
    dt.setTime(t);

    // get ms
    auto d = tp.time_since_epoch();
    auto dsec = ECCS_C11 chrono::duration_cast<duration_sec>(d);
    auto dms = ECCS_C11 chrono::duration_cast<duration_ms>(d);
    dt.ms = u16(dms.count() - dsec.count() * 1000);

    return dt;
}
DateTime now(float timezone)
{
    DateTime dt;

    // get datetime
    struct tm t;
    auto tp = system_clock::now();
    auto secs = system_clock::to_time_t(tp);
    secs += static_cast<int>(timezone) * 3600;
    gmtime_s(&t, &secs);
    dt.setTime(t);

    // get ms
    auto d = tp.time_since_epoch();
    auto dsec = ECCS_C11 chrono::duration_cast<duration_sec>(d);
    auto dms = ECCS_C11 chrono::duration_cast<duration_ms>(d);
    dt.ms = u16(dms.count() - dsec.count() * 1000);

    return dt;
}
int setSystemTime(DateTime dt)
{
#ifdef _WIN32
    SYSTEMTIME st;
    st.wYear = dt.year;
    st.wMonth = dt.mon;
    st.wDay = dt.day;
    st.wHour = dt.hour;
    st.wMinute = dt.min;
    st.wSecond = dt.sec;
    st.wMilliseconds = dt.ms;
    if (!SetSystemTime(&st)) {
        return -1;
    }
    return 0;
#else
    struct tm t = dt.toTM();
    timeval tv;
    tv.tv_sec = timegm(&t);
    tv.tv_usec = (dt.ms)*1000;
    if (settimeofday(&tv, NULL)) {
        return -1;
    }
    system("hwclock -w");
    return 0;
#endif
}


ECCS_END
