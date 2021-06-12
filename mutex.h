#pragma once

#include "macro.h"
#include <pthread.h>

class Mutex
{
public:

    //////////////////////////////////////////////////////////////////////////
    // Mutex::Attribute 属性设置
    typedef enum {
        TIMED = 0,      // 普通锁。
        RECURSIVE = 1,  // 嵌套锁，允许同一个线程对同一个锁成功获得多次，并通过多次unlock解锁。
        ERRORCHECK = 2, // 检错锁，同一线程也只能申请一次锁。
        ADAPTIVE = 3    // 适应锁，仅等待解锁后重新竞争。
    } Type;

    Mutex(Mutex::Type type = Mutex::TIMED);
    ~Mutex();

public:
    int tryLock();
    int lock();
    int unlock();

private:
    pthread_mutex_t _mtx;
    

public:
    
    //////////////////////////////////////////////////////////////////////////
    // Mutex::Helper 作用：辅佐 Mutex 和 pthread_mutex_t 的使用
    class Helper
    {
    public:
        // 帮助在函数退出时解锁。
        Helper(Mutex& lock);
        Helper(pthread_mutex_t& lock);
        virtual ~Helper();

    public:
        // 帮助pthread_mutex_t的初始化。
        static void init(pthread_mutex_t& lock, Mutex::Type type = Mutex::TIMED);
        static void destroy(pthread_mutex_t& lock);

    private:
        Mutex*              _pMutex;
        pthread_mutex_t*    _pMtx;
    }; /* Helper */
}; /* Mutex */

#ifndef EASY_LOCK
#define EASY_LOCK(_mtx) MutexHelper BOOST_PP_CAT(helper_,__LINE__) (_mtx)
#endif /* EASY_LOCK */

typedef Mutex::Helper MutexHelper;
