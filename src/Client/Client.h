//
// Created by Mikołaj Żyłkowski on 01.07.2017.
//

#ifndef PROJEKT_CLIENT_H
#define PROJEKT_CLIENT_H


#include "../Common/Common.h"
#include <list>
#include <string>

using namespace zylkowsk::Common;

namespace zylkowsk {
    /**
     * @namespace Client
     * Separate namespace for the Client part of the application.
     */
    namespace Client {
        /**
         * @namespace ProcessList
         * Module responsible for extracting running processes as a list of processes names.
         */
        namespace ProcessList {
            /**
             *  @class ProcessesReader
             *  A class used to read all running processes on the host.
             */
            class ProcessesReader {
                /**
                 *  Default directory to check for running processes.
                 */
                static const std::string procDir;

                /**
                 *  Default file containing process name.
                 */
                static const std::string procCmdFile;

                /**
                 *  Extract name for the process with given PID.
                 *
                 *  @param pid Process id.
                 *  @return Name of the process.
                 */
                std::string getProcessName(int pid);

            public:
                /**
                 *  Get list of all running processes names.
                 *
                 *  @return List of processes names.
                 */
                std::list<std::string> getRunningProcesses();
            };
        }

        /**
         * @namespace WatcherConnection
         * A module responsible for communication with Overseer Server.
         */
        namespace WatcherConnection {
            /**
             *  @class Communicator
             *  A class used to communicate between client and server.
             */
            class Communicator {
                /**
                 *  Base client used for connection.
                 */
                class Client baseClient;

                /**
                 *  Hasher used to prepare processes list hash.
                 */
                Hasher processListHasher;

            public:
                /**
                 *  Constructor of the communicator.
                 *
                 *  @param client Base client used for communicating.
                 *  @param hasher Hasher used for preparing a hash of processes list.
                 */
                Communicator(class Client client, Hasher hasher);

                /**
                 *  Send a command to register a new client on the server.
                 *
                 *  @param interval How often client should send processes list, in seconds.
                 *	@throws zylkowsk::Common::ErrorHandling::Exception If server responds with error.
                 */
                void registerClient(unsigned int interval);

                /**
                 *  Send a command to inform on running processes. First only hash is sent, the list is sent only if needed.
                 *
                 *  @param processes Processes to be sent.
                 *	@throws zylkowsk::Common::ErrorHandling::Exception If server responds with error.
                 */
                void sendProcessesList(std::list<std::string> processes);
            };
        }

        /**
         * @class Application
         * Client application.
         */
        class Application {
            /**
             * Communicator module used to send proper commands to the server.
             */
            Client::WatcherConnection::Communicator communicator;

            /**
             * Processes reader module for reading running processes.
             */
            Client::ProcessList::ProcessesReader processesReader;
        public:
            /**
             * Constructor with all needed dependencies injected.
             *
             * @param communicator
             * @param processesReader
             */
            Application(Client::WatcherConnection::Communicator communicator, Client::ProcessList::ProcessesReader processesReader);

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


#endif //PROJEKT_CLIENT_H
