//
// Created by Mikołaj Żyłkowski on 29.06.2017.
//

#ifndef PROJEKT_SERVER_H
#define PROJEKT_SERVER_H


#include "../Common/Hasher.h"
#include "../Common/Logger.h"
#include "Overseer.h"

using namespace zylkowsk::Common;
using namespace zylkowsk::Server::Overseer;

namespace zylkowsk {
    namespace Server {
        /**
         * @class Application
         * Base Overseer server class used to listen for connections and processing them.
         */
        class Application {
            /**
             * Registrar used to watch hosts.
             */
            HostsRegistrar registrar;

            /**
             * Hasher used to confirm sent processes list.
             */
            Hasher hasher;

            /**
             * A logger instance.
             */
            Logger logger;

            /**
             * Properly process client connection.
             *
             * @param incomingSocket
             * @param incomingAddress
             */
            void processIncomingConnection(int incomingSocket, struct sockaddr_in incomingAddress);

            /**
             * Process new host registration action.
             *
             * @param incomingSocket Connection socket descriptor.
             * @param hostIp IP of the watched host.
             * @param commandArgument Argument passed to the command from the Client.
             */
            void processHostRegistration(int incomingSocket, std::string hostIp, std::string commandArgument);

            /**
             * Process receving message with processes list from registered host.
             *
             * @param incomingSocket Connection socket descriptor.
             * @param hostIp IP of the watched host.
             * @param commandArgument Argument passed to the command from the Client.
             */
            void processHostProcessesMessage(int incomingSocket, std::string hostIp, std::string commandArgument);

        public:
            /**
             * Constructor with all dependencies.
             *
             * @param hostsRegistrar Registrar used to watch hosts.
             * @param hasher Hasher used to confirm sent processes list.
             * @param logger A logger instance.
             */
            Application(HostsRegistrar hostsRegistrar, Hasher hasher, Logger logger);

            /**
             * Run Server application with given configuration.
             *
             * @param port Port on which server should be listening.
             * @param processesLimit Limit of the child processes used to process requests.
             * @param asDaemon Should be run as a daemon.
             */
            void run(int port, unsigned int processesLimit, bool asDaemon);
        };
    }
}


#endif //PROJEKT_SERVER_H
