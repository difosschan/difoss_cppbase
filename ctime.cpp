/**
 * @file class CTime 实现文件
 * Version:        01.01.001
 * Date:           2003/7/30
 */

#include "ctime.h"

extern "C" {
#include <stdio.h>
#include <assert.h>
#include <string.h>
};
//////////////////////////////////////////////////////////////////////////
#define TM_2_YEAR(_tm_)     (_tm_).tm_year + 1900
#define TM_2_MONTH(_tm_)    (_tm_).tm_mon + 1
#define TM_2_DAY(_tm_)      (_tm_).tm_mday
#define TM_2_HOUR(_tm_)     (_tm_).tm_hour
#define TM_2_MINUTE(_tm_)   (_tm_).tm_min
#define TM_2_SECOND(_tm_)   (_tm_).tm_sec
#define TM_2_WDAY(_tm_)     (_tm_).tm_wday

#ifndef ULL
#define ULL(A) A ## ULL
#endif

#ifndef LL
#define LL(A) A ## LL
#endif
//////////////////////////////////////////////////////////////////////////

/**
 * @brief 构造函数
 *
 */
CTime::CTime()  
{
    // 设定默认的时间为当前时间
    RefreshNow();
}

/**
 * @brief 构造函数
 * @param time 输入时间
 */
CTime::CTime(time_t time) : m_time(time)
{
}

/**
 * @brief 构造函数
 * @param nYear 年
 * @param nYear 月
 * @param nYear 日  
 * @param nYear 小时
 * @param nYear 分钟
 * @param nYear 秒 
 */
CTime::CTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec)
{
    set(nYear, nMonth, nDay, nHour, nMin, nSec);
}

bool CTime::set(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec) {
    if (   ! (nDay >= 1 && nDay <= 31)
        || !(nMonth >= 1 && nMonth <= 12)
        || !(nYear >= 1900))
        return false;
    
	struct tm atm;
    memset(&atm, 0, sizeof(atm));
	atm.tm_sec = nSec;
	atm.tm_min = nMin;
	atm.tm_hour = nHour;
	atm.tm_mday = nDay;
	atm.tm_mon = nMonth - 1;        // tm_mon is 0 based
	atm.tm_year = nYear - 1900;     // tm_year is 1900 based
    
	m_time = mktime(&atm);
    return true;
}

/**
 * @brief 析构函数
 */
CTime::~CTime()
{
}

/**
 * @brief 取年份
 */
int CTime::GetYear() const
{
	struct tm tb;
    memcpy(&tb, localtime(&m_time), sizeof(struct tm));
	return TM_2_YEAR(tb);
}

/**
 * @brief 取月份
 */
int CTime::GetMonth() const
{
	struct tm tb;
    memcpy(&tb, localtime(&m_time), sizeof(struct tm));
	return TM_2_MONTH(tb);
}

/**
 * @brief 取日期
 */
int CTime::GetDay() const
{
	struct tm tb;
    memcpy(&tb, localtime(&m_time), sizeof(struct tm));
	return TM_2_DAY(tb);
}

/**
 * @brief 取小时
 */
int CTime::GetHour() const
{
	struct tm tb;
    memcpy(&tb, localtime(&m_time), sizeof(struct tm));
	return TM_2_HOUR(tb);
}

/**
 * @brief 取分钟
 */
int CTime::GetMinute() const
{
	struct tm tb;
    memcpy(&tb, localtime(&m_time), sizeof(struct tm));
	return TM_2_MINUTE(tb);
}

/**
 * @brief 取秒
 */
int CTime::GetSecond() const
{
	struct tm tb;
    memcpy(&tb, localtime(&m_time), sizeof(struct tm));
	return TM_2_SECOND(tb);
}

/**
 * @brief 取星期
 */
int CTime::GetDayOfWeek() const
{
	struct tm tb;
    memcpy(&tb, localtime(&m_time), sizeof(struct tm));
	return TM_2_WDAY(tb);
}

/**
 * @brief 设定输出的时间格式
 *       #Y - year
 *       #M - month
 *      #D - day
 *      #H - hour in 24 hour format
 *       #m - minute (0-59)
 *      #S - seconds (0-59)
 *      ## - '#' sign
 */
void CTime::Format(char* strOut, const char* strFormat) const
{
	char ch;

	struct tm tb;
    memcpy(&tb, localtime(&m_time), sizeof(struct tm));
	while((ch = *strFormat++) != 0)
	{
		if (ch == '#')
		{
			switch (ch = *strFormat++)
			{
				case 'Y' :
					strOut += sprintf(strOut, "%04d", TM_2_YEAR(tb));
					break;
				case 'M' :
					strOut += sprintf(strOut, "%02d", TM_2_MONTH(tb));
					break;
				case 'D' :
					strOut += sprintf(strOut, "%02d", TM_2_DAY(tb));
					break;
				case 'H' :
					strOut += sprintf(strOut, "%02d", TM_2_HOUR(tb));
					break;
				case 'm' :
					strOut += sprintf(strOut, "%02d", TM_2_MINUTE(tb));
					break;
				case 'S' :
					strOut += sprintf(strOut, "%02d", TM_2_SECOND(tb));
					break;
				default:
			        *strOut++ = ch;
			}
		}
		else
			*strOut++ = ch;
	}
	*strOut = 0;
}

/**
 * @brief 取两个时间的间隔
 */
int CTime::GetInterval(CTime preTime) const
{
	return m_time - preTime.GetTimet();
}

/**
 * @brief 取时间
 */
time_t CTime::GetTimet() const
{
	return m_time;
}

/**
 * @brief 设置时间
 */
void CTime::SetTimet(time_t timet)
{
	m_time = timet;
}

