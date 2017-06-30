//
// Created by Mikołaj Żyłkowski on 30.06.2017.
//

#ifndef PROJEKT_APPLICATION_H
#define PROJEKT_APPLICATION_H

#include "ProcessList.h"
#include "WatcherConnection.h"

using namespace zylkowsk::Client::ProcessList;
using namespace zylkowsk::Client::WatcherConnection;

namespace zylkowsk {
    namespace Client {
        /**
         * @class Application
         * Client application.
         */
        class Application {
            /**
             * Communicator module used to send proper commands to the server.
             */
            Communicator communicator;

            /**
             * Processes reader module for reading running processes.
             */
            ProcessesReader processesReader;
        public:
            /**
             * Constructor with all needed dependencies injected.
             *
             * @param communicator
             * @param processesReader
             */
            Application(Communicator communicator, ProcessesReader processesReader);

            /**
             * Run application: try to register host on server and start sending processes list.
             *
             * @param interval How often client should send processes list, in seconds.
             * @param asDaemon Should client run as a daemon.
             * @throws Exception if an error occurs.
             */
            void run(unsigned int interval, bool asDaemon);
        };
    }
}


#endif //PROJEKT_APPLICATION_H
