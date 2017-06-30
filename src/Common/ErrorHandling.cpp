//
// Created by Mikołaj Żyłkowski on 22.06.2017.
//

#include "ErrorHandling.h"

using namespace zylkowsk::Common;

Exception::Exception(int code, std::string format, ...) : errorCode(code) {
    va_list args;
    va_start(args, format);
    setErrorMessage(format.c_str(), args);
    va_end(args);
}

Exception::Exception(int code, const char *format, ...) : errorCode(code)
{
    va_list args;
    va_start(args, format);
    setErrorMessage(format, args);
    va_end(args);
}

Exception::Exception(const char *format, ...) : errorCode(99) {
    va_list args;
    va_start(args, format);
    setErrorMessage(format, args);
    va_end(args);
}

void Exception::setErrorMessage(const char *format, va_list args) {
    vsnprintf(error, MAX_ERROR_LENGTH, format, args);
}

const int Exception::code() {
    return errorCode;
}

const char* Exception::what() const throw()
{
    return error;
}
