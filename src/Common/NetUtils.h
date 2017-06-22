//
// Created by Mikołaj Żyłkowski on 22.06.2017.
//

#ifndef PROJEKT_NET_UTILS_H
#define PROJEKT_NET_UTILS_H

#include <arpa/inet.h>
#include <exception>
#include <stdarg.h>
#include <string>
#include <string.h>

namespace zylkowsk {
    namespace Common {
        namespace NetUtils {
            /**
             *	Possible types of connections for both Server and Client.
             */
            enum ConnectionType {
                TCP = SOCK_STREAM,
                UDP = SOCK_DGRAM
            };

            /**
             *	@class Server
             *	A class to create a server listening on the given port.
             */
            class Server {
                /**
                 *	Instance of the currently running server.
                 */
                static Server *serverInstance;

                /**
                 *	Server name.
                 */
                std::string name;

                /**
                 *	Socket descriptor the server is working on.
                 */
                int socketDesc;

                /**
                 *  Type of the server.
                 */
                ConnectionType type;

                /**
                 *	Lock file descriptor.
                 */
                int lockFile;

                /**
                 *  Flag to see if server is run as a daemon.
                 */
                bool asDaemon;

                /**
                 *	Internal method to init server process.
                 *	@throws zylkowsk::Common::ErrorHandling::Exception If server cannot be created cause another server process is already running.
                 */
                void initProcess();

                /**
                 *	Internal method to create a socket for given connection type.
                 *	@param type Connection type to use.
                 *	@throws zylkowsk::Common::ErrorHandling::Exception If socket cannot be created.
                 */
                void createSocket(ConnectionType type);

                /**
                 *	Internal method to bind Server's socket to desired port.
                 *	@param port Port on which server will listen.
                 *	@throws zylkowsk::Common::ErrorHandling::Exception If socket cannot be bound to given port.
                 */
                void bindToPort(int port);

                /**
                 *	Internal method to run the server as a daemon.
                 *	@throws zylkowsk::Common::ErrorHandling::Exception If server cannot be daemonized.
                 */
                void daemonize();

                /**
                 *	Method to handle incoming connection with TCP.
                 *	@throws zylkowsk::Common::ErrorHandling::Exception In case of error during opening or closing connection with the client.
                 */
                void handleClientTCP(void (*func)(int, struct sockaddr_in));

                /**
                 *	Method to handle incoming connection with UDP.
                 *	@throws zylkowsk::Common::ErrorHandling::Exception In case of error during opening or closing connection with the client.
                 */
                void handleClientUDP(void (*func)(int, struct sockaddr_in));

                /**
                 *	Log message with given level to syslog if turned on.
                 *	@param level Level of the log, must be valid for syslog function.
                 *	@param message Message to be logged.
                 */
                void logMessage(int level, const char *message);

            public:
                /**
                 * 	Constructor of the server. Note that server is not yet listening to the connections.
                 *	@see listen()
                 *	@param serverName Name of the server.
                 *	@param port Number of the port server will be listening on.
                 *	@param type Type of the connection server will be responding to.
                 *	@param useSyslog Should server be run as a daemon.
                 *	@throws zylkowsk::Common::ErrorHandling::Exception If socket cannot be created or bound to given port.
                 */
                Server(std::string serverName, int port, ConnectionType type, bool asDaemon);

                /**
                 *	Destructor. Server closes connections on the socket that it was working on.
                 */
                ~Server();

                /**
                 *	Start listening to the connections and process them with function passed as an argument.
                 *	@param func Function to properly process received connection. Function must take int as a parameter with socket descriptor.
                 *	@throws zylkowsk::Common::ErrorHandling::Exception If socket cannot be set to listening or during opening or closing connection with the client.
                 */
                void startListening(void (*func)(int, struct sockaddr_in));

                /**
                 *	Handle received signals.
                 */
                static void handleSignal(int sig);
            };

            /**
             *	@class Client
             *	A class to create a client connecting with given server.
             */
            class Client {
                /**
                 *	Desired server address structure.
                 */
                struct sockaddr_in serverAddress;

                /**
                 *	Type of the connection to the server.
                 */
                ConnectionType type;

                /**
                 *	Set server address.
                 *	@param address String representing either host name or IP.
                 *	@param port Port on which we want to connect.
                 */
                void setServerAddress(const char *address, unsigned short port);

                /**
                 *	Get port for service name.
                 *	@param serviceName Name of the service.
                 *	@return Port number.
                 *	@throws zylkowsk::Common::ErrorHandling::Exception If service name cannot be resolved.
                 */
                unsigned short getPort(char *service);

                /**
                 *	Get binary representation of IP address from either host name or IP in decimal representation.
                 *	@param address String representing either host name or IP.
                 *	@return Binary representation of IP address.
                 *	@throws zylkowsk::Common::ErrorHandling::Exception If IP cannot be resolved.
                 */
                unsigned long getIp(const char *address);

            public:
                /**
                 *	Constructor.
                 *	Create a client for given address, service name and connection type.
                 *	@param address String representing either host name or IP.
                 *	@param service Port or name of the service on which we want to connect.
                 *	@param type Connection type to use.
                 */
                Client(const char *address, char *service, ConnectionType type);

                /**
                 *	Connect to the server and process it's answer with passed function.
                 *	@param func Function to process response from the server.
                 *	@throws Exception If socket cannot be created or connection cannot be established or closed.
                 */
                void connectToServer(void (*func)(int, struct sockaddr_in));
            };
        };
    };
};

#endif //PROJEKT_ERROR_HANDLING_H