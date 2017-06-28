//
// Created by Mikołaj Żyłkowski on 27.06.2017.
//

#include "Overseer.h"

using namespace zylkowsk::Server::Overseer;

int main(int argc, char *argv[]) {
    HostsRegistrar registrar;

    registrar.registerHost("127.0.0.1", 5);
    registrar.storeHostChangedProcessesList("127.0.0.1", "abc", {"bash", "test"});

    return 0;
}