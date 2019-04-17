#include "str_util.h"
#include <algorithm>
extern "C" {
#include <errno.h>
};
#include "macro.h"

using namespace std;

void StrUtil::Split(const string& input, const char *delimiters, vector<string> &vec,
                    const string& strBeg, const string& strEnd,
                    bool bTrimSpace /* = true */, bool bIgnoreEmpty /* = true */)
{
    string strTmp = input + delimiters;
    string::size_type beg = 0, loc = 0;
    string dlm = string(delimiters);
    string::size_type cntDlm = dlm.size();
    while((loc = strTmp.find( dlm, beg )) != string::npos) {
        string strSub = strTmp.substr(beg, loc - beg);
        beg = loc + cntDlm;
        if (bTrimSpace)
            TrimSpace(strSub);
        if (bIgnoreEmpty)
            if(strSub.empty())
                continue;
        vec.push_back(strBeg + strSub + strEnd);
    }
}

void StrUtil::Split(const string &input, const char *delimiters, vector<string> &outputVector,
                    bool bSlitSpace /* = true */, bool bIgnoreEmpty /* = true */)
{
    StrUtil::Split(input, delimiters, outputVector, string(""), string(""), bSlitSpace, bIgnoreEmpty);
}

void StrUtil::ToUpper(char *str) {
    while (*str) {
        *str = toupper(*str);
        str++;
    }
}

string StrUtil::ToUpper(const char* str) {
    string result;
    size_t s = strlen(str);
    for (size_t i = 0; i < s; ++i) {
        result += toupper(str[i]);
    }
    return result;
}

void StrUtil::ToLower(char *str) {
    while (*str){
        *str = tolower(*str);
        str++;
    }
}

string StrUtil::ToLower(const char* str) {
    string result;
    size_t s = strlen(str);
    for (size_t i = 0; i < s; ++i) {
        result += tolower(str[i]);
    }
    return result;
}

string StrUtil::IntToString( int i ) {
    return StrUtil::ToString(i);
}

string StrUtil::UintToString( unsigned int ui ) {
    return StrUtil::ToString(ui);
}


string StrUtil::LongToString( long l ) {
    return StrUtil::ToString(l);
}

string StrUtil::FloatToString( float f )  {
    return StrUtil::ToString(f);
}

string StrUtil::DoubleToString( double d ) {
    return StrUtil::ToString(d);
}

string StrUtil::BooleanToString( bool b )
{
    return b ? "true" : "false";
}

template <typename Type_T>
inline bool seems_good(const char* containChars, const std::string& valueStr, Type_T* pValue) {
    if (valueStr.find_first_not_of(containChars) != std::string::npos)
        return false;

    std::stringstream stream(valueStr);
    Type_T value;

    if (!(stream >> value))
        return false;

    if (pValue)
        *pValue = value;

    return true;
}

bool StrUtil::IsFloat(const std::string &valueStr, float *pValue) {
    return seems_good<float>("+-0123456789.eEfF", valueStr, pValue);
}

bool StrUtil::IsInt(const std::string &valueStr, int *pValue)
{
    return seems_good<int>("+-0123456789", valueStr, pValue);
}

bool StrUtil::IsUint(const std::string &valueStr, unsigned int *pValue) {
    return seems_good<unsigned int>("+-0123456789", valueStr, pValue);
}

bool StrUtil::IsLong(const std::string &valueStr, long *pValue) {
    return seems_good<long>("+-0123456789", valueStr, pValue);
}

bool StrUtil::IsUlonglong(const std::string &valueStr, unsigned long long *pValue /*= 0*/ )
{
    return seems_good<unsigned long long>("+-0123456789", valueStr, pValue);
}

int StrUtil::ToInt(const std::string &str) {
    int returnVal = 0;
    if (StrUtil::IsInt(str, &returnVal))
        return returnVal;
    return 0;
}

