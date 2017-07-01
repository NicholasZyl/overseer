//
// Created by Mikołaj Żyłkowski on 01.07.2017.
//

#ifndef PROJEKT_SERVER_H
#define PROJEKT_SERVER_H


#include "../Common/Common.h"

using namespace zylkowsk::Common;

namespace zylkowsk {
    /**
     * @namespace Server
     * Separate namespace for the Server part of the application.
     */
    namespace Server {

        /**
         * @namespace Overseer
         * A module responsible for registering and maintaining a list of watched hosts.
         */
        namespace Overseer {
            /**
             * @class WatchedHost
             * A simple Data Transfer Object calss used to pass watched host data.
             */
            class WatchedHost {
                /**
                 * Watched host ip address.
                 */
                std::string ip;
                /**
                 * Expected interval between messages sent from a client.
                 */
                unsigned int interval;

                /**
                 * Unix timestamp of the next expected message from the client.
                 */
                time_t nextMessageTime;

                /**
                 * Hash of the currently stored processes list.
                 */
                std::string processesListHash;

                /**
                 * Currently stored processes list.
                 */
                std::list<std::string> processesList;

            public:
                /**
                 * Base constructor for a newly registered host.
                 *
                 * @param hostIp IP address of the watched host.
                 * @param interval Interval between expected messages received from a watched client.
                 */
                WatchedHost(const std::string &hostIp, unsigned int interval);

                /**
                 * Constructor used for reading from the storage.
                 *
                 * @param hostIp IP address of the watched host.
                 * @param interval Interval between expected messages received from a watched client.
                 * @param nextMessageExpectedTime Unix timestamp of the next expected message from the client.
                 * @param processesListHash Hash of the currently stored processes list.
                 * @param processes Currently stored processes list.
                 */
                WatchedHost(const std::string &hostIp, unsigned int interval, const time_t nextMessageExpectedTime,
                            const std::string &processesListHash, const std::list<std::string> &processes);

                /**
                 * Replace currently stored processes list with a new one.
                 *
                 * @param hash
                 * @param processes
                 */
                void watchProcesses(const std::string &hash, const std::list<std::string> &processes);

                /**
                 * Set proper timestamp for the next expected message.
                 */
                void updateNextExpectedMessageTime();

                /**
                 * Getter for host ip.
                 *
                 * @return Watched host ip address.
                 */
                const std::string &getIp() const;

                /**
                 * Getter for configured interval.
                 *
                 * @return Interval between expected messages received from a watched client.
                 */
                unsigned int getInterval() const;

                /**
                 * Getter for next expected message time.
                 *
                 * @return Unix timestamp of the next expected message from the client.
                 */
                time_t getNextMessageTime() const;

                /**
                 * Getter for currently stored processes list hash.
                 *
                 * @return Hash of the currently stored processes list.
                 */
                const std::string &getProcessesListHash() const;

                /**
                 * Getter for currently stored processes list.
                 *
                 * @return Currently stored processes list.
                 */
                const std::list<std::string> &getProcessesList() const;
            };

            /**
             * @class HostsRegistrar
             * A class responsible for registering and maintaining registry of the watched hosts.
             */
            class HostsRegistrar {
                /**
                 * Path to the directory where all watched hosts' data is stored.
                 */
                static const std::string storageDir;

                /**
                 * Prepare a proper file name for the host storage.
                 *
                 * @param hostIp IP address of the watched host.
                 * @return Path to the host's storage file.
                 */
                std::string getHostStorageFileName(const std::string &hostIp);

                /**
                 * Check if a host with given IP is already watched by the server.
                 *
                 * @param hostIp IP address of the watched host.
                 * @return Is host watched
                 */
                bool isHostRegistered(std::string hostIp);

                /**
                 * Save the watched host's data in a storage.
                 *
                 * @param host Watched host data.
                 */
                void saveWatchedHostData(WatchedHost &host);

                /**
                 * Get watched host's data from the storage.
                 *
                 * @param hostIp IP address of the watched host.
                 * @return Watched host data.
                 * @throws zylkowsk::Common::ErrorHandling::Exception If storage file does not exist.
                 */
                WatchedHost getWatchedHostData(const std::string &hostIp);

            public:
                /**
                 * Constructor.
                 */
                HostsRegistrar();

                /**
                 * Register a new client as a watched host.
                 *
                 * @param hostIp IP address of the watched host.
                 * @param interval Interval between expected messages received from a watched client.
                 * @throws zylkowsk::Common::ErrorHandling::Exception If host is already watched.
                 */
                void registerHost(const std::string &hostIp, const unsigned int interval);

                /**
                 * Process a message received from a client. Check if a watched host with given IP has the same
                 * processes list based on hash. If hash didn't change then just update next expected message timestamp.
                 *
                 * @param hostIp IP address of the watched host.
                 * @param processesListHash Hash of the processes list.
                 * @return If message is fully processed so processes' lists hashes are the same.
                 * @throws zylkowsk::Common::ErrorHandling::Exception If host is not watched by the server.
                 */
                bool processMessageFromHost(const std::string &hostIp, const std::string &processesListHash);

                /**
                 * Store processes list received from the client in the storage.
                 *
                 * @param hostIp IP address of the watched host.
                 * @param processesListHash Hash of the processes list.
                 * @param processes List of the processes run on the watched client.
                 * @throws zylkowsk::Common::ErrorHandling::Exception If host is not watched by the server.
                 */
                void storeHostChangedProcessesList(const std::string &hostIp, const std::string &processesListHash,
                                                   const std::list<std::string> &processes);

                /**
                 * Get list of all watched hosts.
                 *
                 * @return List with all watched hosts.
                 */
                std::list<WatchedHost> getWatchedHosts();
            };
        }

        /**
         * @namespace Checker
         * A module responsible for monitoring all watched hosts and checking if every one of them send processes list in expected time.
         */
        namespace Checker {
            /**
             * @class HostsSubmissionMonitor
             * Class that monitors if every watched host send processes within expected time.
             */
            class HostsSubmissionMonitor {
                /**
                 * How often monitor should scan watched hosts.
                 */
                unsigned int interval;

                /**
                 * Registrar used to get all watched processes
                 */
                Server::Overseer::HostsRegistrar registrar;

                /**
                 * Logger instance
                 */
                Logger logger;

            public:
                /**
                 * Constructor with all needed dependencies injected.
                 * @param interval How often scan hosts.
                 * @param registrar Registrar instance.
                 * @param logger Logger instance.
                 */
                HostsSubmissionMonitor(unsigned int interval, Server::Overseer::HostsRegistrar registrar, Logger logger);

                /**
                 * Start scanning watched hosts.
                 */
                void start();
            };
        }

        /**
         * @class Application
         * Base Overseer server class used to listen for connections and processing them.
         */
        class Application {
            /**
             * Registrar used to watch hosts.
             */
            Server::Overseer::HostsRegistrar registrar;

            /**
             * Hasher used to confirm sent processes list.
             */
            Hasher hasher;

            /**
             * Monitor that's checking if every watched host submit processes list at expected time.
             */
            Server::Checker::HostsSubmissionMonitor monitor;

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
             * @param monitor Monitor to check if hosts submit their messages at expected time.
             * @param logger A logger instance.
             */
            Application(Server::Overseer::HostsRegistrar hostsRegistrar, Hasher hasher, Server::Checker::HostsSubmissionMonitor monitor, Logger logger);

            /**
             * Server application with given configuration. Monitor will be run in a child process;
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
