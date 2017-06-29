//
// Created by Mikołaj Żyłkowski on 29.06.2017.
//

#ifndef PROJEKT_SERVER_H
#define PROJEKT_SERVER_H


#include "Overseer.h"
#include "../Common/Hasher.h"

using namespace zylkowsk::Common::Hasher;
using namespace zylkowsk::Server::Overseer;

namespace zylkowsk {
    namespace Server {
        class Server {
            HostsRegistrar registrar;

            ProcessListHasher hasher;

            void processIncomingConnection(int incomingSocket, struct sockaddr_in incomingAddress);

            void processHostRegistration(int incomingSocket, std::string hostIp, std::string commandArgument);

            void processHostProcessesMessage(int incomingSocket, std::string hostIp, std::string commandArgument);

        public:
            Server(HostsRegistrar hostsRegistrar, ProcessListHasher processListHasher);

            void run();
        };
    }
}


#endif //PROJEKT_SERVER_H