float StrUtil::ToFloat(const std::string &str) {
    float returnVal = 0.0;
    if (StrUtil::IsFloat(str, &returnVal))
        return returnVal;
    return 0.0;
}

unsigned int StrUtil::ToUint(const std::string &str) {
    unsigned int returnVal = 0;
    if (StrUtil::IsUint(str, &returnVal))
        return returnVal;
    return 0;
}

long StrUtil::ToLong(const std::string &str) {
    long returnVal = 0;
    if (StrUtil::IsLong(str, &returnVal))
        return returnVal;
    return 0;
}

unsigned long long StrUtil::ToUlonglong( const std::string &str ) {
    unsigned long long returnVal = 0;
    if (StrUtil::IsUlonglong(str, &returnVal))
        return returnVal;
    return 0;
}

bool IsNotSpace(const char c)
{
    return !isspace(c);
}

string& StrUtil::RightTrim(string& str) {
    string::reverse_iterator ritr = find_if(str.rbegin(), str.rend(), IsNotSpace);  
    str.erase(str.length()-(ritr - str.rbegin()));
    return str;
}

string& StrUtil::LeftTrim(string& str) {
    string::iterator itr = find_if(str.begin(), str.end(), IsNotSpace);
    str.erase(str.begin(), itr);
    return str;
}

string& StrUtil::TrimSpace(string& str) {
    return StrUtil::RightTrim(StrUtil::LeftTrim(str));
}

string StrUtil::TrimSpace_c(const std::string& cstr) {
    string s(cstr);
    return TrimSpace(s);
}

string StrUtil::LeftTrim_c(const std::string& cstr) {
    string s(cstr);
    return LeftTrim(s);
}

string StrUtil::RightTrim_c(const std::string& cstr) {
    string s(cstr);
    return RightTrim(s);
}

void StrUtil::ReplaceAll(std::string & s, const std::string & t, const std::string & w)
{
    if (0 ==t.size())
        return;

    string::size_type pos = s.find(t);
    string::size_type t_size = t.size();
    string::size_type r_size = w.size();
    while(pos != std::string::npos) { // found
        s.replace(pos, t_size, w);
        pos = s.find(t, pos + r_size );
    }
}

void StrUtil::ReplaceAll( std::string & s, const char* t, const char* w )
{
    string s_t(t);
    string s_w(w);
    ReplaceAll(s, s_t, s_w);
}

