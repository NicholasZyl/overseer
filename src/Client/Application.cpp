//
// Created by Mikołaj Żyłkowski on 30.06.2017.
//

#include "../Common/Common.h"
#include "Client.h"
#include <cerrno>
#include <stdlib.h>
#include <zconf.h>

using namespace zylkowsk::Client;

Application::Application(Client::WatcherConnection::Communicator communicator, Client::ProcessList::ProcessesReader processesReader) : communicator(communicator), processesReader(processesReader) {}

void Application::run(unsigned int interval, bool asDaemon) {
    if (asDaemon) {
        if (-1 == daemon(0, 0)) {
            throw Exception("Unable to daemonize client with error: %s", strerror(errno));
        }
    }

    try {
        communicator.registerClient(interval);
    } catch (Exception &e) {
        if (ERR_CODE_HOST_WATCHED != e.code()) {
            throw e;
        }
    }

    while (true) {
        communicator.sendProcessesList(processesReader.getRunningProcesses());
        sleep(interval);
    }
}
