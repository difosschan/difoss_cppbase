#pragma once

#include "types.h"

extern "C" {
    #include <pthread.h>
};
#include <ostream>
#include <vector>
#include <signal.h>
#include "application.h"

template <class _Ty> class MQ {
    
public:
    typedef _Ty Message_T;

public:

    MQ(unsigned int maxSize = 10)
        : _max_size(maxSize)
        , _buffer(NULL)
        , _head(0)
        , _tail(0)
        , _can_be_used(true)
    {
        // 缓冲区为数组（带头结点的循环数组）。
        _buffer = new Message_T[maxSize + 1];

        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP); // 嵌套锁，允许同一个线程对同一个锁成功获得多次，并通过多次unlock解锁。
        pthread_mutex_init(&_lock, &attr);
        pthread_mutexattr_destroy(&attr);

        pthread_cond_init(&_not_empty, NULL);
        pthread_cond_init(&_not_full, NULL);
        
        _AllPointers.push_back(this);
        Application::reg_sig(SIGINT, sig_term, true);
        Application::reg_sig(SIGTERM, sig_term, true);
    }

    virtual ~MQ() {
        release();
    }
    
    static void sig_term(int signo) {
        size_t size = _AllPointers.size();
        for (size_t i = 0; i < size; ++i) {
            if (_AllPointers[i])
                ((MQ<Message_T>*)_AllPointers[i])->clearExcept(Message_T());
        }
    }
    
    //! \fn 在队列结尾预留一个消息，调用本方法与直接调用send()比起来，为的是减少无谓的内存拷贝。
    //! @note 1) 注意，调用此函数后，会对队列加锁，当完成该消息对象指针所指内容的修改后，
    //         请调用 reserveConfirm() 方法进行解锁，调用失败（返回NULL）则无需再调用 reserveConfirm()；
    //         2) 由于@class类的回收机制，请对返回的指针所指的消息对象的内容清空；
    //         3) 切勿显式地 delete 返回的Message_T类型指针所指的内存。
    // @retval 如果失败，则返回NULL；成功返回这个消息对象的指针。
    Message_T* reserve() {
        pthread_mutex_lock(&_lock);
        if (! _can_be_used) {
            pthread_mutex_unlock(&_lock);
            return NULL;
        }
        
        // 缓冲区满时等待
        while (_IsFull()) {
            pthread_cond_wait(&_not_full, &_lock);
            if (! _can_be_used) {
                pthread_mutex_unlock(&_lock);
                return NULL;
            }
        }
        
        // 如果是缓冲区本来空，需要唤醒被 _not_empty 阻塞的线程
        if (_IsEmpty())
            pthread_cond_signal(&_not_empty);
        // 尾下标向后移
        _tail = _Next(_tail);
        return _buffer + _tail;
    }
    
    //! \fn 这是reserve()的非阻塞版本，当队列满的时候，不会等待消费者消费队列元素，立即返回NULL。
    //! \note 若无法获得锁，本函数也会阻塞。
    Message_T* tryReserve() {
        Message_T* pmsg = NULL;
        pthread_mutex_lock(&_lock);
        if (! _IsFull()) {
            pmsg = reserve();
        }
        pthread_mutex_unlock(&_lock);
        return pmsg;
    }
    
    void reserveConfirm() {
        pthread_mutex_unlock(&_lock);
    }

    //! \fn 在队列的结尾插入一个消息
    bool send(const Message_T& msg) {
        Message_T* pmsg = reserve();
        if (NULL == pmsg) {
            return false;
        }
        // 写数据
        *pmsg = msg;
        reserveConfirm();
        return true;
    }
    
    //! \fn 这是send()的非阻塞版本，当队列满的时候，不会等待消费者消费队列元素，立即返回NULL。
    //! \note 若无法获得锁，本函数也会阻塞。
    bool trySend(const Message_T& msg) {
        Message_T* pmsg = tryReserve();
        if (NULL == pmsg) {
            return false;
        }
        // 写数据
        *pmsg = msg;
        reserveConfirm();
        return true;
    }

    // @note 在队列开头预留一个消息，调用本方法与直接调用sendPriority()比起来，为的是减少无谓的内存拷贝。
    // @notice （注意事项与 reserve() 函数相同）
    Message_T* reservePriority() {
        pthread_mutex_lock(&_lock);
        if (! _can_be_used) {
            pthread_mutex_unlock(&_lock);
            return NULL;
        }
        
        // 缓冲区满时等待
        while (_IsFull()) {
            pthread_cond_wait(&_not_full, &_lock);
            if (! _can_be_used) {
                pthread_mutex_unlock(&_lock);
                return NULL;
            }
        }
        
        unsigned int old_head = _head;
        // 如果是缓冲区本来空，需要唤醒被 _not_empty 阻塞的线程
        if (_IsEmpty())
            pthread_cond_signal(&_not_empty);
        // 头下标向前移
        _head = _Prev(_head);
        return _buffer + old_head;
    }
    
    // @note 在队列的开头插入一个消息
    void sendPriority(const Message_T& msg) {
        Message_T* pmsg = reservePriority();
        if (NULL == pmsg) {
            return;
        }
        // 写数据
        *pmsg = msg;
        reserveConfirm();
    }
    
    Message_T receive() {
        Message_T msg;
        pthread_mutex_lock(&_lock);
        if (! _can_be_used) {
            pthread_mutex_unlock(&_lock);
            return msg;
        }
        // 缓冲区空时等待
        while (_IsEmpty()) {
            pthread_cond_wait(&_not_empty, &_lock);
            if (! _can_be_used) {
                pthread_mutex_unlock(&_lock);
                return msg;
            }
        }
        // 如果在插入这一个数据前是满的，发送当前缓冲区未满信号
        if (_IsFull())
            pthread_cond_signal(&_not_full);
        // 头下标向后移
        _head = _Next(_head);
        // 读数据
        msg = _buffer[_head];
        pthread_mutex_unlock(&_lock);

        return msg;
    }

    bool tryReceive(Message_T& out) {
        bool r = false;
        pthread_mutex_lock(&_lock);
        if (_Count()) {
            out = receive();
            r = true;
        }
        pthread_mutex_unlock(&_lock);
        return r;
    }

    void clear() {
        pthread_mutex_lock(&_lock);
        // 如果队列原来是满的，发送信号让等待“队列未满”的生产者线程被唤醒。
        if (_IsFull()) {
            pthread_cond_signal(&_not_full);
        }
        _head = _tail;
        pthread_mutex_unlock(&_lock);
    }

    // 除了插入参数传入的消息，其他消息都clear掉。
    void clearExcept(const Message_T& one) {
        pthread_mutex_lock(&_lock);
        // 如果队列原来是满的，发送信号让等待“队列未满”的生产者线程被唤醒。
        if (_IsFull()) {
            pthread_cond_signal(&_not_full);
        } else if (_IsEmpty()) {
            pthread_cond_signal(&_not_empty);
        }
        _buffer[_tail] = one;
        _head = _Prev(_tail);
        pthread_mutex_unlock(&_lock);
    }

    // 是否可用标记 _can_be_used ，用于安全退出生产者和消费者。
    bool canBeUsed() {
        bool r = false;
        pthread_mutex_lock(&_lock);
        r = _can_be_used;
        pthread_mutex_unlock(&_lock);
        return r;
    }
    void canBeUsed(bool b) {
        pthread_mutex_lock(&_lock);
        _can_be_used = b;
        pthread_mutex_unlock(&_lock);
    }
    // 唤醒生产者
    void weakUpSender() {
        pthread_mutex_lock(&_lock);
        pthread_cond_broadcast(&_not_full);
        pthread_mutex_unlock(&_lock);
    }
    // 唤醒消费者
    void weakUpReveiver() {
        pthread_mutex_lock(&_lock);
        pthread_cond_broadcast(&_not_empty);
        pthread_mutex_unlock(&_lock);
    }

    size_t size() const { return _max_size; }

    size_t count() const {
        size_t s = 0;
        pthread_mutex_lock(&_lock);
        s = _Count();
        pthread_mutex_unlock(&_lock);
        return s;
    }

    void show(std::ostream& os) const {
        pthread_mutex_lock(&_lock);
        os  << "|- size:" << _max_size << ", count:" << count() << std::endl
            << "|- [inner] head:" << _head << ", tail:" << _tail << std::endl;
        pthread_mutex_unlock(&_lock);
    }

    void showDetail(std::ostream& os, const char* delimiters = "\n") const {
        pthread_mutex_lock(&_lock);
        size_t c = _Count();
        size_t cur = _Next(_head);
        os << c << " message(s) in MQ:" << std::endl;
        for (size_t i = 0; i < c; ++i) {
            if (i != 0)
                os << delimiters;
            os << _buffer[cur + i];
        }
        os << std::endl;
        pthread_mutex_unlock(&_lock);
    }
    
