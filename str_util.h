#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <sstream>
#include <ostream>

namespace StrUtil
{
    void Split(const std::string &input, const char *delimiters,
                     std::vector<std::string> &outputVector,
                     bool bTrimSpace = true, bool bIgnoreEmpty = true);

    void Split(const std::string &input, const char *delimiters,
                     std::vector<std::string> &outputVector,
                     const std::string& strBeg, const std::string& strEnd,
                     bool bTrimSpace = true, bool bIgnoreEmpty = true);

    void ToUpper(char *str);
    std::string ToUpper(const char* str);
    void ToLower(char *str);
    std::string ToLower(const char* str);

    template <typename Type_T>
    inline std::string ToString(Type_T d) {
        std::stringstream ss;
        ss << d;
        return ss.str();
    }

    std::string IntToString(int i);
    std::string UintToString(unsigned int ui);
    std::string LongToString(long l);
    std::string FloatToString(float f);
    std::string DoubleToString(double d);
    std::string BooleanToString(bool b);

    bool IsFloat(const std::string &valueStr, float *pValue = 0);
    bool IsInt(const std::string &valueStr, int *pValue = 0);
    bool IsUint(const std::string &valueStr, unsigned int *pValue = 0);
    bool IsLong(const std::string &valueStr, long *pValue = 0);
    bool IsUlonglong(const std::string &valueStr, unsigned long long *pValue = 0);

    int ToInt(const std::string &str);
    float ToFloat(const std::string &str);
    unsigned int ToUint(const std::string &str);
    long ToLong(const std::string &str);
    unsigned long long ToUlonglong(const std::string &str);

    bool ToBoolean(const std::string &str);

    std::string& TrimSpace(std::string& str);
    std::string& LeftTrim (std::string& str);
    std::string& RightTrim(std::string& str);

    std::string TrimSpace_c(const std::string&);
    std::string LeftTrim_c (const std::string&);
    std::string RightTrim_c(const std::string&);

    // replace all occurance of t in s to w
    void ReplaceAll(std::string & s, const std::string & t, const std::string & w);
    void ReplaceAll(std::string & s, const char* t, const char* w);

    //! get errno name in <errno.h>
    std::string ErrnoName(int err_no);
} // namespace StrUtil

/// C++11 only
template <class Ty_>
std::ostream& operator<< (std::ostream& os, const std::vector<Ty_>& v) {
    size_t s = v.size();
    os << "[";
    for (size_t i = 0; i < s; ++i) {
        os << v[i];
        if (i < s - 1)
            os << ", ";
    }
    os << "]";
    return os;
}

template <typename _Key, typename _Tp, typename _Compare, typename _Alloc>
std::ostream& operator<< (std::ostream& os, const std::map<_Key, _Tp, _Compare, _Alloc>& m) {
    os << "{";
    typename std::map<_Key, _Tp, _Compare, _Alloc>::const_reverse_iterator rbegin_citer = m.rbegin();
    for (typename std::map<_Key, _Tp, _Compare, _Alloc>::const_iterator citer = m.begin(); citer != m.end(); ++citer) {
        os << citer->first << ":" << citer->second;
        if (&(citer->second) != &(rbegin_citer->second))
            os << ", ";
    }
    os << "}";
    return os;
}

template <typename _Key, typename _Tp, typename _Compare, typename _Alloc>
std::ostream& operator<< (std::ostream& os, const std::multimap<_Key, _Tp, _Compare, _Alloc>& m) {
    os << "{";
    typename std::multimap<_Key, _Tp, _Compare, _Alloc>::const_reverse_iterator rbegin_citer = m.rbegin();
    for (typename std::multimap<_Key, _Tp, _Compare, _Alloc>::const_iterator citer = m.begin(); citer != m.end(); ++citer) {
        os << citer->first << ":" << citer->second;
        if (&(citer->second) != &(rbegin_citer->second))
            os << ", ";
    }
    os << "}";
    return os;
}

template <typename _Key, typename _Compare, typename _Alloc>
std::ostream& operator<< (std::ostream& os, const std::set<_Key, _Compare, _Alloc>& m) {
    os << "<";
    typename std::set<_Key, _Compare, _Alloc>::const_reverse_iterator rbegin_citer = m.rbegin();
    for (typename std::set<_Key, _Compare, _Alloc>::const_iterator citer = m.begin(); citer != m.end(); ++citer) {
        os << *citer;
        if (&(*citer) != &(*rbegin_citer))
            os << ", ";
    }
    os << ">";
    return os;
}

template <typename _Key, typename _Compare, typename _Alloc>
std::ostream& operator<< (std::ostream& os, const std::multiset<_Key, _Compare, _Alloc>& m) {
    os << "<";
    typename std::multiset<_Key, _Compare, _Alloc>::const_reverse_iterator rbegin_citer = m.rbegin();
    for (typename std::multiset<_Key, _Compare, _Alloc>::const_iterator citer = m.begin();
         citer != m.end(); ++citer) {
        os << *citer;
        if (&(*citer) != &(*rbegin_citer))
            os << ", ";
    }
    os << ">";
    return os;
}

template <typename _Tp, typename _Alloc>
std::ostream& operator<< (std::ostream& os, const std::list<_Tp, _Alloc>& l) {
    os << "[";
    typename std::list<_Tp, _Alloc>::const_reverse_iterator rbegin_citer = l.rbegin();
    for (typename std::list<_Tp, _Alloc>::const_iterator citer = l.begin(); citer != l.end(); ++citer) {
        os << *citer;
        if (&(*citer) != &(*rbegin_citer))
            os << ", ";
    }
    os << "]";
    return os;
}

#define CSTR_TO_STDSTRING(_zc_) ((_zc_ && strlen(_zc_)) ? _zc_ : "")
