
#include "time_convert.h"
#include <time.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <iosfwd>
#include "sal_chrono.h"

ECCS_BEGIN


//------------------------------------------------------
// Structs
//------------------------------------------------------

WSM::WSM()
{
    clear();
}
WSM::WSM(const u16& week, const u32& second, const u16& milisecond)
    : week(week), sec(second), ms(milisecond)
{

}
void WSM::clear()
{
    week = 0;
    sec = 0;
    ms = 0;
}
bool WSM::isValid() const
{
    return !(week == 0 && sec == 0 && ms == 0);
}


YMDHMSM::YMDHMSM()
{
    clear();
}
YMDHMSM::YMDHMSM(struct tm t)
    : year(t.tm_year+1900), mon(t.tm_mon+1), day(t.tm_mday),
      hour(t.tm_hour), min(t.tm_min), sec(t.tm_sec)
{

}
YMDHMSM::YMDHMSM(const u16& y, const u16& M, const u16& d, const u16& h, const u16& m, const u16& s, const u16& milisecond)
    : year(y), mon(M), day(d), hour(h), min(m), sec(s), ms(milisecond)
{

}
void YMDHMSM::clear()
{
    year = 0;
    mon = 0;;
    day = 0;;
    hour = 0;
    min = 0;
    sec = 0;
    ms = 0;
}
struct tm YMDHMSM::toTM() const
{
    struct tm t;
    memset(&t, 0, sizeof(t));
    t.tm_year = year-1900;
    t.tm_mon = mon-1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = min;
    t.tm_sec = sec;
    return t;
}
void YMDHMSM::setTime(struct tm t)
{
    year = t.tm_year+1900;
    mon = t.tm_mon+1;
    day = t.tm_mday;
    hour = t.tm_hour;
    min = t.tm_min;
    sec = t.tm_sec;
}


//------------------------------------------------------
// Time convert functions
//
// !!NOTE: 'leap_sec' below is the value read from GPS receiver, caution of SIGN!!
//------------------------------------------------------

void gps2local(
    const u16&	week,
    const u32&	secInWeek,
    const int&  zone,
    const int&	leap_sec,
    u16*		year,
    u16*		mon,
    u16*		day,
    u16*		hour,
    u16*		min,
    u16*		sec
    )
{
    // 【1970-1-1 00:00:00 UTC】至【1980-1-6 00:00:00 UTC】的时间间隔
    const time_t gps_start = 315964800;  // gps start time

    // gps time -> time_t !! leap_sec is negative !!
    time_t now_sec = gps_start + ((time_t)week)*7*24*3600 + (time_t)secInWeek + zone*3600 +  leap_sec;

    // time_t -> tm
#if  0
    struct tm now = *(gmtime(&now_sec));   // may not thread-safe
#else
    struct tm now;
    gmtime_s(&now, &now_sec);
#endif

    // assignment
    if (year){
        *year = (u16)(now.tm_year+1900);
    }
    if (mon){
        *mon = (u16)(now.tm_mon+1);
    }
    if (day){
        *day = (u16)(now.tm_mday);
    }
    if (hour){
        *hour = (u16)(now.tm_hour);
    }
    if (min){
        *min = (u16)(now.tm_min);
    }
    if (sec){
        *sec = (u16)(now.tm_sec);
    }
}
void local2gps(
    const u16&	year,
    const u16&	mon,
    const u16&	day,
    const u16&	hour,
    const u16&	min,
    const u16&	sec,
    const int&	zone,
    const int&	leap_sec,
    u16*		week,
    u32*		secInWeek
    )
{
    // now (tm)
    struct tm now;
    now.tm_year = year - 1900;
    now.tm_mon = mon - 1;
    now.tm_hour = hour;
    now.tm_min = min;
    now.tm_sec = sec;
    now.tm_mday = day;
    now.tm_isdst = -1;

    // 【1970-1-1 00:00:00 UTC】至【1980-1-6 00:00:00 UTC】的时间间隔
    const time_t gps_start = 315964800;  // gps start time

    // !! leap_sec is negative !!
    time_t sec_s = timegm(&now) - gps_start - zone*3600 - leap_sec;

    if (week){
        *week = (u16)(sec_s / (7*24*3600));
    }
    if (secInWeek){
        *secInWeek = sec_s % (7*24*3600);
    }
}
void gps2utc(
    const u16&	week,
    const u32&	secInWeek,
    const int&	leap_sec,
    u16*		year,
    u16*		mon,
    u16*		day,
    u16*		hour,
    u16*		min,
    u16*		sec
    )
{
    gps2local(week, secInWeek, 0, leap_sec, year, mon, day, hour, min, sec);
}
void utc2gps(
    const u16&	year,
    const u16&	mon,
    const u16&	day,
    const u16&	hour,
    const u16&	min,
    const u16&	sec,
    const int&	leap_sec,
    u16*		week,
    u32*		secInWeek
    )
{
    local2gps(year, mon, day, hour, min, sec, 0, leap_sec, week, secInWeek);
}

