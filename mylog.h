#pragma once
// create by difoss@163.com at 2014/1/26

#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <pthread.h>
#include "design_patterns/singleton.h"
//-----------------------------------------------------------------------------
// 注意：此日志类线程安全
//-----------------------------------------------------------------------------

#ifndef g_log
#define g_log MyLog::instance()
#endif

#define MY_LOG(le, X) \
    do{\
        if ( g_log.checkLevelFile(le) ) {\
            g_log.setConditional(le, __LINE__, __FILE__);\
            g_log.log1 X;\
        }\
        if ( g_log.checkLevelTerm(le) ) {\
            g_log.setConditional(le, __LINE__, __FILE__);\
            g_log.log2 X;\
        }\
    } while(0)

//-----------------------------------------------------------------------------
#ifdef _NO_MY_LOG_

#define MY_AUTO_TRACE(X)  do {} while(0)
#define MY_TRACE(X)       do {} while(0)
#define MY_DEBUG(X)       do {} while(0)
#define MY_INFO(X)        do {} while(0)
#define MY_WARN(X)        do {} while(0)
#define MY_ERROR(X)       do {} while(0)
#define MY_FATAL(X)       do {} while(0)

#else

#define MY_AUTO_TRACE(X)    MyTrace ___ (X, __LINE__, __FILE__)
#define MY_TRACE(X)         MY_LOG(MyLog::log_trace, X)
#define MY_DEBUG(X)         MY_LOG(MyLog::log_debug, X)
#define MY_INFO(X)          MY_LOG(MyLog::log_info, X)
#define MY_WARN(X)          MY_LOG(MyLog::log_warn, X)
#define MY_ERROR(X)         MY_LOG(MyLog::log_error, X)
#define MY_FATAL(X)         MY_LOG(MyLog::log_fatal, X)

#endif // _NO_MY_LOG_

//-----------------------------------------------------------------------------
// 日志单个文件最大记录数
#define MY_LOG_SIZE        1000000

//-----------------------------------------------------------------------------
#define SET_LOG_LEVEL(fl, tm)           g_log.setLevelClass(fl, tm)
#define SET_LOG_TERM_COLORFUL(whether)  g_log.setTermColorful(whether)
#define FLUSH_LOG_FILE()                g_log.flush()
#define SET_LOG_NAME_HEAD(x)            g_log.setLogNameHead(x)
#define SET_LOG_SUB_DIR(x)              g_log.setLogSubDir(x)
#define SET_LOG_DIR(x)                  g_log.setLogDir(x)
#define REOPEN_LOG()                    g_log.logOpen()

//-----------------------------------------------------------------------------
#define LOG_CHECK_TRACE() (g_log.checkLevelFile(MyLog::log_trace)||g_log.checkLevelTerm(MyLog::log_trace))
#define LOG_CHECK_DEBUG() (g_log.checkLevelFile(MyLog::log_debug)||g_log.checkLevelTerm(MyLog::log_debug))
#define LOG_CHECK_INFO() (g_log.checkLevelFile(MyLog::log_info)||g_log.checkLevelTerm(MyLog::log_info))
#define LOG_CHECK_WARN() (g_log.checkLevelFile(MyLog::log_warn)||g_log.checkLevelTerm(MyLog::log_warn))
#define LOG_CHECK_ERROR() (g_log.checkLevelFile(MyLog::log_error)||g_log.checkLevelTerm(MyLog::log_error))
#define LOG_CHECK_FATAL() (g_log.checkLevelFile(MyLog::log_fatal)||g_log.checkLevelTerm(MyLog::log_fatal))
//-----------------------------------------------------------------------------

#define LOG_SET_FILE_SIZE(size) g_log.setFileSize(size)
#define LOG_SET_BUF_SIZE(size) g_log.setBufSize(size)

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class MyLog
{
    SINGLETON_DECLARE(MyLog)

public:
    typedef enum _Level_E
    {
        log_trace    = 0,
        log_debug    = 20,
        log_info     = 40,
        log_warn     = 60,
        log_error    = 80,
        log_fatal    = 100,
        log_end      = 120
    } Level;
        
public:
    int setLogDir(const char *dir);
    int setLogSubDir(const char *dir);
    void setLogNameHead(const char *nm);
    FILE *logOpen();
    FILE *logOpen(const char *name);
    void logClose(bool bRename = true);

    //void log(const char *format, ...); // 写日志和终端
    void log1(const char *format, ...); //写日志
    void log2(const char *format, ...); //写终端
    
    void setConditional(int le, int li, const char * f);    
    void setLevelClass(int le_fl, int le_tm);    
    bool checkLevelFile(int le);    
    bool checkLevelTerm(int le);
    void setTermColorful(bool whether);
    void setFileSize(unsigned int size);
    void setBufSize(unsigned int size);

    void flush();
    bool checkPath(const char *path);


private:
    int _line;
    const char* _file;
    int _level;
    
    int _level_file;
    int _level_term;
    bool _term_colorful;
    
    FILE *_os_log;
    
    unsigned int _log_size; // log size
    unsigned int _log_count; // log count
    char _name[PATH_MAX];
    char _prefix[128];    
    char _subdir[128];
    char _fulldir[PATH_MAX];
    struct tm _date;

    unsigned int _buf_size; // setvbuf size
    unsigned int _file_size; // max line count before seplit log file, for _log_size

    pthread_mutex_t _term_and_member_lock;
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class MyTrace
{
public:
    MyTrace(const char *msg, int line, const char *file)
    {
        name = msg;
        MY_TRACE(("calling %s in file `%s' on line %d\n", msg, file, line));
    }
    
    ~MyTrace()
    {
        MY_TRACE(("leaving %s\n", name));
    }
    
private:
    const char *name;
};
