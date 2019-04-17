#pragma once

#include "macro.h"
#include <pthread.h>

class Mutex
{
public:

    //////////////////////////////////////////////////////////////////////////
    // Mutex::Attribute ��������
    typedef enum {
        TIMED = 0,      // ��ͨ����
        RECURSIVE = 1,  // Ƕ����������ͬһ���̶߳�ͬһ�����ɹ���ö�Σ���ͨ�����unlock������
        ERRORCHECK = 2, // �������ͬһ�߳�Ҳֻ������һ������
        ADAPTIVE = 3    // ��Ӧ�������ȴ����������¾�����
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
    // Mutex::Helper ���ã����� Mutex �� pthread_mutex_t ��ʹ��
    class Helper
    {
    public:
        // �����ں����˳�ʱ������
        Helper(Mutex& lock);
        Helper(pthread_mutex_t& lock);
        virtual ~Helper();

    public:
        // ����pthread_mutex_t�ĳ�ʼ����
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
