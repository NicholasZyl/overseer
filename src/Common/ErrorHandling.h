//
// Created by Mikołaj Żyłkowski on 22.06.2017.
//

#ifndef PROJEKT_ERROR_HANDLING_H
#define PROJEKT_ERROR_HANDLING_H


#include <cstdarg>
#include <exception>
#include <string>

#define MAX_ERROR_LENGTH 256

namespace zylkowsk {
    namespace Common {
        /**
         *	@class Exception
         *	Exception class thrown in case of errors.
         */
        class Exception : public std::exception {
            /**
             *  Error code.
             */
            int errorCode;

            /**
             *	Error that happened.
             */
            char error[MAX_ERROR_LENGTH];

            /**
             *	Prepare error message.
             *
             *	@param format Format in printf style.
             *	@param args Arguments passed to the formatted string
             */
            void setErrorMessage(const char *format, va_list args);

        public:
            /**
             *	Constructor.
             *	Exception allows to set errors in same format as printf function.
             *
             *	@param code Number representing error code.
             *	@param format Format in printf style.
             */
            Exception(int code, std::string format, ...);

            /**
             *	Constructor.
             *	Exception allows to set errors in same format as printf function.
             *
             *	@param code Number representing error code.
             *	@param format Format in printf style.
             */
            Exception(int code, const char *format, ...);

            /**
             *	Constructor that defaults code to 99.
             *
             *	@param format Format in printf style.
             */
            Exception(const char *format, ...);

            /**
             * Get error code.
             *
             * @return Error code.
             */
            const int code();

            /**
             *	Overridden what method to get actual error message.
             */
            virtual const char *what() const throw();
        };
    };
};

#endif //PROJEKT_ERROR_HANDLING_H
