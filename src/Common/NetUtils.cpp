#include "NetUtils.h"
#include "ErrorHandling.h"
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <unistd.h>
#include <sstream>

using namespace zylkowsk::Common;

#define MAX_BUFFER 1024
#define PID_BUFFER 10
#define LISTEN_QUEUE_BACKLOG 10

Server* Server::serverInstance = 0;

Server::Server(std::string serverName, int port, ConnectionType type, unsigned int childProcessesLimit, bool daemon, Logger logger) :
        name(serverName), type(type), maxChildProcesses(childProcessesLimit), childProcessesCount(0), isParent(true), asDaemon(daemon), logger(logger) {
    if (asDaemon) {
        daemonize();
    }
    initProcess();
    createSocket(type);
    bindToPort(port);
    logger.log(LOG_INFO, "Server started");
    serverInstance = this;
}

void Server::initProcess()
{
    char lockFileName[MAX_BUFFER];
    sprintf(lockFileName, "/var/spool/%s.lock", name.c_str());
    if ((lockFile = open(lockFileName, O_RDWR|O_CREAT, 0640)) < 0) {
        throw Exception("Unable to open the lock file with error: %s", strerror(errno));
    }
    char pid[PID_BUFFER];
    sprintf(pid, "%6d\n", getpid());
    write(lockFile, pid, strlen(pid));
    if (-1 == flock(lockFile, LOCK_EX|LOCK_NB)) {
        throw Exception("Unable to lock the lock file with error: %s", strerror(errno));
    }

    signal(SIGINT, Server::handleSignal);
    signal(SIGTERM, Server::handleSignal);
    signal(SIGCHLD, Server::handleSignal);
}

void Server::createSocket(ConnectionType type)
{
    socketDesc = socket(AF_INET, type, 0);
    if (-1 == socketDesc) {
        throw Exception("Unable to create a socket with error: %s", strerror(errno));
    }
    int optval = 1;
    if (-1 == setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval)) {
        throw Exception("Unable to set option on socket with error: %s", strerror(errno));
    }
}

void Server::bindToPort(int port)
{
    struct sockaddr_in address;

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if (-1 == bind(socketDesc, (struct sockaddr *) &address, sizeof(struct sockaddr))) {
        throw Exception("Unable to bind port %d with error: %s", port, strerror(errno));
    }
}

void Server::daemonize()
{
    if (-1 == daemon(0, 0)) {
        throw Exception("Unable to daemonize server with error: %s", strerror(errno));
    }
}

Server::~Server()
{
    if (-1 == close(socketDesc)) {
        char errorMsg[MAX_BUFFER];
        sprintf(errorMsg, "Unable to close the connection for socket %d with error: %s", socketDesc, strerror(errno));
        logger.log(LOG_CRIT, errorMsg);
    }

    if (isParent) {
        if (-1 == flock(lockFile, LOCK_UN)) {
            char errorMsg[MAX_BUFFER];
            sprintf(errorMsg, "Unable to unlock the lock file with error: %s", strerror(errno));
            logger.log(LOG_CRIT, errorMsg);
        }

        char lockFileName[MAX_BUFFER];
        sprintf(lockFileName, "/var/spool/%s.lock", name.c_str());
        if (-1 == unlink(lockFileName)) {
            char errorMsg[MAX_BUFFER];
            sprintf(errorMsg, "Unable to remove the lock file with error: %s", strerror(errno));
            logger.log(LOG_CRIT, errorMsg);
        }

        if (-1 == close(lockFile)) {
            char errorMsg[MAX_BUFFER];
            sprintf(errorMsg, "Unable to close the lock file descriptor with error: %s", strerror(errno));
            logger.log(LOG_CRIT, errorMsg);
        }
    }

    char closeServerMsg[MAX_BUFFER];
    sprintf(closeServerMsg, "Server process #%d closed", getpid());
    logger.log(LOG_INFO, closeServerMsg);
}

void Server::startListening(std::function<void(int, struct sockaddr_in)> func)
{
    if (TCP == type) {
        if (-1 == listen(socketDesc, LISTEN_QUEUE_BACKLOG)) {
            throw Exception("Unable to listen on socket %d with backlog %d with error: %s", socketDesc, LISTEN_QUEUE_BACKLOG, strerror(errno));
        }
        logger.log(LOG_INFO, "Server started listening");
    }

    pid_t childPid;
    while (true) {
        if (childProcessesCount < maxChildProcesses) {
            childPid = fork();
            if (0 == childPid) {
                isParent = false;
                while (true) {
                    if (TCP == type) {
                        handleClientTCP(func);
                    } else if (UDP == type) {
                        handleClientUDP(func);
                    }
                }
            } else if (0 < childPid) {
                ++childProcessesCount;
            } else {
                char errorMsg[MAX_BUFFER];
                sprintf(errorMsg, "Unable to create child process with error: %s", strerror(errno));
                logger.log(LOG_ERR, errorMsg);
            }
        } else {
            sleep(1);
        }
    }
}

