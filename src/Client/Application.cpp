//
// Created by Mikołaj Żyłkowski on 30.06.2017.
//

#include "../Common/Communication.h"
#include "../Common/ErrorHandling.h"
#include "Application.h"
#include <cerrno>
#include <stdlib.h>
#include <zconf.h>

using namespace zylkowsk::Client;

Application::Application(Communicator communicator, ProcessesReader processesReader) : communicator(communicator), processesReader(processesReader) {}

void Application::run(unsigned int interval, bool asDaemon) {
    try {
        communicator.registerClient(interval);
    } catch (Exception &e) {
        if (ERR_CODE_HOST_WATCHED != e.code()) {
            throw e;
        }
    }
    if (asDaemon) {
        if (-1 == daemon(0, 0)) {
            throw Exception("Unable to daemonize client with error: %s", strerror(errno));
        }
    }

    while (true) {
        communicator.sendProcessesList(processesReader.getRunningProcesses());
        sleep(interval);
    }
}