protected:
    inline size_t _Next(size_t pos) const {
        return (pos == _max_size) ? 0 : (pos + 1);
    }

    inline size_t _Prev(size_t pos) const {
        return (0 == pos) ? (_max_size) : (pos - 1);
    }

    inline size_t _Count() const {
        if (_head <= _tail) {
            return size_t(_tail - _head);
        } else {
            return _tail + _max_size - _head + 1;
        }
    }
    
    inline bool _IsFull() const {
        return _Prev(_head) == _tail;
    }
    
    inline bool _IsEmpty() const {
        return _head == _tail;
    }

    // 释放动态内存，调用后请勿对MQ做其他操作。
    void release() {
        _can_be_used = false;
        if (_buffer) {
            pthread_cond_destroy(&_not_full);
            pthread_cond_destroy(&_not_empty);
            pthread_mutex_destroy(&_lock);

            delete[] _buffer;
            _buffer = NULL;
        }
    }

protected:
    unsigned int _max_size;
    Message_T*   _buffer;
        
    mutable pthread_mutex_t _lock;
    pthread_cond_t  _not_empty; // 缓冲区非空
    pthread_cond_t  _not_full;  // 缓冲区未满
        
    unsigned int _head; // 头结点下标
    unsigned int _tail; // 尾结点下标

    bool _can_be_used; // 标记是否可用
    
    static std::vector<void*> _AllPointers;
};

template <class _Ty> std::vector<void*> MQ<_Ty>::_AllPointers;
