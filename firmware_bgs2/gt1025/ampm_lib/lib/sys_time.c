#include "sys_time.h"
#include "m_time.h"
#include <time.h>


DATE_TIME sysTime;
uint32_t rtcTimeSec = 0;
uint32_t timeZone = TIME_ZONE*3600;
uint32_t sysTimeZone = TIME_ZONE*3600;
uint32_t rtcUpdateFlag = 0;
uint32_t rtcUpdateCnt = 0;

void SysTimeSetup(void)
{
	sysTime.hour = 18;
	sysTime.min = 15;
	sysTime.sec = 0;
	sysTime.year = 2014;
	sysTime.month = 4;
	sysTime.mday = 19;
	rtcTimeSec = TIME_GetSec(&sysTime);
}


void UpdateRtcTime(uint32_t updateTimeSec)
{
    if(rtcTimeSec < updateTimeSec)
		{
			if((updateTimeSec - rtcTimeSec) >= 10)
				rtcUpdateFlag = 1;
		}
		else if(rtcTimeSec - updateTimeSec >= 10)
			rtcUpdateFlag = 1;
		rtcUpdateCnt = updateTimeSec;
}


int8_t TIME_FromGsm(DATE_TIME *t, DATE_TIME *time)
{
	struct tm tm;
	time_t tt;
	tm.tm_sec = time->sec;
	tm.tm_min = time->min;
	tm.tm_hour = time->hour;
	tm.tm_mday = time->mday;
	tm.tm_wday = time->wday;
	if((time->month >= 1) && (time->month <= 12))
		tm.tm_mon = time->month - 1;
	else return 0;
	if((time->year >= 2012) && (time->year <= 2099))
		tm.tm_year = time->year - 1900;
	else return 0;
	tm.tm_isdst = 0;
	// convert UTC time to number of seconds since 00:00 hours, Jan 1, 1970 UTC
	tt = mktime(&tm);
	// change to localtime by adding TIMEZONE
	tt += timeZone;
	// convert back to UTC time struct
        #if defined ( __ICCARM__ )
        tm = *gmtime(&tt);
        #elif defined (__CC_ARM)
        gmtime_r(&tt,&tm);
        #endif
	
	t->sec = tm.tm_sec;
	t->min = tm.tm_min;
	t->hour = tm.tm_hour;
	t->mday = tm.tm_mday;
	t->wday = tm.tm_wday + 1;			// tm_wday 0 - 6 (0 = sunday)
	t->month = tm.tm_mon + 1;			// tm_mon 0 - 11 (0 = Jan)
	t->year = tm.tm_year + 1900;		// tm_year = current year - 1900
	
	return 0;
}

int8_t TIME_FromGps(DATE_TIME *t, nmeaINFO *nmeaInfo)
{
	struct tm tm;
        #if defined ( __ICCARM__ )
        struct tm *gt;
        #endif
	time_t tt;
	tm.tm_sec = nmeaInfo->utc.sec;
	tm.tm_min = nmeaInfo->utc.min;
	tm.tm_hour = nmeaInfo->utc.hour;
	tm.tm_mday = nmeaInfo->utc.day;
	tm.tm_mon = nmeaInfo->utc.mon;
	tm.tm_year = nmeaInfo->utc.year;
	tm.tm_isdst = 0;
	// convert UTC time to number of seconds since 00:00 hours, Jan 1, 1970 UTC
	tt = mktime(&tm);
	// change to localtime by adding TIMEZONE
	tt += timeZone;
	// convert back to UTC time struct
	#if defined ( __ICCARM__ )
        gt = gmtime(&tt);
	t->sec = gt->tm_sec;
	t->min = gt->tm_min;
	t->hour = gt->tm_hour;
	t->mday = gt->tm_mday;
	t->wday = gt->tm_wday + 1;			// tm_wday 0 - 6 (0 = sunday)
	t->month = gt->tm_mon + 1;			// tm_mon 0 - 11 (0 = Jan)
	t->year = gt->tm_year + 1900;		// tm_year = current year - 1900
        #elif defined (__CC_ARM)
        gmtime_r(&tt,&tm);
	t->sec = tm.tm_sec;
	t->min = tm.tm_min;
	t->hour = tm.tm_hour;
	t->mday = tm.tm_mday;
	t->wday = tm.tm_wday + 1;			// tm_wday 0 - 6 (0 = sunday)
	t->month = tm.tm_mon + 1;			// tm_mon 0 - 11 (0 = Jan)
	t->year = tm.tm_year + 1900;		// tm_year = current year - 1900
	 #endif
	return 0;
}
// add number of secs to a DATE_TIME struct
int8_t TIME_AddSec(DATE_TIME *t, int32_t sec)
{
	if(t == NULL) return 0xff;
	if(sec == 0) return 0;
	
	TIME_FromSec(t, TIME_GetSec(t) + sec);
	
	return 0;
}
// find the number of sec since 00:00 hours, Jan 1, 1970 UTC
int32_t TIME_GetSec(DATE_TIME *t)
{
	struct tm tx;
	if(t == NULL) return -1;
	
	 tx.tm_sec = t->sec;
	 tx.tm_min = t->min;
	 tx.tm_hour = t->hour;
	 tx.tm_mday = t->mday;
	 tx.tm_wday = t->wday;
	if((t->month >= 1) && (t->month <= 12))
		tx.tm_mon = t->month - 1;
	else return 0;
	if((t->year >= 2012) && (t->year <= 2099))
		tx.tm_year = t->year - 1900;
	else return 0;
	 tx.tm_isdst = 0;
	
	return mktime(&tx);
}
int8_t TIME_FromSec(DATE_TIME *t, int32_t sec)
{
	struct tm tm;
        struct tm *gt;
	if(t == NULL) return 0xff;
	#if defined ( __ICCARM__ )
        gt = gmtime((const time_t *)&sec);
        t->sec = gt->tm_sec;
	t->min = gt->tm_min;
	t->hour = gt->tm_hour;
	t->mday = gt->tm_mday;
	t->wday = gt->tm_wday + 1;			// tm_wday 0 - 6 (0 = sunday)
	t->month = gt->tm_mon + 1;			// tm_mon 0 - 11 (0 = Jan)
	t->year = gt->tm_year + 1900;		// tm_year = current year - 1900
        #elif defined (__CC_ARM)
        gmtime_r((const time_t *)&sec,&tm);
        t->sec = tm.tm_sec;
        t->min = tm.tm_min;
        t->hour = tm.tm_hour;
        t->mday = tm.tm_mday;
        t->wday = tm.tm_wday + 1;			// tm_wday 0 - 6 (0 = sunday)
        t->month = tm.tm_mon + 1;			// tm_mon 0 - 11 (0 = Jan)
        t->year = tm.tm_year + 1900;		// tm_year = current year - 1900
        #endif
	return 0;
}

