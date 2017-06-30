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

using namespace zylkowsk::Client::ProcessList;
using namespace zylkowsk::Client::WatcherConnection;
using namespace zylkowsk::Common;

int main(int argc, char *argv[]) {
    if (4 != argc) {
        fprintf(stderr, "Usage: %s <Server host> <Server Port> <Interval>\n", argv[0]);
        return 1;
    }
    int interval;
    if (0 == (interval = atoi(argv[3]))) {
        fprintf(stderr, "Interval must be an integer value\n");
        return 1;
    }

    Hasher hasher;
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

    try {
        ProcessesReader processesReader;
        while (true) {
            communicator.sendProcessesList(processesReader.getRunningProcesses());

            sleep((unsigned) interval);
        }
    } catch (Exception &e) {
        fprintf(stderr, "%s\n", e.what());
        return e.code();
    }
}