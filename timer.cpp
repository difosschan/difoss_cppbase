#include "timer.h"
#include <signal.h>
#include <errno.h>
#include <algorithm>

#include "application.h"
#include "types.h"
#include "macro.h"
#include "ctime.h"
#include "mutex.h"
#include "cond_var.h"
#include "str_util.h"

using namespace std;

bool Timer::run() {
    EASY_LOCK(_lock);
    // 没有计时任务时，线程休眠
    if (_delays.empty()) {
        pause();
        return true;
    }

    Moment now;
    now.now();

    for (Timer::Delays_t::iterator ds_iter = _delays.begin();
                                   ds_iter != _delays.end();)
    {
        Moment alarm(_begin_time + ds_iter->first);
        if (now >= alarm) {
#ifdef _DEBUG
            COUT(ds_iter->first);
#endif // _DEBUG

            uint64_t id = ds_iter->second; // 在remove前把订阅ID存起来
            ++ds_iter;
            remove(id);  // NOTE: ds_iter 不能再用！
            this->action(id);

        } else {
            // 发现没到时间的定时器事件
            int r = CondVarHelper::Wait(_cond, _lock, alarm);
            if (Thread::STOPPED == _state) {
                return false;
            }
            switch (r) {
            case ETIMEDOUT:
            case EINVAL:
            default:
#ifdef _DEBUG
                cout << " CondVarHelper::Wait()  return " << StrUtil::ErrnoName(r) << endl;
#endif // _DEBUG
                break;
            }
            return true;
        }
    }


    return true;
}

Timer::Timer()
: Thread(true)
{}

Timer::~Timer() {
    EASY_LOCK(_lock);
}

bool Timer::add(Moment delay, uint64_t id) {
    if (delay.is_null())
        return false;

    EASY_LOCK(_lock);

    Moment now;
    now.now();

    // 记录【响铃时刻】，为防止本函数中的语句造成误差而延时
    Moment alarm = now + delay;
    Moment delay_from_begin = alarm - _begin_time;

    // COUT(alarm);
    // COUT(delay_from_begin);
    
    // 订阅计时器的id必须是唯一的
    if (_sub_ids.find(id) != _sub_ids.end())
        return false;
    
    bool need_settime = false;
    Moment min_delay;
    Delays_t::const_iterator min_delay_citer = _delays.begin();
    if (min_delay_citer != _delays.end()) {
       min_delay = min_delay_citer->first;
        if (delay_from_begin < min_delay)
            need_settime = true;
    } else {
        // 将插入第一条记录
        need_settime = true;
    }
    
    if (need_settime) {
        struct itimerspec ts;
        now.now(); // 再次刷新时间，为了减少误差。
        Moment delay_from_now = alarm - now;
        ts.it_value.tv_sec = ts.it_interval.tv_sec = delay_from_now.sec();
        ts.it_value.tv_nsec = ts.it_interval.tv_nsec = delay_from_now.usec() * 1000;

#ifdef _DEBUG
        cout << START_YELLOW << FL() << "ts=" << ts.it_value.tv_sec << ":" << ts.it_value.tv_nsec;
        if (ts.it_value.tv_sec && ts.it_value.tv_nsec)
            cout << ", " << alarm << " to alarm" << END_COLOR << endl;
        else
            cout << ", timer stop." << END_COLOR << endl;
#endif // _DEBUG

        start();
    }

    _delays.insert(make_pair(delay_from_begin, id));
    _sub_ids.insert(make_pair(id, delay_from_begin));
    
    return true;
}

bool Timer::remove(uint64_t id) {
    EASY_LOCK(_lock);

    SubscriberIDs_t::iterator t_iter = _sub_ids.find(id);
    if (t_iter == _sub_ids.end())
        return false;
    
    Delays_t::iterator ds_iter;
    do {
        ds_iter = _delays.find(t_iter->second);
        if (id == ds_iter->second)
            break;
    } while (ds_iter != _delays.end());
    
    if (ds_iter == _delays.end())
        return false;
    
    _sub_ids.erase(t_iter);
    _delays.erase(ds_iter);
    
    struct itimerspec ts;
    memset(&ts, 0, sizeof(ts));
    
    Delays_t::const_iterator new_beg = _delays.begin();
    if (new_beg != _delays.end()) {
        Moment delay_from_now = _begin_time + new_beg->first - Moment::Now();
        ts.it_value.tv_sec = ts.it_interval.tv_sec = delay_from_now.sec();
        ts.it_value.tv_nsec = ts.it_interval.tv_nsec = delay_from_now.usec() * 1000;
    }

#ifdef _DEBUG
    cout << START_YELLOW << FL() << "ts=" << ts.it_value.tv_sec << ":" << ts.it_value.tv_nsec;
    if (ts.it_value.tv_sec && ts.it_value.tv_nsec)
        cout << ", " << _begin_time + new_beg->first << " to alarm" << END_COLOR << endl;
    else
        cout << ", timer stop." << END_COLOR << endl;
#endif // _DEBUG

    return true;
}

