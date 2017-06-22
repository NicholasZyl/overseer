//
// Created by Mikołaj Żyłkowski on 22.06.2017.
//

#ifndef PROJEKT_ERROR_HANDLING_H
#define PROJEKT_ERROR_HANDLING_H

#include <exception>

#define MAX_ERROR_LENGTH 256

namespace zylkowsk {
    namespace Common {
        namespace ErrorHandling {
            /**
             *	@class Exception
             *	Exception class thrown in case of errors.
             */
            class Exception : public std::exception {
                /**
                 *	Error that happened.
                 */
                char error[MAX_ERROR_LENGTH];

            public:
                /**
                 *	Constructor.
                 *	Exception allows to set errors in same format as printf function.
                 *	@param format Format is printf style.
                 */
                Exception(const char *format, ...);

                /**
                 *	Overridden what method to get actual error message.
                 */
                virtual const char *what() const throw();
            };
        };
    };
};

#endif //PROJEKT_ERROR_HANDLING_H
