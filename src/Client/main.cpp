//
// Created by Mikołaj Żyłkowski on 22.06.2017.
//

#include "../Common/Communication.h"
#include "../Common/ErrorHandling.h"
#include "../Common/Hasher.h"
#include "../Common/NetUtils.h"
#include "ProcessList.h"
#include "WatcherConnection.h"
#include <iostream>
#include <unistd.h>

#define MILISECONDS_IN_SECOND 1000

using namespace zylkowsk::Client::ProcessList;
using namespace zylkowsk::Client::WatcherConnection;
using namespace zylkowsk::Common::Communication;
using namespace zylkowsk::Common::ErrorHandling;
using namespace zylkowsk::Common::Hasher;
using namespace zylkowsk::Common::NetUtils;

int main(int argc, char *argv[]) {
    if (4 != argc) {
        fprintf(stderr, "Usage: %s <Server IP> <Server Port> <Interval>\n", argv[0]);
        return 1;
    }
    int interval;
    if (0 == (interval = atoi(argv[3]))) {
        fprintf(stderr, "Interval must be an integer value\n");
        return 1;
    }

    ProcessListHasher hasher;
    Client baseClient(argv[1], argv[2], TCP);
    Communicator communicator(baseClient, hasher);

    try {
        communicator.registerClient((unsigned) interval);
    } catch (Exception &e) {
        if (ERR_CODE_HOST_WATCHED != e.code()) {
            fprintf(stderr, "%s\n", e.what());
            return e.code();
        }
    }

    ProcessesReader processesReader;
    while (true) {
        communicator.sendProcessesList(processesReader.getRunningProcesses());

        sleep((unsigned) (interval * MILISECONDS_IN_SECOND));
    }
}