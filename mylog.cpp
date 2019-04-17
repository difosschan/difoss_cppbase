#include "mylog.h"

#include <string>
#include "color_print.h"
#include "mutex.h"

#define IO_BUFFER_SIZE 64*1024

MyLog::MyLog()
    : _line(0)
    , _file(0)
    , _level(log_trace), _level_file(-1), _level_term(-1)
    , _os_log(NULL), _log_size(0), _log_count(0), _buf_size(0), _file_size(MY_LOG_SIZE)
{
    memset(_name, 0, PATH_MAX);
    memset(&_date, 0, sizeof(_date));
    memset(_prefix, 0, sizeof(_prefix));
    memset(_subdir, 0, sizeof(_subdir));        
    memset(_fulldir, 0, sizeof(_fulldir));
    MutexHelper::init(_term_and_member_lock, Mutex::RECURSIVE);
}

MyLog::~MyLog()
{
    logClose();
    MutexHelper::destroy(_term_and_member_lock);
}

const char *LOG_HEAD[] = 
{
    "T.", // trace
    "D.", // debug
    "I.", // info
    "W.", // warn
    "E.", // error
    "F." // fatal
};

void MyLog::setLogNameHead(const char *nm)
{
    EASY_LOCK(_term_and_member_lock);
    // ��־���ļ�����������head_ʱ��_����ID_��־����.log
    int len = strlen(nm);
    if(len != 0 && memcmp(_prefix, nm, len)!=0)
    {
        strcpy(_prefix, nm);
        _log_count = 0;
        logClose(true);
    }
}

int MyLog::setLogSubDir(const char *dir)
{
    EASY_LOCK(_term_and_member_lock);
    int len = strlen(dir);
    if(len != 0 && memcmp(_subdir, dir, len)!=0)
    {
		if (!checkPath(dir))
			return -1;
			
        strcpy(_subdir, dir); 
	    _log_count = 0;
        logClose(true);
        return 0;
	}
		
    return -1;
}

int MyLog::setLogDir(const char *dir)
{
    EASY_LOCK(_term_and_member_lock);
    int len = strlen(dir);
    if(len != 0 && memcmp(_fulldir, dir, len)!=0)
    {
    	if (!checkPath(dir))
    		return -1;
    		
        strcpy(_fulldir, dir);        
	    _log_count = 0;
        logClose(true);
    }
    return 0;
}


FILE *MyLog::logOpen()
{
    EASY_LOCK(_term_and_member_lock);

    logClose(true);

    time_t t;
    struct tm tt;
    time(&t);

    memcpy(&tt, localtime(&t), sizeof(struct tm));

    if (_date.tm_mday!=tt.tm_mday||_date.tm_mon!=tt.tm_mon||_date.tm_year!=tt.tm_year)
    {
        // ���ڸı䣬���¼���
        _log_count = 0;
        memcpy(&_date, &tt, sizeof(_date));
    }
    
    char dirpath[PATH_MAX];
    memset(dirpath, 0 ,PATH_MAX);
    if( _fulldir[0] == 0)
        sprintf(dirpath, "%s", _subdir);
    else
        sprintf(dirpath, "%s",  _fulldir);

    snprintf(_name, PATH_MAX, "%s%s%s_%d_%04d%02d%02d_%d.log",
       dirpath, "/", _prefix, getpid(), _date.tm_year+1900, _date.tm_mon+1, _date.tm_mday, _log_count);
    
    
    char name2[PATH_MAX] = { 0 };
    snprintf(name2, PATH_MAX, "%s.tmp", _name);
    
    logOpen(name2);
    _log_count++;
    
    return _os_log;
}

FILE *MyLog::logOpen(const char *name)
{  
    EASY_LOCK(_term_and_member_lock);
  // to do
  // ���ԭ�ļ������ԭ�ļ�����
    _os_log = fopen(name, "w");
    
    if (_os_log == NULL)
    	fprintf(stderr, "ERROR init [%s] log _file\n", name);
    else if ( _buf_size > 0 )
        setvbuf(_os_log, NULL, _IOFBF, _buf_size); // NULL mean: _file system automatically allocated buffer, size specifies the size of the buffer to be used

    return _os_log;
}

void MyLog::logClose(bool bRename /* = true */)
{
    EASY_LOCK(_term_and_member_lock);

    if (_os_log != NULL )
    {
        fclose(_os_log);
        _os_log = NULL;
               
        if (bRename)
        { 
            char name2[PATH_MAX] = { 0 };
            snprintf(name2, PATH_MAX, "%s.tmp", _name);
                        
            rename(name2, _name);
        }
    }
}

