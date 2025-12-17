
#pragma once
#include "../global.h"

#ifdef SUPPORT_C11
#include <chrono>
#else
#include <boost/chrono.hpp>
#endif

#ifdef _WIN32
#define timegm(t)  _mkgmtime(t)
#endif

ECCS_BEGIN


typedef ECCS_C11 chrono::steady_clock steady_clock;
typedef ECCS_C11 chrono::system_clock system_clock;
typedef ECCS_C11 chrono::seconds      duration_sec;
typedef ECCS_C11 chrono::milliseconds duration_ms;
typedef ECCS_C11 chrono::microseconds duration_us;


#if defined(__linux__)

#include <time.h>
inline void gmtime_s(struct tm* _tm, time_t* _time)
{
    gmtime_r(_time, _tm);
}
inline void localtime_s(struct tm* _tm, time_t* _time)
{
    localtime_r(_time, _tm);
}

#endif


ECCS_END