YMDHMSM gps2local(
    const WSM&	wsm,
    const int&	zone,
    const int&	leap_sec
    )
{
    YMDHMSM datetime;
    gps2local(wsm.week, wsm.sec, zone, leap_sec,
        &datetime.year, &datetime.mon, &datetime.day, &datetime.hour, &datetime.min, &datetime.sec);
    datetime.ms = wsm.ms;
    return datetime;
}
WSM local2gps(
    const YMDHMSM&	local,
    const int&		zone,
    const int&		leap_sec
    )
{
    WSM wsm;
    local2gps(local.year, local.mon, local.day, local.hour, local.min, local.sec,
        zone, leap_sec, &wsm.week, &wsm.sec);
    wsm.ms = local.ms;
    return wsm;
}
YMDHMSM gps2utc(
    const WSM&	wsm,
    const int&	leap_sec
    )
{
    YMDHMSM datetime;
    gps2utc(wsm.week, wsm.sec, leap_sec,
        &datetime.year, &datetime.mon, &datetime.day, &datetime.hour, &datetime.min, &datetime.sec);
    datetime.ms = wsm.ms;
    return datetime;
}
WSM utc2gps(
    const YMDHMSM&	utc,
    const int&		leap_sec
    )
{
    WSM wsm;
    utc2gps(utc.year, utc.mon, utc.day, utc.hour, utc.min, utc.sec, leap_sec, &wsm.week, &wsm.sec);
    wsm.ms = utc.ms;
    return wsm;
}
char* ConvertBJT2UTC(struct tm* t2,const char* date){
     struct tm t;
     memset(&t,0,sizeof(t));
     t.tm_year = atoi(date)-1900;
     t.tm_mon = atoi(date+5)-1;
     t.tm_mday = atoi(date+8);
     //由于SYN记录为东八区时间，工控机系统时区需要为CST，否则t.tm_hour需要计算UTC时区差值
     t.tm_hour = atoi(date+11);
     //t.tm_hour = atoi(date+11);
     t.tm_min = atoi(date+14);
     t.tm_sec = atoi(date+17);
     //time_t tt = _mkgmtime64(&t);
     time_t tt = mktime(&t);

     if(tt != -1){
     if(t2 == NULL){
      t2 = &t;
     }
     //*t2 = *localtime(&tt);
     *t2 = *gmtime(&tt);
     char* ds = (char*) malloc(24);
     memset(ds, 0, 24);
     sprintf(ds, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", t2->tm_year + 1900,
      t2->tm_mon + 1, t2->tm_mday, t2->tm_hour, t2->tm_min,
      t2->tm_sec);
     return ds;
 }
 return NULL;
}
void UTC2GPS(int year, int month, int day, int hour, int minute, double second, int *weekNo, double *secondOfweek)
{
/*****协调世界时转换为GPS的周秒表示*****///输入时间应为协调世界时UTC，返回时间为GPS周和周秒
    int DayofYear = 0;
    int DayofMonth = 0;


    for (int i = 1980; i < year; i++)  //从1980年到当前年的上一年经过的天数
    {
        if ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0)
        DayofYear += 366;
        else
        DayofYear += 365;
    }

    for (int i = 1; i < month; i++)   //从一月到当前月的上一月经历的天数
    {
        if (i == 1 || i == 3 || i == 5 || i == 7 || i == 8 || i == 10 || i ==12)
            DayofMonth += 31;
        else if (i == 4 || i == 6 || i == 9 || i == 11)
            DayofMonth += 30;
        else
        {
            if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
            DayofMonth += 29;
            else
            DayofMonth += 28;
        }
    }
    int Day;
    Day = DayofMonth + day + DayofYear-6;
    *weekNo = Day/7;
    *secondOfweek = Day % 7 * 86400 + hour * 3600 + minute * 60 + second+18;//18表示跳秒
}
ECCS_END
