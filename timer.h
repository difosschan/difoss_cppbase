#pragma once
extern "C" {
#   include <time.h>
}
#include <map>
#include <set>
#include "moment.h"
#include "thread.h"

class Timer : protected Thread {
    
public:
    Timer();
    virtual ~Timer();
    
    bool add(Moment delay, uint64_t id);
    bool remove(uint64_t id);
    
protected:
    virtual void action(uint64_t id) = 0;

private:
    bool run();
    
private:
    typedef std::set<int>                   SignalNo_t;
    typedef Moment                          DelayTime_t;
    typedef std::multimap<DelayTime_t, uint64_t> Delays_t;
    typedef std::map<uint64_t, DelayTime_t>      SubscriberIDs_t;
    
    int             _signo;
    Delays_t        _delays;
    SubscriberIDs_t _sub_ids;
    
    Moment          _begin_time;
};
