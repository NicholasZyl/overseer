//
// Created by Mikołaj Żyłkowski on 30.06.2017.
//

#include <syslog.h>
#include "Logger.h"

zylkowsk::Common::Logger::Logger(std::string name, bool useSyslog) : useSyslog(useSyslog) {
    if (this->useSyslog) {
        openlog(name.c_str(), LOG_PID, LOG_USER);
    }
}

zylkowsk::Common::Logger::~Logger() {
    if (useSyslog) {
        closelog();
    }
}

void zylkowsk::Common::Logger::log(int level, std::string message) {
    log(level, message.c_str());
}

void zylkowsk::Common::Logger::log(int level, const char *message) {
    if (useSyslog) {
        syslog(level, "%s\n", message);
    } else if (level <= LOG_ERR) {
        fprintf(stderr, "%s\n", message);
    } else {
        fprintf(stdout, "%s\n", message);
    }
}
