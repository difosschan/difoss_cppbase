#pragma once

#include <stdexcept>
#include <string>

class RuntimeException : public std::runtime_error {
protected:
    int         _error_code;
    std::string _pretty_func;
    std::string _filename;
    size_t      _line;

public:
    RuntimeException(const std::string& reason, int errCode, const char * prettyFunc, const char* fileName, size_t line);
    virtual ~RuntimeException() throw() {}
    virtual const char* what() const throw();

    int error_code() const { return _error_code; }
};
