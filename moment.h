#pragma once

#ifdef WIN32
#include <time.h>   /* use for time_t */
#else
#include <sys/time.h>
#endif

#include <ostream>
#include "types.h"

// seconds in class 'Moment' is only 32bits second.
class Moment
{
public:
    Moment(time_t sec = 0, suseconds_t usec = 0);
    Moment(const char* cstr);
    Moment(const Moment& other);

    Moment& operator = (const Moment& other);

    bool operator == (const Moment& other) const;
    bool operator != (const Moment& other) const {return !( *this == other);}
    bool operator <  (const Moment& other) const;
    bool operator >  (const Moment& other) const {return (other < *this);}
    bool operator <= (const Moment& other) const {return !(*this > other);}
    bool operator >= (const Moment& other) const {return !(*this < other);}

    /* 注意：当“被减数”小于等于“减数”，都会导致返回的Moment值被置0。*/
    Moment operator - (const Moment& other) const;
    Moment operator + (const Moment& other) const;

    // 暂时只支持同一时区比较。
    Moment& operator -= (const Moment& other);
    Moment& operator += (const Moment& other);

    // 改运算只对Δ有意义。
    void divide_by(uint32_t piece);

    bool convertToMilliSeccond(uint32_t& msec) const;

    // getter
    time_t sec() const {return _sec;}
    suseconds_t usec() const {return _usec;}
    // setter
    void sec(time_t sec_) {_sec = sec_;}
    void usec(suseconds_t usec_) {_usec = usec_;}
    bool set(const char*);
    bool setUTC(const char*);
    
    void serializate(std::ostream& os) const;
    void serializate(char* out) const;
    static unsigned int serializationLength();

    // refresh time now.
    Moment& now();
    static Moment Now();

    // get MAXUMN time the class Moment can descripted.
    static Moment max();
    
    // set _sec and _usec to 0
    void null();
    // whether _sec and _usec equare 0.
    bool is_null() const;

    std::ostream& operator<< (std::ostream & os) const;

protected:
    time_t      _sec;
    suseconds_t _usec;
};

std::ostream& operator<< (std::ostream & os, const Moment& one) ;