void MyLog::log1(const char *format, ...) //д��־
{
    EASY_LOCK(_term_and_member_lock);
    
    struct timeval now;
    struct tm ts;

    gettimeofday(&now, 0);
    memcpy(&ts, localtime(&now.tv_sec), sizeof(struct tm));

	// ÿ��0���л���־
    if ( _os_log == NULL || _log_size>=_file_size
       || (_date.tm_mday!=ts.tm_mday||_date.tm_mon!=ts.tm_mon||_date.tm_year!=ts.tm_year))
    {
        if ( logOpen() == NULL )
        	return ;
    
        _log_size = 0;
    }

    char dateinfo[32] = {0};
    strftime(dateinfo, sizeof(dateinfo), "%F %T", &ts);    
    // sprintf(dateinfo, "%04d-%02d-%02d %02d:%02d:%02d",
    //    ts.tm_year + 1900, ts.tm_mon + 1, ts.tm_mday,
    //    ts.tm_hour, ts.tm_min, ts.tm_sec);

    // �˴�����־��ʽ������Ҫ������
    if (_level>=log_trace&&_level<=log_fatal)
    	fprintf(_os_log, LOG_HEAD[_level/20]);
    else
    	fprintf(_os_log, "%06d", _level);
    	
    fprintf(_os_log, "[%s:%06ld]", dateinfo, now.tv_usec);
                        
    // Start of variable args section.
    va_list argp;
    va_start (argp, format);
  
    vfprintf(_os_log, format, argp); 
      
    va_end (argp);
  
    _log_size++;
}

void MyLog::log2(const char *format, ...) //д�ն�
{
    EASY_LOCK(_term_and_member_lock);
    
    struct timeval now;
    gettimeofday(&now, 0);

    // ���ݼ���ѡ����ɫ
    std::string start_color;
    if (_term_colorful) {
        switch (_level/20) {
        case log_trace:
            start_color = START_GRAY;
            break;
        case log_debug/20:
            break;
        case log_info/20:
            start_color = START_GREEN;
            break;
        case log_warn/20:
            start_color = START_YELLOW;
            break;
        case log_error/20:
            start_color = START_RED;
            break;
        case log_fatal/20:
            start_color = START_CYAN START_UNDERLINE;
            break;
        default:
            break;
        }
    }

    // �˴�����־��ʽ������Ҫ������
    if (_level>=log_trace&&_level<=log_fatal)
        fprintf(stdout, LOG_HEAD[_level/20]);
    else
        fprintf(stdout, "%06d", _level);
        
    struct tm ts;
    memcpy(&ts, localtime(&now.tv_sec), sizeof(struct tm));
    
    char dateinfo[32] = {0};
    strftime(dateinfo, sizeof(dateinfo), "%F %T", &ts);
    // sprintf(dateinfo, "%04d-%02d-%02d %02d:%02d:%02d",
    //     ts.tm_year + 1900, ts.tm_mon + 1, ts.tm_mday,
    //     ts.tm_hour, ts.tm_min, ts.tm_sec);
    
    fprintf(stdout, "[%s:%06ld]", dateinfo, now.tv_usec);

    std::string colorful = _term_colorful ? (start_color + format + END_COLOR) : format;
    // Start of variable args section.
    va_list argp;
    va_start (argp, format);

    vfprintf(stdout, colorful.c_str(), argp); 
    va_end(argp);
}

bool MyLog::checkPath(const char *path)
{
    DIR *dir = NULL;
    if((dir= opendir(path)) ==NULL) 
    {
        //��Ŀ¼�����ڣ�������
        //if( mkdir(dirpath, 0755) == -1)
        //{
        fprintf(stderr,"Directory: %s is not exist\n", path);
        return false;
        //}
    }
    if(dir != NULL)
    {
        if (closedir(dir) < 0)
        {
            fprintf(stderr,"Unable to close directory %s\n", path);
            return false;
        }
        dir = NULL;
    }
    if(access(path, W_OK) < 0)
    {
        fprintf(stderr,"The dir access permissions do not allow!path=%s\n", path);
        return false;
    }
    return true;
}

void MyLog::setConditional( int le, int li, const char * f )
{
    EASY_LOCK(_term_and_member_lock);
    assert(le>=log_trace&&le<=log_end);
    _level = le;
    _line = li;
    _file = f;
}

void MyLog::setLevelClass( int le_fl, int le_tm )
{
    EASY_LOCK(_term_and_member_lock);
    _level_file = le_fl;
    _level_term = le_tm;
}

bool MyLog::checkLevelFile( int le )
{
    EASY_LOCK(_term_and_member_lock);
    return le>=_level_file;
}

bool MyLog::checkLevelTerm( int le )
{
    EASY_LOCK(_term_and_member_lock);
    return le>=_level_term;
}

void MyLog::setTermColorful( bool whether )
{
    EASY_LOCK(_term_and_member_lock);
    _term_colorful = whether;
}

void MyLog::setFileSize( unsigned int size )
{
    EASY_LOCK(_term_and_member_lock);
    assert(size>0);
    if ( size>0 )
        _file_size = size;
}

void MyLog::setBufSize( unsigned int size )
{
    EASY_LOCK(_term_and_member_lock);
    assert(size>0);
    if ( size > 0)
        _buf_size = size;
}

void MyLog::flush()
{
    EASY_LOCK(_term_and_member_lock);
    if (_os_log != NULL ) { fflush(_os_log); }
}



// example:

//    MY_TRACE("main");

// ע�⣺�������־����ʹ�õ�ʱ��һ��Ҫ���һ��С���Ž���־��Ϣ������

//    MY_INFO(("INFO\n"));
//    MY_DEBUG(("DEBUG\n"));
//    MY_WARN(("WARN\n"));
//    MY_ERROR(("ERROR\n"));
    
//    struct tm *lt;
//    struct timezone tz;
//    struct timeval now;
//    gettimeofday(&now, &tz);
//    lt = localtime(&now.tv_sec);
//    MY_ERROR(("%02d:%02d:%02d:%06d\n",
//    lt->tm_hour, lt->tm_min, lt->tm_sec, now.tv_usec));
