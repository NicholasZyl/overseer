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
        /**
         * @class Server
         * Base Overseer server class used to listen for connections and processing them.
         */
        class Server {
            /**
             * Registrar used to watch hosts.
             */
            HostsRegistrar registrar;

            /**
             * Hasher used to confirm sent processes list.
             */
            ProcessListHasher hasher;

            /**
             * Properly process client connection.
             *
             * @param incomingSocket
             * @param incomingAddress
             */
            void processIncomingConnection(int incomingSocket, struct sockaddr_in incomingAddress);

            void processHostRegistration(int incomingSocket, std::string hostIp, std::string commandArgument);

            void processHostProcessesMessage(int incomingSocket, std::string hostIp, std::string commandArgument);

        public:
            Server(HostsRegistrar hostsRegistrar, ProcessListHasher processListHasher);

            void run(int port, unsigned int  processesLimit, bool asDaemon);
        };
    }
}


#endif //PROJEKT_SERVER_H
