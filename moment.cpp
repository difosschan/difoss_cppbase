#include "moment.h"
#include "macro.h"
#include "ctime.h"
#include <string>

using namespace std;

static const size_t Len_YYYYMMDD_HHmmSS_xxxxxx = sizeof("YYYYMMDD_HHmmSS_xxxxxx") / sizeof(char) - 1;
static const size_t Len_YYYYMMDD_HHmmss = sizeof("YYYYMMDD:HHmmss") / sizeof(char) - 1;
// 432000 = 5 * 24 * 60 * 60，大于5年的时间认为是“时间”，否则认为是“时刻”，此规定只用于打印
#define DELTA_SEC_MAX 432000

Moment::Moment(time_t sec, suseconds_t usec)
: _sec(sec)
, _usec(usec) {}

Moment::Moment(const Moment& other)
: _sec(other._sec)
, _usec(other._usec) {}

/*static*/ Moment Moment::max() {
    return Moment(-1, 999999);
}


Moment::Moment(const char* cstr) 
: _sec(0)
, _usec(0) {
    set(cstr);
}

bool Moment::set(const char* cstr) {
    size_t len = strlen(cstr);
    if (len < Len_YYYYMMDD_HHmmss)
        return false;

    int year = StrUtil::ToInt(string(cstr).substr(0, 4));
    int month = StrUtil::ToInt(string(cstr).substr(4, 2));
    int day = StrUtil::ToInt(string(cstr).substr(6, 2));
    int hour = StrUtil::ToInt(string(cstr).substr(9, 2));
    int min = StrUtil::ToInt(string(cstr).substr(11, 2));
    int sec = StrUtil::ToInt(string(cstr).substr(13, 2));
    CTime t;
    if (!t.set(year, month, day, hour, min, sec))
        return false;
    this->_sec = t.GetTimet();

    if (len > Len_YYYYMMDD_HHmmss + 1) {
        this->_usec = StrUtil::ToInt(string(cstr).substr(Len_YYYYMMDD_HHmmss + 1));
    }
    return true;
}

bool Moment::setUTC(const char* cstr) {
    if (! set(cstr))
        return false;
    _sec += CTime().GetGMTOff();
    return true;
}

Moment& Moment::operator =(const Moment& other) {
    if (&other != this) {
        this->_sec = other._sec;
        this->_usec = other._usec;
    }
    return *this;
}

bool Moment::operator==( const Moment& other ) const {
    if (&other == this)
        return true;
    return (this->_sec == other._sec) && (this->_usec == other._usec);
}

bool Moment::operator<( const Moment& other ) const {
    return (this->_sec < other._sec)
        || ((this->_sec == other._sec) && (this->_usec < other._usec));
}

void Moment::null() {
    _sec = 0;
    _usec = 0;
}

bool Moment::is_null() const {
    return (0==_sec) && (0==_usec);
}

Moment Moment::operator-( const Moment& other ) const
{
    if (this->_sec < other._sec)
        return Moment();

    Moment res(this->_sec - other._sec);
    suseconds_t tmp_usec = static_cast<suseconds_t>(this->_usec) - static_cast<suseconds_t>(other._usec);
    if (tmp_usec < 0) { // 借位
        if (res._sec) {
            --res._sec;
            tmp_usec += USEC_PER_SEC;
            res._usec = static_cast<suseconds_t>(tmp_usec);
        } else {
            tmp_usec = 0;
            res._usec = 0;
        }
    } else {
        res._usec = static_cast<suseconds_t>(tmp_usec);
    }
    return res;
}

Moment Moment::operator+( const Moment& other ) const
{
    Moment res(*this);
    res._sec += other._sec;

    time_t tUsec = res._usec;
    tUsec += other._usec;

    if (tUsec >= USEC_PER_SEC) {
        res._sec += 1;
        tUsec %= USEC_PER_SEC;
    }
    res._usec = tUsec;

    return res;
}

Moment&
Moment::operator -= (const Moment& other)
{
    uint64_t tSec = _sec;
    tSec += other._sec;

    uint64_t tUSec = _usec;
    tUSec += other._usec;

    if (_sec >= other._sec)
    {
        // 暂时处理了 sec 。
        _sec -= other._sec;

        // usec
        if (_usec >= other._usec)  // 不用借位
            _usec -= other._usec;
        else
        {
            if (_sec)    // Δsec 还可以借
            {
                _sec--;
                _usec += (USEC_PER_SEC - other._usec);
            }
            else    // Δsec 不够借
            {
                _usec = 0;
            }
        }
    }
    else
    {
        _sec = 0;
        _usec = 0;
    }

    return *this;
}

Moment&
Moment::operator += (const Moment& other)
{
    int64_t tSec = (*this)._sec;
    tSec += other._sec;

    int64_t tUSec = (*this)._usec;
    tUSec += other._usec;

    if (tSec > (time_t)(-1))    // sec 和 越界
    {
        _sec = (time_t)-1;
        _usec = 999999;
    }
    else
    {
        if (tUSec >= USEC_PER_SEC)
        {
            tSec += 1;
            tUSec %= USEC_PER_SEC;
        }
        _sec = (time_t)tSec;
        (*this)._usec = (suseconds_t)tUSec;
    }

    return *this;
}

// 可以改成除法运算符，不过只对Δ有意义。
void Moment::divide_by(uint32_t piece)
{
    if (is_null())
        return;

    if (0 == piece)
        return;

    uint64_t sec_div = _sec / piece;         // 秒的可以整除部分
    uint64_t sec_undiv = _sec % piece;       // 秒的余数部分
    // 除法运算后的 微秒 = 微秒的可整除部分（余数部分省略） +  秒的余数部分（转换成微秒） / piece
    uint64_t usec_div = static_cast<uint64_t>(_usec / piece) + (sec_undiv * USEC_PER_SEC / piece);
    sec_div += usec_div / USEC_PER_SEC;     // 微秒运算后可能会产生大于1秒的结果，需要进位

    _sec = sec_div;
    _usec = static_cast<uint32_t>(usec_div % USEC_PER_SEC);
}

bool Moment::convertToMilliSeccond(uint32_t & msec) const
{
    uint32_t maxUL = (uint32_t) -1;
    if ( static_cast<uint32_t>(_sec) > maxUL / 1000)
        return false;

    msec = _sec * 1000 + (_usec % (1000* 1000)) / 1000;
    return true;
}

Moment& Moment::now()
{
    struct timeval st_time;
    memset(&st_time, 0, sizeof(st_time));
    gettimeofday(&st_time, NULL);
    _sec = st_time.tv_sec;
    _usec = st_time.tv_usec;
    return *this;
}

/*static*/ Moment Moment::Now() {
    static Moment tmp;
    return tmp.now();
}

ostream& Moment::operator<<( ostream & os ) const
{
    CTime t(static_cast<time_t>(_sec));
    if (_sec >= DELTA_SEC_MAX)
        os << t << "(" << _sec << "):" << _usec;
    else
        os << _sec << ":" << _usec;
    return os;
}

ostream& operator<<( ostream & os, const Moment& one )
{
    one.operator<<(os);
    return os;
}
