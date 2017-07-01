//
// Created by Mikołaj Żyłkowski on 01.07.2017.
//

#ifndef PROJEKT_COMMON_H
#define PROJEKT_COMMON_H


#include <arpa/inet.h>
#include <cstdarg>
#include <exception>
#include <functional>
#include <stdarg.h>
#include <string>
#include <string.h>
#include <list>

#define MAX_ERROR_LENGTH 512

namespace zylkowsk {
    /**
     *  @namespace Common
     *  Containing classes and constants related to communication between Client and Server.
     */
    namespace Common {
        const int SERVER_DEFAULT_PORT = 51435;

        const std::string CMD_REGISTER{"WATCH REGISTER"};
        const std::string CMD_SEND_PROCESSES{"WATCH APPLICATIONS"};

        const std::string RESPONSE_REGISTERED{"OK, HOST WATCHED"};
        const std::string RESPONSE_NO_CHANGES_IN_PROCESSES{"OK, NO CHANGES"};
        const std::string RESPONSE_SEND_PROCESSES{"OK, START LIST, END WITH EMPTY LINE"};
        const std::string RESPONSE_PROCESSES_SAVED{"OK, LIST WATCHED"};

        const std::string ERR_INVALID_INTERVAL{"FAIL, INVALID INTERVAL"};
        const std::string ERR_HOST_WATCHED{"FAIL, HOST ALREADY WATCHED"};
        const std::string ERR_HOST_NOT_WATCHED{"FAIL, HOST NOT WATCHED"};
        const std::string ERR_INVALID_LIST{"FAIL, LIST INVALID"};
        const std::string ERR_UNKNOWN_CMD{"FAIL, UNKNOWN COMMAND"};

        const int ERR_CODE_INVALID_INTERVAL = 1;
        const int ERR_CODE_HOST_WATCHED = 2;
        const int ERR_CODE_HOST_NOT_WATCHED = 3;
        const int ERR_CODE_INVALID_LIST = 4;
        const int ERR_CODE_UNKNOWN_CMD = 9;

        /**
         *	@class Exception
         *	Exception class thrown in case of errors.
         */
        class Exception : public std::exception {
            /**
             *  Error code.
             */
            int errorCode;

            /**
             *	Error that happened.
             */
            char error[MAX_ERROR_LENGTH];

            /**
             *	Prepare error message.
             *
             *	@param format Format in printf style.
             *	@param args Arguments passed to the formatted string
             */
            void setErrorMessage(const char *format, va_list args);

        public:
            /**
             *	Constructor.
             *	Exception allows to set errors in same format as printf function.
             *
             *	@param code Number representing error code.
             *	@param format Format in printf style.
             */
            Exception(int code, std::string format, ...);

            /**
             *	Constructor.
             *	Exception allows to set errors in same format as printf function.
             *
             *	@param code Number representing error code.
             *	@param format Format in printf style.
             */
            Exception(int code, const char *format, ...);

            /**
             *	Constructor that defaults code to 99.
             *
             *	@param format Format in printf style.
             */
            Exception(const char *format, ...);

            /**
             * Get error code.
             *
             * @return Error code.
             */
            const int code();

            /**
             *	Overridden what method to get actual error message.
             */
            virtual const char *what() const throw();
        };

        /**
         * @class ProcessListHasher
         * A class used to get hash of the whole process list.
         */
        class Hasher {
            /**
             * Helper function to join all processes names into one string.
             *
             * @param processes List of processes.
             * @return Concatenated processes names.
             */
            std::string joinList(const std::list<std::string> &processes);

        public:
            /**
             * Calculate hash from passed processes list.
             *
             * @param processes List of processes.
             * @return Hash of the processes list.
             */
            std::string hashList(const std::list<std::string> &processes);
        };

        /**
         * @class Logger
         * Simple class to log message in either syslog or to standard output.
         */
        class Logger {
            /**
             * Should syslog be used
             */
            bool useSyslog;

        public:
            /**
             * Format given time with expected log format.
             *
             * @param timestamp Unix timestamp to format.
             * @return Formatted time.
             */
            static const std::string formatTime(time_t timestamp);

            /**
             * Constructor.
             *
             * @param name Name used for syslog.
             * @param useSyslog Should syslog be used.
             */
            Logger(std::string name, bool useSyslog);

            /**
             * Destructor
             */
            ~Logger();

            /**
             *	Log message with given level to syslog if turned on.
             *	@param level Level of the log, must be valid for syslog function.
             *	@param message Message to be logged.
             */
            void log(int level, std::string message);

            /**
             *	Log message with given level to syslog if turned on.
             *	@param level Level of the log, must be valid for syslog function.
             *	@param message Message to be logged.
             */
            void log(int level, const char *message);
        };

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
             *  Limit of the child processes used to handle incoming connections.
             */
            unsigned int maxChildProcesses;

            /**
             *  Current child processes.
             */
            unsigned int childProcessesCount;

            /**
             * Is this process a child one?
             */
            bool isParent;

            /**
             * Logger instance
             */
            Logger logger;

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
            void handleClientTCP(std::function<void(int, struct sockaddr_in)> func);

            /**
             *	Method to handle incoming connection with UDP.
             *	@throws zylkowsk::Common::ErrorHandling::Exception In case of error during opening or closing connection with the client.
             */
            void handleClientUDP(std::function<void(int, struct sockaddr_in)> func);

        public:
            /**
             * 	Constructor of the server. Note that server is not yet listening to the connections.
             *
             *	@see listen()
             *	@param serverName Name of the server.
             *	@param port Number of the port server will be listening on.
             *	@param type Type of the connection server will be responding to.
             *	@param childProcessesLimit Limit of the child processes used to handle incoming connections.
             *	@param daemon Should server be run as a daemon.
             *	@param logger A logger instance.
             *	@throws zylkowsk::Common::ErrorHandling::Exception If socket cannot be created or bound to given port.
             */
            Server(std::string serverName, int port, ConnectionType type, unsigned int childProcessesLimit, bool daemon, Logger logger);

            /**
             *	Destructor. Server closes connections on the socket that it was working on.
             */
            ~Server();

            /**
             *	Server use preforking to create child processes that are listening to the connections and process them with function passed as an argument.
             *	If any of child processes die, a new one is forked and starts to listen for connections.
             *
             *	@param func Function to properly process received connection. Function must take int as a parameter with socket descriptor and structure with sockaddr.
             *	@throws zylkowsk::Common::ErrorHandling::Exception If socket cannot be set to listening or during opening or closing connection with the client.
             */
            void startListening(std::function<void(int, struct sockaddr_in)> func);

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
            unsigned short getPort(const char *service);

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
            Client(const char *address, const char *service, ConnectionType type);

            /**
             *	Connect to the server and process it's answer with passed function.
             *	@param func Function to process response from the server.
             *	@throws Exception If socket cannot be created or connection cannot be established or closed.
             */
            void connectToServer(std::function<void(int, struct sockaddr_in)> func);
        };
    }
}



#endif //PROJEKT_COMMON_H
