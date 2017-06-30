//
// Created by Mikołaj Żyłkowski on 30.06.2017.
//

#include "Logger.h"
#include <ctime>
#include <syslog.h>

#define TIME_BUFFER 20

const std::string zylkowsk::Common::Logger::formatTime(time_t timestamp) {
    char formattedTime[TIME_BUFFER];
    strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d %H:%M:%S", localtime(&timestamp));

    return std::string(formattedTime);
}

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
    time_t now = time(nullptr);

    if (useSyslog) {
        syslog(level, "%s: %s\n", formatTime(now).c_str(), message);
    } else if (level <= LOG_ERR) {
        fprintf(stderr, "%s: %s\n", formatTime(now).c_str(), message);
    } else {
        fprintf(stdout, "%s: %s\n", formatTime(now).c_str(), message);
    }
}
