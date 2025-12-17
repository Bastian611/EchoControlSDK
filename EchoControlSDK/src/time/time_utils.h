
#pragma once
#include "time_convert.h"

ECCS_BEGIN


void msleep(u32 ms);
u64 cpuCounter();
u64 cpuFrequency();


typedef YMDHMSM DateTime;
double operator - (const DateTime& l, const DateTime& r); // sec
bool operator == (const DateTime& l, const DateTime& r);
bool operator > (const DateTime& l, const DateTime& r);
bool operator < (const DateTime& l, const DateTime& r);
bool operator >= (const DateTime& l, const DateTime& r);
bool operator <= (const DateTime& l, const DateTime& r);


DateTime now();
DateTime now(float timezone);
int setSystemTime(DateTime dt);


template<size_t MAX_SIZE>
str stringTime(const struct tm& t, const char* fmt)
{
    char dt[MAX_SIZE] = {0};
    strftime(dt, sizeof(dt), fmt, &t);
    return dt;
}
char* stringPTime(const char* s,struct tm& t, const char* fmt);


inline str stringTime(const struct tm& t, const char* fmt)
{
    return stringTime<128>(t, fmt);
}

template<size_t MAX_SIZE>
str stringTime(const DateTime& dt, const char* fmt)
{
    return stringTime<MAX_SIZE>(dt.toTM(), fmt);
}
inline str stringTime(const DateTime& dt, const char* fmt)
{
    return stringTime(dt.toTM(), fmt);
}

template<size_t MAX_SIZE>
str stringNow(const char* fmt)
{
    return stringTime<MAX_SIZE>(now().toTM(), fmt);
}
inline str stringNow(const char* fmt)
{
    return stringTime(now().toTM(), fmt);
}

template<size_t MAX_SIZE>
str stringNow(float tz, const char* fmt)
{
    return stringTime<MAX_SIZE>(now(tz).toTM(), fmt);
}
inline str stringNow(float tz, const char* fmt)
{
    return stringTime(now(tz).toTM(), fmt);
}


ECCS_END