bool StrUtil::ToBoolean( const std::string &str )
{
    string upper = StrUtil::ToUpper(str.c_str());
    upper = StrUtil::TrimSpace(upper);
    if ("FALSE" == upper) {
        return false;
    } else if ("TRUE" == upper) {
        return true;
    } else if (0 == StrUtil::ToInt(upper)) {
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////
typedef multimap<int,string>            ErrnoNames_t;
typedef ErrnoNames_t::iterator          ErrnoNames_Iter;
typedef ErrnoNames_t::const_iterator    ErrnoNames_Citer;

/* <errno.h>.  */
#define INSERT(_int_errno_, _str_name_) \
    (mm).insert(make_pair<int, string>(_int_errno_, _str_name_))

static void InitErr2Name(ErrnoNames_t& mm) {
    /* <asm-generic/errno-base.h> */
    INSERT(EPERM    , "EPERM");
    INSERT(ENOENT   , "ENOENT");
    INSERT(ESRCH    , "ESRCH");
    INSERT(EINTR    , "EINTR");
    INSERT(EIO      , "EIO");
    INSERT(ENXIO    , "ENXIO");
    INSERT(E2BIG    , "E2BIG");
    INSERT(ENOEXEC  , "ENOEXEC");
    INSERT(EBADF    , "EBADF");
    INSERT(ECHILD   , "ECHILD");
    INSERT(EAGAIN   , "EAGAIN");
    INSERT(ENOMEM   , "ENOMEM");
    INSERT(EACCES   , "EACCES");
    INSERT(EFAULT   , "EFAULT");
    INSERT(ENOTBLK  , "ENOTBLK");
    INSERT(EBUSY    , "EBUSY");
    INSERT(EEXIST   , "EEXIST");
    INSERT(EXDEV    , "EXDEV");
    INSERT(ENODEV   , "ENODEV");
    INSERT(ENOTDIR  , "ENOTDIR");
    INSERT(EISDIR   , "EISDIR");
    INSERT(EINVAL   , "EINVAL");
    INSERT(ENFILE   , "ENFILE");
    INSERT(EMFILE   , "EMFILE");
    INSERT(ENOTTY   , "ENOTTY");
    INSERT(ETXTBSY  , "ETXTBSY");
    INSERT(EFBIG    , "EFBIG");
    INSERT(ENOSPC   , "ENOSPC");
    INSERT(ESPIPE   , "ESPIPE");
    INSERT(EROFS    , "EROFS");
    INSERT(EMLINK   , "EMLINK");
    INSERT(EPIPE    , "EPIPE");
    INSERT(EDOM     , "EDOM");
    INSERT(ERANGE   , "ERANGE");
    
    /* <asm-generic/errno.h> */
    INSERT(EDEADLK      , "EDEADLK");
    INSERT(ENAMETOOLONG , "ENAMETOOLONG");
    INSERT(ENOLCK       , "ENOLCK");
    INSERT(ENOSYS       , "ENOSYS");
    INSERT(ENOTEMPTY    , "ENOTEMPTY");
    INSERT(ELOOP        , "ELOOP");
    INSERT(EWOULDBLOCK  , "EWOULDBLOCK");
    INSERT(ENOMSG       , "ENOMSG");
    INSERT(EIDRM        , "EIDRM");
    INSERT(ECHRNG       , "ECHRNG");
    INSERT(EL2NSYNC     , "EL2NSYNC");
    INSERT(EL3HLT       , "EL3HLT");
    INSERT(EL3RST       , "EL3RST");
    INSERT(ELNRNG       , "ELNRNG");
    INSERT(EUNATCH      , "EUNATCH");
    INSERT(ENOCSI       , "ENOCSI");
    INSERT(EL2HLT       , "EL2HLT");
    INSERT(EBADE        , "EBADE");
    INSERT(EBADR        , "EBADR");
    INSERT(EXFULL       , "EXFULL");
    INSERT(ENOANO       , "ENOANO");
    INSERT(EBADRQC      , "EBADRQC");
    INSERT(EBADSLT      , "EBADSLT");
    
    INSERT(EDEADLOCK    , "EDEADLOCK");
    
    INSERT(EBFONT         , "EBFONT");
    INSERT(ENOSTR         , "ENOSTR");
    INSERT(ENODATA        , "ENODATA");
    INSERT(ETIME          , "ETIME");
    INSERT(ENOSR          , "ENOSR");
    INSERT(ENONET         , "ENONET");
    INSERT(ENOPKG         , "ENOPKG");
    INSERT(EREMOTE        , "EREMOTE");
    INSERT(ENOLINK        , "ENOLINK");
    INSERT(EADV           , "EADV");
    INSERT(ESRMNT         , "ESRMNT");
    INSERT(ECOMM          , "ECOMM");
    INSERT(EPROTO         , "EPROTO");
    INSERT(EMULTIHOP      , "EMULTIHOP");
    INSERT(EDOTDOT        , "EDOTDOT");
    INSERT(EBADMSG        , "EBADMSG");
    INSERT(EOVERFLOW      , "EOVERFLOW");
    INSERT(ENOTUNIQ       , "ENOTUNIQ");
    INSERT(EBADFD         , "EBADFD");
    INSERT(EREMCHG        , "EREMCHG");
    INSERT(ELIBACC        , "ELIBACC");
    INSERT(ELIBBAD        , "ELIBBAD");
    INSERT(ELIBSCN        , "ELIBSCN");
    INSERT(ELIBMAX        , "ELIBMAX");
    INSERT(ELIBEXEC       , "ELIBEXEC");
    INSERT(EILSEQ         , "EILSEQ");
    INSERT(ERESTART       , "ERESTART");
    INSERT(ESTRPIPE       , "ESTRPIPE");
    INSERT(EUSERS         , "EUSERS");
    INSERT(ENOTSOCK       , "ENOTSOCK");
    INSERT(EDESTADDRREQ   , "EDESTADDRREQ");
    INSERT(EMSGSIZE       , "EMSGSIZE");
    INSERT(EPROTOTYPE     , "EPROTOTYPE");
    INSERT(ENOPROTOOPT    , "ENOPROTOOPT");
    INSERT(EPROTONOSUPPORT, "EPROTONOSUPPORT");
    INSERT(ESOCKTNOSUPPORT, "ESOCKTNOSUPPORT");
    INSERT(EOPNOTSUPP     , "EOPNOTSUPP");
    INSERT(EPFNOSUPPORT   , "EPFNOSUPPORT");
    INSERT(EAFNOSUPPORT   , "EAFNOSUPPORT");
    INSERT(EADDRINUSE     , "EADDRINUSE");
    INSERT(EADDRNOTAVAIL  , "EADDRNOTAVAIL");
    INSERT(ENETDOWN       , "ENETDOWN");
    INSERT(ENETUNREACH    , "ENETUNREACH");
    INSERT(ENETRESET      , "ENETRESET");
    INSERT(ECONNABORTED   , "ECONNABORTED");
    INSERT(ECONNRESET     , "ECONNRESET");
    INSERT(ENOBUFS        , "ENOBUFS");
    INSERT(EISCONN        , "EISCONN");
    INSERT(ENOTCONN       , "ENOTCONN");
    INSERT(ESHUTDOWN      , "ESHUTDOWN");
    INSERT(ETOOMANYREFS   , "ETOOMANYREFS");
    INSERT(ETIMEDOUT      , "ETIMEDOUT");
    INSERT(ECONNREFUSED   , "ECONNREFUSED");
    INSERT(EHOSTDOWN      , "EHOSTDOWN");
    INSERT(EHOSTUNREACH   , "EHOSTUNREACH");
    INSERT(EALREADY       , "EALREADY");
    INSERT(EINPROGRESS    , "EINPROGRESS");
    INSERT(ESTALE         , "ESTALE");
    INSERT(EUCLEAN        , "EUCLEAN");
    INSERT(ENOTNAM        , "ENOTNAM");
    INSERT(ENAVAIL        , "ENAVAIL");
    INSERT(EISNAM         , "EISNAM");
    INSERT(EREMOTEIO      , "EREMOTEIO");
    INSERT(EDQUOT         , "EDQUOT");

    INSERT(ENOMEDIUM   ,    "ENOMEDIUM");
    INSERT(EMEDIUMTYPE ,    "EMEDIUMTYPE");
    INSERT(ECANCELED   ,    "ECANCELED");
    INSERT(ENOKEY      ,    "ENOKEY");
    INSERT(EKEYEXPIRED ,    "EKEYEXPIRED");
    INSERT(EKEYREVOKED ,    "EKEYREVOKED");
    INSERT(EKEYREJECTED,    "EKEYREJECTED");
    
    INSERT(EOWNERDEAD,      "EOWNERDEAD");
    INSERT(ENOTRECOVERABLE, "ENOTRECOVERABLE");
}

string StrUtil::ErrnoName(int err_no) {
    static ErrnoNames_t s_names;
    if (s_names.empty())
        InitErr2Name(s_names);
    
    string result;
    pair<ErrnoNames_Citer, ErrnoNames_Citer> p = s_names.equal_range(err_no);
    for (ErrnoNames_Citer citer = p.first; citer != p.second; ++citer) {
        if (citer != p.first)
            result += "|";
        result += citer->second;
    }
    return result;
}
