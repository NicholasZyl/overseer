//
// Created by Mikołaj Żyłkowski on 27.06.2017.
//

#include "../Common/ErrorHandling.h"
#include "../Common/Hasher.h"
#include "Overseer.h"
#include "Server.h"

using namespace zylkowsk::Common::ErrorHandling;
using namespace zylkowsk::Common::Hasher;
using namespace zylkowsk::Server;
using namespace zylkowsk::Server::Overseer;

int main(int argc, char *argv[]) {
    try {
        HostsRegistrar registrar;
        ProcessListHasher hasher;
        Server overseer(registrar, hasher);
        overseer.run(51435, 10, false);
    } catch (Exception &e) {
        fprintf(stderr, "%s\n", e.what());

        return e.code();
    }

    return 0;
}