/**
 * @brief 在当前时间增加iSecond秒
 */
void CTime::PassSecond(int iSecond)
{
	m_time = m_time + iSecond;
}

/**
 * @brief 设置时间到当前日期的00:00:00
 */
void CTime::ZeroClock()
{
    m_time = CTime::ZeroClock(m_time);
}

time_t CTime::ZeroClock( time_t tt )
{
    struct tm tb;
    memcpy(&tb, localtime(&tt), sizeof(struct tm));
    tb.tm_hour = 0;
    tb.tm_min = 0;
    tb.tm_sec = 0;

    tt = mktime(&tb);
    return tt;
}

void CTime::UTCZeroClock()
{
    m_time = CTime::UTCZeroClock(m_time);
}

time_t CTime::UTCZeroClock( time_t tt )
{
    return tt - (tt % 86400);
}

int CTime::GetGMTOff()
{
    struct tm tb;
    memcpy(&tb, localtime(&m_time), sizeof(struct tm));
    return tb.tm_gmtoff;
}



int CTime::GetUTCYear() const
{
    struct tm tb;
    memcpy(&tb, gmtime(&m_time), sizeof(struct tm));
    return TM_2_YEAR(tb);
}

int CTime::GetUTCMonth() const
{
    struct tm tb;
    memcpy(&tb, gmtime(&m_time), sizeof(struct tm));
    return TM_2_MONTH(tb);
}

int CTime::GetUTCDay() const
{
    struct tm tb;
    memcpy(&tb, gmtime(&m_time), sizeof(struct tm));
    return TM_2_DAY(tb);
}

int CTime::GetUTCHour() const
{
    struct tm tb;
    memcpy(&tb, gmtime(&m_time), sizeof(struct tm));
    return TM_2_HOUR(tb);
}

int CTime::GetUTCMinute() const
{
    struct tm tb;
    memcpy(&tb, gmtime(&m_time), sizeof(struct tm));
    return TM_2_MINUTE(tb);
}

int CTime::GetUTCSecond() const
{
    struct tm tb;
    memcpy(&tb, gmtime(&m_time), sizeof(struct tm));
    return TM_2_MINUTE(tb);
}

int CTime::GetUTCDayOfWeek() const
{
    struct tm tb;
    memcpy(&tb, gmtime(&m_time), sizeof(struct tm));
    return TM_2_WDAY(tb);
}


// 设定时间为当前时间
void CTime::RefreshNow()
{
    m_time = time(NULL);
}

std::ostream& operator<<(std::ostream& os, const CTime& self) {
    char buf[32] = {0};
    self.Format(buf, "#Y-#M-#D #H:#m:#S");
    os << buf;
#if defined(_DEBUG_CTIME_UTC)
    os << "/";
    self.UTCFormat(buf, "#Y-#M-#D #H:#m:#S");
    os << buf;
#endif
    return os;
}

std::string CTime::ToString() const {
	char p[32];
	strftime(p, sizeof(p), "%F %T", localtime(&m_time));
    return p;
}

void CTime::UTCFormat( char* strOut, const char* strFormat ) const
{
    char ch;

    struct tm tb;
    memcpy(&tb, gmtime(&m_time), sizeof(struct tm));
    while((ch = *strFormat++) != 0)
    {
        if (ch == '#')
        {
            switch (ch = *strFormat++)
            {
            case 'Y' :
                strOut += sprintf(strOut, "%04d", TM_2_YEAR(tb));
                break;
            case 'M' :
                strOut += sprintf(strOut, "%02d", TM_2_MONTH(tb));
                break;
            case 'D' :
                strOut += sprintf(strOut, "%02d", TM_2_DAY(tb));
                break;
            case 'H' :
                strOut += sprintf(strOut, "%02d", TM_2_HOUR(tb));
                break;
            case 'm' :
                strOut += sprintf(strOut, "%02d", TM_2_MINUTE(tb));
                break;
            case 'S' :
                strOut += sprintf(strOut, "%02d", TM_2_SECOND(tb));
                break;
            default:
                *strOut++ = ch;
            }
        }
        else
            *strOut++ = ch;
    }
    *strOut = 0;
}

unsigned long long CTime::ToUint64() const {
    unsigned long long result = 0;

    struct tm tb;
    memcpy(&tb, localtime(&m_time), sizeof(struct tm));
    result  = (TM_2_YEAR(tb)) * ULL(10000000000);
    result += (TM_2_MONTH(tb)) * ULL(100000000);
    result += (TM_2_DAY(tb)) * ULL(1000000);
    result += (TM_2_HOUR(tb)) * ULL(10000);
    result += (TM_2_MINUTE(tb)) * ULL(100);
    result += TM_2_SECOND(tb);

    return result;
}

bool CTime::set(unsigned long long ull) {
    int year   = (ull / ULL(10000000000)) % 10000;
    int month  = (ull / ULL(100000000)) % 100;
    int day    = (ull / ULL(1000000)) % 100;
    int hour   = (ull / ULL(10000)) % 100;
    int minute = (ull / ULL(100)) % 100;
    int second = ull % 100;
    return set(year, month, day, hour, minute, second);
}

bool CTime::operator < (const CTime& o) const {
    return m_time < o.m_time;
}

bool CTime::operator > (const CTime& o) const {
    return m_time > o.m_time;
}

bool CTime::operator == (const CTime& o) const {
    return m_time == o.m_time;
}

bool CTime::operator != (const CTime& o) const {
    return m_time != o.m_time;
}

bool CTime::operator <= (const CTime& o) const {
    return m_time <= o.m_time;
}

bool CTime::operator >= (const CTime& o) const {
    return m_time >= o.m_time;
}
