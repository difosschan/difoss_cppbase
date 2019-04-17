#pragma once
#include "types.h"
#include <pthread.h>
#include "enum_util.h"

typedef int (*thread_callback_t)(void*);

class Thread {
public:
    Thread(bool isPaused = false);
    virtual ~Thread();

    DEFINE_ENUM(State,,
        (UNINITED)      // δ��ʼ����
        (INITED)        // ��ʼ��
        (RUNNING)       // ��������
        (PAUSED)        // ����ͣ
        (STOPPED)       // ��ֹͣ
        (DEAD)          // ������
    );

    virtual void start();
    virtual void pause();
    virtual void stop();
    virtual void join();

    int getState();

    // �ṩ2�������̴߳������ķ�ʽ��
public:
    // 1��C style:���ûص�����
    // ����callback function�ķ���ֵ��0�������᲻�ϵ�ѭ��
    thread_callback_t setCallback(thread_callback_t func, void* param);
protected:
    // 2��C++ style inheritance:�̳�Thread��override�˷���
    virtual bool run();

private:
    static void* realThreadProc( void* myself );

protected:
    pthread_mutex_t         _lock;
    pthread_cond_t          _cond;
    int                     _state;
    pthread_t               _thread_id;
    thread_callback_t       _callback;
    void*                   _param;
};
