#pragma once

#include "mutex.h"
#include "moment.h"

//! Encapsulate a condition variable for thread synchronization.
class CondVar {
public:
    class Helper {
    public:
        static int WaitIn(pthread_cond_t& cnd, pthread_mutex_t& mtx, const Moment& interval);
        static int Wait(pthread_cond_t& cnd, pthread_mutex_t& mtx, const Moment& alarm);
    };
};

typedef CondVar::Helper CondVarHelper;
