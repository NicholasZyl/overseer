//
// Created by Mikołaj Żyłkowski on 30.06.2017.
//

#ifndef PROJEKT_LOGGER_H
#define PROJEKT_LOGGER_H


#include <iosfwd>
#include <string>

namespace zylkowsk {
    namespace Common {
        /**
         * @class Logger
         * Simple class to log message in either syslog or to standard output.
         */
        class Logger {
            /**
             * Should syslog be used
             */
            bool useSyslog;

        public:
            /**
             * Constructor.
             *
             * @param name Name used for syslog.
             * @param useSyslog Should syslog be used.
             */
            Logger(std::string name, bool useSyslog);

            /**
             * Destructor
             */
            ~Logger();

            /**
             *	Log message with given level to syslog if turned on.
             *	@param level Level of the log, must be valid for syslog function.
             *	@param message Message to be logged.
             */
            void log(int level, std::string message);

            /**
             *	Log message with given level to syslog if turned on.
             *	@param level Level of the log, must be valid for syslog function.
             *	@param message Message to be logged.
             */
            void log(int level, const char *message);
        };
    }
}


#endif //PROJEKT_LOGGER_H