void Server::handleClientTCP(std::function<void(int, struct sockaddr_in)> func)
{
    int incomingSocket;
    struct sockaddr_in incomingAddress;
    socklen_t incomingAddressLength = sizeof(incomingAddress);

    incomingSocket = accept(socketDesc, (struct sockaddr *) &incomingAddress, &incomingAddressLength);
    if (-1 == incomingSocket) {
        char errorMsg[MAX_BUFFER];
        sprintf(errorMsg, "Unable to accept connection from client on %s:%d with error: %s", inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port), strerror(errno));
        logger.log(LOG_CRIT, errorMsg);
        return;
    }
    char msg[MAX_BUFFER];
    sprintf(msg, "Server received a connection from client on %s:%d", inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port));
    logger.log(LOG_NOTICE, msg);

    func(incomingSocket, incomingAddress);

    sprintf(msg, "Server served response for client on %s:%d", inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port));
    logger.log(LOG_NOTICE, msg);

    if (-1 == close(incomingSocket)) {
        char errorMsg[MAX_BUFFER];
        sprintf(errorMsg, "Unable to close connection with client on %s:%d with error: with error: %s", inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port), strerror(errno));
        logger.log(LOG_CRIT, errorMsg);
    }

    sprintf(msg, "Server closed connection with client on %s:%d", inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port));
    logger.log(LOG_NOTICE, msg);
}

void Server::handleClientUDP(std::function<void(int, struct sockaddr_in)> func)
{
    char receivedDatagram[MAX_BUFFER];
    struct sockaddr_in incomingAddress;
    socklen_t incomingAddresLength = sizeof(incomingAddress);

    if (recvfrom(socketDesc, receivedDatagram, MAX_BUFFER, 0, (struct sockaddr *) & incomingAddress, &incomingAddresLength) < 0) {
        char errorMsg[MAX_BUFFER];
        sprintf(errorMsg, "Error while receiving datagram from %s:%d with error: %s", inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port), strerror(errno));
        logger.log(LOG_CRIT, errorMsg);
        return;
    }

    char msg[MAX_BUFFER];
    sprintf(msg, "Server received a datagram \"%s\" from client on %s:%d", receivedDatagram, inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port));
    logger.log(LOG_NOTICE, msg);

    func(socketDesc, incomingAddress);

    sprintf(msg, "Server served response for client on %s:%d", inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port));
    logger.log(LOG_NOTICE, msg);
}

void Server::handleSignal(int sig)
{
    switch (sig) {
        case SIGINT:
        case SIGTERM:
            char msg[MAX_BUFFER];
            sprintf(msg, "Server received signal %d", sig);
            serverInstance->logger.log(LOG_NOTICE, msg);
            serverInstance->~Server();
            exit(EXIT_SUCCESS);
        case SIGCHLD:
            while (0 < waitpid(-1, NULL, WNOHANG)) {
                --serverInstance->childProcessesCount;
            }
            break;
        default:
            return;
    }
}

Client::Client(const char *address, const char *service, ConnectionType type)
{
    this->type = type;
    unsigned short port;
    if (0 == (port = (unsigned short) atoi(service))) {
        port = getPort(service);
    }
    setServerAddress(address, port);
}

unsigned short Client::getPort(const char *serviceName)
{
    char proto[3];
    if (TCP == type) {
        sprintf(proto, "tcp");
    } else if (UDP == type) {
        sprintf(proto, "udp");
    } else {
        throw Exception("Unknown connection type.");
    }

    struct servent *serviceInfo;
    if (NULL == (serviceInfo = getservbyname(serviceName, proto))) {
        throw Exception("Cannot resolve service name %s", serviceName);
    }

    return ntohs(serviceInfo->s_port);
}

void Client::setServerAddress(const char *address, unsigned short port)
{
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = (in_addr_t) getIp(address);
}

unsigned long Client::getIp(const char *address)
{
    unsigned long ip = inet_addr(address);
    if (INADDR_NONE == ip) {
        struct hostent *hostInfo;
        if (NULL == (hostInfo = gethostbyname(address))) {
            throw Exception("Cannot resolve domain name %s", address);
        }

        ip = *((unsigned long *) hostInfo->h_addr_list[0]);
    }

    return ip;
}

void Client::connectToServer(std::function<void(int, struct sockaddr_in)> func)
{
    int connectionSocket = socket(AF_INET, type, 0);
    if (-1 == connectionSocket) {
        throw Exception("Unable to create a socket with error: %s", strerror(errno));
    }

    if (TCP == type) {
        if (-1 == connect(connectionSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress))) {
            throw Exception("Unable to connect with server %s:%d, with error: %s", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port), strerror(errno));
        }
    }

    func(connectionSocket, serverAddress);

    if (TCP == type) {
        if (-1 == close(connectionSocket)) {
            throw Exception("Unable to close connection with server %s:%d, with error: %s", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port), strerror(errno));
        }
    }
}
