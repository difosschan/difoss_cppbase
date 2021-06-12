#include "thread.h"

extern "C" {
#   include <unistd.h>
#   include <sys/syscall.h>
#   include <errno.h>
};

#include "mutex.h"
#include "macro.h"
#include "color_print.h"
#include <iostream>
#include "str_util.h"

void* Thread::realThreadProc( void* myself )
{
    TRACE();
    Thread* me = ((Thread*)myself);

    long tid = static_cast<long>(syscall(SYS_gettid));

    pthread_mutex_lock(&(me->_lock));

    if (Thread::UNINITED == me->_state) {
        pthread_mutex_unlock(&(me->_lock));
        printf(START_YELLOW "_state == %s.\n" END_COLOR, Thread::State_get_name(me->_state));
        return NULL;
    }
    if (Thread::PAUSED == me->_state) {
        printf(START_YELLOW "pthread_cond_wait...\n" END_COLOR);
        pthread_cond_wait(&(me->_cond), &(me->_lock));
        printf(START_YELLOW "weakup...\n" END_COLOR);
    }

    int state = me->_state;
    thread_callback_t pCallback = me->_callback;
    void* param = me->_param;
    pthread_mutex_unlock(&(me->_lock));

    while (Thread::STOPPED != state) {
        // <debug>
        // printf("before thread run [tid=%ld].\n", tid);
        if (pCallback) {
            if (0 != (*pCallback)(param)) {
                break;
            }
        }
        else {
            if (! me->run()) {
                break;
            }
        }

        pthread_mutex_lock(&(me->_lock));
        if (Thread::PAUSED == me->_state)
            pthread_cond_wait(&(me->_cond), &(me->_lock));
        pthread_mutex_unlock(&(me->_lock));

        // ˢ�¶����״̬
        pthread_mutex_lock(&(me->_lock));
        state = me->_state;
        pthread_mutex_unlock(&(me->_lock));
    }
    pthread_mutex_lock(&(me->_lock));
    me->_state = DEAD;
    pthread_mutex_unlock(&(me->_lock));
    printf(START_RED "end of realThreadProc" END_COLOR "[tid=%ld].\n", tid);
    return NULL;
}

Thread::Thread( bool isPaused /*= false*/ )
    : _state(Thread::UNINITED)
    , _thread_id(pthread_t(0))
    , _callback(NULL)
    , _param(NULL)
{
    TRACE();
    MutexHelper::init(_lock, Mutex::RECURSIVE);
    if (0 != pthread_cond_init(&_cond, NULL))
        return;

    if (isPaused) {
        _state = PAUSED;
    }

    EASY_LOCK(_lock);
    if (0 != pthread_create(&_thread_id, NULL, Thread::realThreadProc, (void*)this)) {
        perror(START_UNDERLINE "pthread_create error.\n" END_COLOR);
        if (isPaused)
            _state = UNINITED;
        return;
    }

    if (! isPaused) {
        _state = INITED;
    }
    printf("thread{%s} create success: _state = %s.\n",
           StrUtil::ToString( (uint64_t)_thread_id ).c_str(),
           State_get_name(_state));
}

/*virtual*/ Thread::~Thread() {
    TRACE();
    join();
    while (pthread_cond_destroy(&_cond) == EBUSY)
    {
        pthread_cond_broadcast(&_cond);
    }
    pthread_mutex_destroy(&_lock);
}

/*virtual*/ void Thread::start()
{
    printf("thread{%s} start.\n", StrUtil::ToString( (uint64_t)_thread_id ).c_str());

    EASY_LOCK(_lock);
    OUTPUT(State_get_name(_state));
    if (Thread::PAUSED == _state) {
        _state = RUNNING;
        pthread_cond_signal(&_cond);
        printf("in start() pthread_cond_signal.\n");
    } else {
        printf(START_UNDERLINE "in start() _state(%s) != PAUSED.\n" END_COLOR, State_get_name(_state));
    }
}

/*virtual*/ void Thread::pause()
{
    EASY_LOCK(_lock);
    OUTPUT(State_get_name(_state));
    if (Thread::RUNNING == _state) {
        _state = Thread::PAUSED;
    }
}

/*virtual*/ void Thread::stop()
{
    pthread_mutex_lock(&_lock);
    OUTPUT(State_get_name(_state));
    if (   _state != STOPPED
        && _state != UNINITED
        && _state != DEAD) {
        _state = STOPPED;
    }
    pthread_cond_broadcast(&_cond);
    pthread_mutex_unlock(&_lock);
}


/*virtual*/ void Thread::join()
{
    OUTPUT(State_get_name(_state));
    stop();
    if (_thread_id) {
        pthread_join(_thread_id, NULL);
        _thread_id = 0;
    }
}

thread_callback_t Thread::setCallback( thread_callback_t func, void* param )
{
    TRACE();
    thread_callback_t old = _callback;
    _callback = func;
    _param = param;
    return old;
}

bool Thread::run()
{
    return false;
}

int Thread::getState()
{
    EASY_LOCK(_lock);
    return _state;
}



//////////////////////////////////////////////////////////////////////////
