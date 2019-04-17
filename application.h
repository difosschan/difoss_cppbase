#pragma once
#include <iostream> // ���е� static ios_base::Init __ioinit ���뱻��������Ȼ
                    // �����˳�ʱ���core��libg++��ȱ�ݣ���
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

    // @note ��ȡ�߳�ID
    static long gettid();

    // @note ��ȡ����ID
    static int getpid();

    // @note ��ȡ�����̽���ID
    static int getppid();

    static unsigned int sleep(unsigned int seconds);

    typedef void (*SigAction_t)(int, struct siginfo *, void *);
    typedef void (*SigHandler_t)(int);
    
    // @note �˺�����������ע���źŴ���ֻ�����źŴ���ʱ��@param func�ᱻ�ص�
    static bool reg_sig(int sig, SigHandler_t func, bool repeat = false);
    
    // @note �������� sigaction() �����źŴ��������滻
    static bool reg_sig(int sig, const struct sigaction& act, struct sigaction* old_act = NULL);

    static std::string my_getline();

    //////////////////////////////////////////////////////////////////////////
    // �ṩ�򵥵��źŴ�����ǩ��¼
    
    // ע�ᣬrepeat�������źŴ������Ƿ��ڴ������ٴ�ע���Լ�
    static void register_signal_flag(int sig_no, bool repeat = false);
    // �ź��Ƿ��յ�
    static bool check_signal_flag(int sig_no);
    // �����źŶ�Ӧ�ı�־
    static void reset_signal_flag(int sig_no, bool value = false);
    // ���ý��յ��źź��Ƿ��ӡ
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
