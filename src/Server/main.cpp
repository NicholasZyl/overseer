//
// Created by Mikołaj Żyłkowski on 27.06.2017.
//

#include "../Common/Communication.h"
#include "../Common/ErrorHandling.h"
#include "../Common/Hasher.h"
#include "../Common/Logger.h"
#include "Overseer.h"
#include "Application.h"
#include <cstdlib>
#include <getopt.h>

#define DEFAULT_CHILD_PROCESSES_LIMIT 10

using namespace zylkowsk::Common;
using namespace zylkowsk::Server;
using namespace zylkowsk::Server::Overseer;

void helpInfo(char *name, int exitCode);

int main(int argc, char *argv[]) {
    int port = SERVER_DEFAULT_PORT;
    bool runAsDaemon = true;
    unsigned int childProcessesLimit = DEFAULT_CHILD_PROCESSES_LIMIT;

    const char* const options = "hl:np:";
    const struct option longOptions[] = {
        { "help", 0, NULL, 'h' },
        { "limit", 1, NULL, 'l' },
        { "no-daemon", 0, NULL, 'n' },
        { "port", 1, NULL, 'p' },
        { NULL, 0, NULL, 0 }
    };

    int opt;
    while (-1 != (opt = getopt_long(argc, argv, options, longOptions, NULL))) {
        switch (opt) {
            case 'h':
                helpInfo(argv[0], EXIT_SUCCESS);
                break;
            case 'l':
                childProcessesLimit = (unsigned int) atoi(optarg);
                break;
            case 'n':
                runAsDaemon = false;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case -1:
                break;
            case '?':
            default:
                helpInfo(argv[0], EXIT_FAILURE);
                break;
        }
    }

    try {
        HostsRegistrar registrar;
        Hasher hasher;
        Logger logger("Overseer", runAsDaemon);
        Application overseer(registrar, hasher, logger);
        overseer.run(port, childProcessesLimit, runAsDaemon);
    } catch (Exception &e) {
        fprintf(stderr, "%s\n", e.what());

        return e.code();
    }

    return 0;
}

void helpInfo(char *name, int exitCode) {
    fprintf(stdout, "Usage: %s options\n", name);
    fprintf(stdout,
            " -h  --help             Display this information.\n"
            " -l  --limit limit      Set limit for child processes.\n"
            " -n  --no-daemon        Do not run as a daemon.\n"
            " -p  --port port_num    Use port different than default.\n"
    );

    exit(exitCode);
}