//
// Created by Mikołaj Żyłkowski on 22.06.2017.
//

#include "../Common/Communication.h"
#include "../Common/ErrorHandling.h"
#include "../Common/Hasher.h"
#include "../Common/NetUtils.h"
#include "Application.h"
#include "ProcessList.h"
#include "WatcherConnection.h"
#include <getopt.h>
#include <iostream>
#include <unistd.h>

#define DEFAULT_INTERVAL 60

using namespace zylkowsk::Client;
using namespace zylkowsk::Client::ProcessList;
using namespace zylkowsk::Client::WatcherConnection;
using namespace zylkowsk::Common;

void helpInfo(char *name, int exitCode);

int main(int argc, char *argv[]) {
    const char *serviceName = NULL;
    bool runAsDaemon = true;
    unsigned int interval = DEFAULT_INTERVAL;

    const char* const options = ":hi:ns:";
    const struct option longOptions[] = {
            { "help", 0, NULL, 'h' },
            { "interval", 1, NULL, 'i' },
            { "no-daemon", 0, NULL, 'n' },
            { "service", 1, NULL, 's' },
            { NULL, 0, NULL, 0 }
    };

    int opt;
    while (-1 != (opt = getopt_long(argc, argv, options, longOptions, NULL))) {
        switch (opt) {
            case 'h':
                helpInfo(argv[0], EXIT_SUCCESS);
                break;
            case 'i':
                interval = (unsigned int) atoi(optarg);
                break;
            case 'n':
                runAsDaemon = false;
                break;
            case 's':
                serviceName = optarg;
                break;
            case -1:
                break;
            case '?':
            default:
                helpInfo(argv[0], EXIT_FAILURE);
                break;
        }
    }
    if (argc == optind) {
        helpInfo(argv[0], EXIT_FAILURE);
    }

    const char *service;
    if (NULL == serviceName) {
        service = std::to_string(SERVER_DEFAULT_PORT).c_str();
    } else {
        service = serviceName;
    }

    try {
        Hasher hasher;
        class Client baseClient(argv[optind], service, TCP);
        Communicator communicator(baseClient, hasher);
        ProcessesReader processesReader;
        Application client(communicator, processesReader);

        client.run(interval, runAsDaemon);
    } catch (Exception &e) {
        fprintf(stderr, "%s\n", e.what());
        return e.code();
    }
}

void helpInfo(char *name, int exitCode) {
    fprintf(stdout, "Usage: %s options <Server host>\n", name);
    fprintf(stdout,
            " -h  --help             Display this information.\n"
            " -i  --interval int     Use different interval than default.\n"
            " -n  --no-daemon        Do not run as a daemon.\n"
            " -s  --service service  Use service name or port different than default. Can be either service name or port number. \n"
    );

    exit(exitCode);
}