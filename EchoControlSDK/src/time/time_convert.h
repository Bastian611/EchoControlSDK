
#pragma once
#include <time.h>
#include "../global.h"
#include "../debug/Logger.h"

ECCS_BEGIN


//------------------------------------------------------
// Time structs and convert functions
//
// !!NOTE: 'leap_sec' below is the value read from GPS receiver, caution of SIGN!!
//------------------------------------------------------

struct WSM	/*week second millisecond*/
{
    u16 week;
    u32 sec;
    u16 ms;

    WSM();
    WSM(const u16& week, const u32& second, const u16& milisecond);

    void clear();
    bool isValid() const;
};

struct YMDHMSM	/*year month day hour minute second millisecond*/
{
    u16 year;
    u16 mon;
    u16 day;
    u16 hour;
    u16 min;
    u16 sec;
    u16 ms;

    YMDHMSM();
    YMDHMSM(struct tm t);
    YMDHMSM(const u16& y, const u16& M, const u16& d, const u16& h, const u16& m, const u16& s, const u16& milisecond);

    void clear();

    struct tm toTM() const;
    void setTime(struct tm t);
};

/**
 * @brief gps2local : 将GPS周秒转换成当地时间,根据UTC时区划分
 * @param week : GPS周
 * @param secInWeek : GPS周内秒
 * @param zone : UTC时区,可为正或负值
 * @param leap_sec : GPS跳秒
 * @param year : UTC年
 * @param mon : UTC月
 * @param day : UTC日
 * @param hour : UTC时
 * @param min : UTC分
 * @param sec : UTC秒
 */
void gps2local(
    const u16&	week,
    const u32&	secInWeek,
    const int&	zone,
    const int&	leap_sec,
    u16*		year,
    u16*		mon,
    u16*		day,
    u16*		hour,
    u16*		min,
    u16*		sec
    );
/**
 * @brief local2gps : 将当地时间转换成GPS周秒
 * @param year : UTC年
 * @param mon : UTC月
 * @param day : UTC日
 * @param hour : UTC时
 * @param min : UTC分
 * @param sec : UTC秒
 * @param zone: UTC时区,可为正或负值
 * @param leap_sec : GPS跳秒
 * @param week : GPS周
 * @param secInWeek : GPS周内秒
 */
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
    );
/**
 * @brief gps2utc:将GPS周秒转换成UTC时间
 * @param week
 * @param secInWeek
 * @param leap_sec
 * @param year
 * @param mon
 * @param day
 * @param hour
 * @param min
 * @param sec
 */
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
    );
/**
 * @brief utc2gps : 将utc时间转换成GPS周秒
 * @param year
 * @param mon
 * @param day
 * @param hour
 * @param min
 * @param sec
 * @param leap_sec
 * @param week
 * @param secInWeek
 */
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
    );

YMDHMSM gps2local(
    const WSM&	wsm,
    const int&	zone,
    const int&	leap_sec
    );
WSM local2gps(
    const YMDHMSM&	local,
    const int&		zone,
    const int&		leap_sec
    );
YMDHMSM gps2utc(
    const WSM&	wsm,
    const int&	leap_sec
    );
WSM utc2gps(
    const YMDHMSM&	utc,
    const int&		leap_sec
    );

/*******************************************************************
*函数名：ConvertBJT2UTC
*功能：将北京时间转换成UTC时间
*******************************************************************/
char* ConvertBJT2UTC(struct tm* t2,const char* date);
/*******************************************************************
*函数名：UTC2GPS
*功能：协调世界时转换为GPS的周秒表示
*******************************************************************/
void UTC2GPS(int year, int month, int day, int hour, int minute, double second, int *weekNo, double *secondOfweek);
ECCS_END
