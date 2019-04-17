#pragma once

#include <ostream>
extern "C" {
#include <time.h>
#ifndef _MSC_VER
#include <sys/time.h>  // time_t
#endif
};

/**
 * @brief 时间操作的类
 */
class CTime
{
public:
    CTime();
    CTime(time_t time);
    CTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec);
    ~CTime();

    bool set(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec);

    int GetYear() const;
    int GetMonth() const;
    int GetDay() const;
    int GetHour() const;
    int GetMinute() const;
    int GetSecond() const;
    int GetDayOfWeek() const;

    void Format(char* strOut, const char* strFormat) const;
    int GetInterval(CTime preTime) const;
    void PassSecond(int iSecond);

    time_t GetTimet() const;
    void SetTimet(time_t timet);

    void ZeroClock();
    static time_t ZeroClock(time_t);

    void UTCZeroClock();
    static time_t UTCZeroClock(time_t);

    void RefreshNow();

    // @brief 返回比UTC时间多的秒数
    int GetGMTOff();

    // @brief UTC时间相关接口
    int GetUTCYear() const;
    int GetUTCMonth() const;
    int GetUTCDay() const;
    int GetUTCHour() const;
    int GetUTCMinute() const;
    int GetUTCSecond() const;
    int GetUTCDayOfWeek() const;
    void UTCFormat(char* strOut, const char* strFormat) const;

    // @brief 返回形如“20150327163700”的无符号64位整数
    unsigned long long ToUint64() const;

    // @brief 从形如“20150327163700”的无符号64位整数表示的时间填充成员
    bool set(unsigned long long ull);

	friend std::ostream& operator<<(std::ostream&, const CTime&);

    std::string ToString() const;

    bool operator < (const CTime&) const;
    bool operator > (const CTime&) const;
    bool operator == (const CTime&) const;
    bool operator != (const CTime&) const;
    bool operator <= (const CTime&) const;
    bool operator >= (const CTime&) const;

protected:
    time_t m_time; ///< 时间结构

};
