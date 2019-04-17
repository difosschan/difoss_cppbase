#include "runtime_exception.h"
#include "str_util.h"

RuntimeException::RuntimeException(const std::string& reason, int errCode,
    const char * prettyFunc, const char* fileName, size_t line)
: std::runtime_error(reason)
, _error_code(errCode)
, _pretty_func(prettyFunc)
, _filename(fileName)
, _line(line)
{}

// \note 多线程静态局部变量会有问题
const char* RuntimeException::what() const throw() {
    static std::string str;
    str = "Error Code: ";
    str += StrUtil::ToString(_error_code);
    str += "\nPosition: ";
    str += _filename;
    str += ":";
    str += StrUtil::ToString(_line);
    str += " : ";
    str += _pretty_func;
    str += "\nReason: ";
    str += runtime_error::what();
    return str.c_str();
}
