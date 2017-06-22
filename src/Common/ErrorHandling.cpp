//
// Created by Mikołaj Żyłkowski on 22.06.2017.
//

#include "ErrorHandling.h"
#include <stdarg.h>
#include <string>

using namespace zylkowsk::Common::ErrorHandling;

Exception::Exception(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(error, MAX_ERROR_LENGTH, format, args);
    va_end(args);
}

const char* Exception::what() const throw()
{
    return error;
}