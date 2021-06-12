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
        (UNINITED)      // 未初始化的
        (INITED)        // 初始化
        (RUNNING)       // 正在运行
        (PAUSED)        // 已暂停
        (STOPPED)       // 已停止
        (DEAD)          // 已死亡
    );

    virtual void start();
    virtual void pause();
    virtual void stop();
    virtual void join();

    int getState();

    // 提供2种设置线程处理函数的方式：
public:
    // 1、C style:设置回调函数
    // 其中callback function的返回值是0，程序会会不断的循环
    thread_callback_t setCallback(thread_callback_t func, void* param);
protected:
    // 2、C++ style inheritance:继承Thread并override此方法
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
