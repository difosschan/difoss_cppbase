// Mutex.cpp
#include "mutex.h"

Mutex::Mutex(Mutex::Type type /* = Mutex::TIMED */)
{
    Helper::init(_mtx, type);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&_mtx);
}

int
Mutex::tryLock()
{
    return pthread_mutex_trylock(&_mtx);
}

int
Mutex::lock()
{
    return pthread_mutex_lock(&_mtx);
}

int
Mutex::unlock()
{
    return pthread_mutex_unlock(&_mtx);
}

//////////////////////////////////////////////////////////////////////////
Mutex::Helper::Helper(Mutex& lock)
:_pMutex(&lock)
,_pMtx(NULL)
{
    lock.lock();
}

Mutex::Helper::Helper(pthread_mutex_t& lock)
:_pMutex(NULL)
,_pMtx(&lock)
{
    pthread_mutex_lock(_pMtx);
}

/* virtual */
Mutex::Helper::~Helper()
{
    // 两个指针的单一性由构造函数保证。
    if (_pMutex)
        _pMutex->unlock();
    else if(_pMtx)
        pthread_mutex_unlock(_pMtx);
}

/* static */
void Mutex::Helper::init(pthread_mutex_t& lock, Mutex::Type type /* = Mutex::TIMED */)
{
    int attrType = PTHREAD_MUTEX_TIMED_NP;
    switch(type)
    {
    case Mutex::ADAPTIVE:
        attrType = PTHREAD_MUTEX_ADAPTIVE_NP;
        break;
    case Mutex::RECURSIVE:
        attrType = PTHREAD_MUTEX_RECURSIVE_NP;
        break;
    case Mutex::ERRORCHECK:
        attrType = PTHREAD_MUTEX_ERRORCHECK_NP;
        break;
    case Mutex::TIMED:
    default:
        break;
    }
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, attrType);
    pthread_mutex_init(&lock, &attr);
    pthread_mutexattr_destroy(&attr);
}

/* static */
void Mutex::Helper::destroy( pthread_mutex_t& lock ) {
    pthread_mutex_destroy(&lock);
}
