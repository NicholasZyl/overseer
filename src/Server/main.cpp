//
// Created by Mikołaj Żyłkowski on 27.06.2017.
//

#include "../Common/NetUtils.h"
#include "Overseer.h"
#include "Server.h"

using namespace zylkowsk::Common;
using namespace zylkowsk::Server;
using namespace zylkowsk::Server::Overseer;

int main(int argc, char *argv[]) {
    HostsRegistrar registrar;
    Hasher::ProcessListHasher hasher;
    Server overseer(registrar, hasher);

    overseer.run();

    return 0;
}