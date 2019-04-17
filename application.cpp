#include "application.h"
extern "C" {
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>
};

#ifdef _MSC_VER
#include "winbase.h"
#endif // _MSC_VER

//////////////////////////////////////////////////////////////////////////
#include <utility>
#include "color_print.h"
#ifdef _DEBUG
#include "macro.h"
#endif // _DEBUG

using namespace std;

bool Application::_IsSigOutput = true;
Application::SignalMap_t Application::_SignalsMap;
 
Application::Application()
: _argc(0), _argv(NULL) {}

Application::~Application() {}

bool Application::init(int argc, char* argv[]) {
    _argc = argc;
    _argv = argv;
    return true;
}

int Application::getpid() { return ::getpid(); }
int Application::getppid(void) { return 0; }

// for debug
#include "macro.h"

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#ifndef STDIN_FILENO
#   define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
#   define STDOUT_FILENO 1
#endif
#ifndef STDOUT_FILENO
#   define STDERR_FILENO 2
#endif
#else
#include <unistd.h>
#endif

void Application::daemon() {
    switch (fork()) {
    case -1:
        perror("fork()");
        return;

    case 0:
        cout << "fork() return 0, child process(id:"<< getpid() << ") begin." << endl;
        break;

    default:
        cout << "father process(id:" << getpid() <<  ") _exit." << endl;
        _exit(EXIT_SUCCESS);
    }

    if (setsid() == -1) {
        perror("setsid()");
        return;
    }

    int fd = 0;
    if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
        if(dup2(fd, STDIN_FILENO) < 0) {
            perror("dup2 stdin");
            return;
        }
        if(dup2(fd, STDOUT_FILENO) < 0) {
            perror("dup2 stdout");
            return;
        }
        if(dup2(fd, STDERR_FILENO) < 0) {
            perror("dup2 stderr");
            return;
        }
    
        if (fd > STDERR_FILENO) {
            if(close(fd) < 0) {
                perror("close");
                return;
            }
        }
    }
    return;
}

bool Application::reg_sig(int sig, SigHandler_t func, bool repeat /* = false */) {
    
#ifdef _DEBUG
    cout << "in Application::reg_sig() parameters as follow:" << endl;
    COUT(sig);
    SEE_ADDRESS_64_PTR(func);
    COUT(repeat);
#endif // _DEBUG
    
    // �Ȳ��Ҹ��ź��Ƿ���ע��
    SignalMap_t::iterator iter = _SignalsMap.find(sig);
    if (_SignalsMap.find(sig) == _SignalsMap.end()) {
#ifdef _DEBUG
        cout << "first time to regisetr signal(" << sig << ")." << endl;
#endif // _DEBUG
        // δע�ᣬʹ��Application�ڲ����źű�Ǻ�������g_signal_handle����ռע���źš�
        register_signal_flag(sig, repeat);
    }
    
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = func;
    // �����Ҫע�ᴦ�����źŵĺ�����ֻ�ᱻ���뵽 actions �У�
    // ��Ȼ���ظ�ע��Ĵ������������ᱻ�������������ε��ã�ֱ�ӷ��� true ����
    size_t actions_size = iter->second.actions.size();
    size_t i = 0; 
    for (;i < actions_size; ++i) {
        if ((iter->second.actions)[i].sa_handler == func)
            break;
    }
    if (i == actions_size) {
        // @param func ���� actions �У����ӽ���actions������� sigaction ע�ᣬ��ֹ����ˢ��
        iter->second.actions.push_back(act);
    }
    
    return true;
}

bool Application::reg_sig(int sig, const struct sigaction& act, struct sigaction* old_act /* = NULL */) {
    if (sigaction(sig, &act, NULL) < 0) {
        perror("install signal error");
        return false;
    }
    return true;
}

long Application::gettid() {
    return static_cast<long> (syscall(SYS_gettid));
}

unsigned int Application::sleep( unsigned int seconds )
{
#ifdef _MSC_VER
    Sleep(seconds);
    return 0;
#else
    return ::sleep(seconds);
#endif // _MSC_VER
}

// ����<string>�ṩ��getline�ڳ�������źź���쳣��Ӧ��
// ���mysql 5.6.15 Դ���Լ�дʵ��һ������scanf("%c")��getline��
std::string Application::my_getline()
{
    std::string result;
    char buf[8];
    while (1==scanf("%c", buf)) {
        if (buf[0] == '\n') {
            break;
        }
        result.push_back( buf[0] );
    }
    return result;
}

void Application::g_signal_handle(int sig_no) {
    if (_IsSigOutput)
        printf(START_BLUE "process(id=%ld): g_signal_handle(%d) is touched." END_COLOR "\n",
            Application::gettid(), sig_no);
    Application::SignalMap_t::iterator iter = _SignalsMap.find(sig_no);
    if (iter != _SignalsMap.end()) {
        // �ص����� Application::register_signal_flag() �ӿڰ�װ���źŴ�������
        size_t size = iter->second.actions.size();
        for (size_t i = 0; i < size; ++i) {
            struct sigaction& old_act = iter->second.actions[i];
            if (SIG_DFL != old_act.sa_handler
               && SIG_ERR != old_act.sa_handler
               && SIG_IGN != old_act.sa_handler) {
                if (g_signal_handle != old_act.sa_handler)
                    (*old_act.sa_handler)(sig_no);
            }
        }
        iter->second.flag = true;
        if (iter->second.repeat)
            Application::reg_sig(sig_no, g_signal_handle, true);
    }
}


void Application::register_signal_flag(int sig_no, bool repeat /* = false */) {
    if (_SignalsMap.find(sig_no) != _SignalsMap.end())
        return;
    
    struct sigaction act, old_act;
    memset(&act, 0, sizeof(struct sigaction));
    memset(&old_act, 0, sizeof(struct sigaction));
    act.sa_handler = g_signal_handle;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    
    if (sigaction(sig_no, &act, &old_act) < 0) {
        perror("install signal (sa_handler) error");
        return;
    }
    
    struct SignalInfo_t si;
    si.flag = false;
    si.repeat = repeat;
    si.actions.push_back(old_act);
    si.actions.push_back(act);
    _SignalsMap.insert(make_pair(sig_no, si));
    
#ifdef _DEBUG
    SignalMap_t::iterator iter = _SignalsMap.find(sig_no);
    if (iter != _SignalsMap.end()) {
        cout << START_BLUE;
        COUT(sig_no);
        COUT(iter->second.actions.size());
        for (size_t i = 0; i < iter->second.actions.size(); ++i) {
            SEE_ADDRESS_64_PTR(iter->second.actions[i].sa_handler);
        }
        cout << END_COLOR;
    }
#endif // _DEBUG
}

bool Application::check_signal_flag(int sig_no) {
    SignalMap_t::iterator iter = _SignalsMap.find(sig_no);
    if (iter == _SignalsMap.end()) {
        return false;
    }
    return iter->second.flag;
}

void Application::reset_signal_flag(int sig_no, bool value /* = false */) {
    SignalMap_t::iterator iter = _SignalsMap.find(sig_no);
    if (iter != _SignalsMap.end()) {
        iter->second.flag = value;
    }
}

void Application::set_is_output( bool b ) {
    _IsSigOutput = b;
}
