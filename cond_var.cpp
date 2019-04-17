#include "cond_var.h"

/* static */ int CondVar::Helper::Wait(pthread_cond_t& cnd, pthread_mutex_t& mtx, const Moment& alarm) {
    struct timespec timeout;
    timeout.tv_sec = alarm.sec();
    timeout.tv_nsec = alarm.usec() * 1000;

    return pthread_cond_timedwait(&cnd, &mtx, &timeout);
}

/* static */ int CondVar::Helper::WaitIn(pthread_cond_t& cnd, pthread_mutex_t& mtx, const Moment& interval) {
    Moment dest;
    dest.now();
    dest += interval;

    return CondVar::Helper::Wait(cnd, mtx, dest);
}
