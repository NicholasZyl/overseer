//
// Created by Mikołaj Żyłkowski on 01.07.2017.
//

#include "Server.h"
#include <unistd.h>
#include <sstream>
#include <syslog.h>

using namespace zylkowsk::Server::Checker;

HostsSubmissionMonitor::HostsSubmissionMonitor(unsigned int interval, Server::Overseer::HostsRegistrar registrar, Logger logger) : interval(interval), registrar(registrar), logger(logger) {}

void HostsSubmissionMonitor::start() {
    while (true) {
        auto now = time(nullptr);
        auto hosts = registrar.getWatchedHosts();
        for (auto host : hosts) {

            time_t expectedMessageTime = host.getNextMessageTime();
            if (expectedMessageTime < now) {
                std::stringstream message("");
                message << "Watched host " << host.getIp() << " missed submission at " << Logger::formatTime(expectedMessageTime);
                logger.log(LOG_ERR, message.str());
            }
        }

        sleep(interval);
    }
}
