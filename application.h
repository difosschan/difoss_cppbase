#pragma once
#include <iostream> // 当中的 static ios_base::Init __ioinit 必须被声明，不然
                    // 程序退出时会出core（libg++的缺陷）。
#include <string>
#include <signal.h>
#include <vector>
#include <map>

class Application {
public:
    struct SignalInfo_t {
        bool flag;
        bool repeat;
        std::vector<struct sigaction> actions; 
    };
    typedef std::map<int, SignalInfo_t> SignalMap_t;
    
public:
    Application();
    virtual ~Application();

    virtual bool init(int argc, char* argv[]);
    virtual void run() {}

    // @note 获取线程ID
    static long gettid();

    // @note 获取进程ID
    static int getpid();

    // @note 获取父进程进程ID
    static int getppid();

    static unsigned int sleep(unsigned int seconds);

    typedef void (*SigAction_t)(int, struct siginfo *, void *);
    typedef void (*SigHandler_t)(int);
    
    // @note 此函数并非真正注册信号处理，只是在信号触发时，@param func会被回调
    static bool reg_sig(int sig, SigHandler_t func, bool repeat = false);
    
    // @note 真正调用 sigaction() 进行信号处理函数的替换
    static bool reg_sig(int sig, const struct sigaction& act, struct sigaction* old_act = NULL);

    static std::string my_getline();

    //////////////////////////////////////////////////////////////////////////
    // 提供简单的信号触发标签记录
    
    // 注册，repeat参数让信号处理函数是否在触发后再次注册自己
    static void register_signal_flag(int sig_no, bool repeat = false);
    // 信号是否收到
    static bool check_signal_flag(int sig_no);
    // 重置信号对应的标志
    static void reset_signal_flag(int sig_no, bool value = false);
    // 设置接收到信号后是否打印
    static void set_is_output(bool b);

    static void daemon();  // change into reception mode.

private:
    static void g_signal_handle(int sig_no);
    
protected:

    int     _argc;
    char**  _argv;
    
    static SignalMap_t _SignalsMap;
    static bool _IsSigOutput;
};
