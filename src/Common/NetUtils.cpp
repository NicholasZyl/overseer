#include "NetUtils.h"
#include "ErrorHandling.h"
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/file.h>
#include <string>
#include <unistd.h>

using namespace zylkowsk::Common::ErrorHandling;
using namespace zylkowsk::Common::NetUtils;

#define MAX_BUFFER 128
#define PID_BUFFER 10
#define LISTEN_QUEUE_BACKLOG 10

Server* Server::serverInstance = 0;

Server::Server(std::string serverName, int port, ConnectionType type, bool asDaemon)
{
    name = serverName;
    this->type = type;
    if (asDaemon) {
        daemonize();
    }
    initProcess();
    createSocket(type);
    bindToPort(port);
    logMessage(LOG_INFO, "Server started");
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
}

void Server::createSocket(ConnectionType type)
{
    socketDesc = socket(AF_INET, type, 0);
    if (-1 == socketDesc) {
        throw Exception("Unable to create a socket with error: %s", strerror(errno));
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
    openlog(name.c_str(), LOG_PID, LOG_USER);
}

Server::~Server()
{
    if (-1 == close(socketDesc)) {
        char errorMsg[MAX_BUFFER];
        sprintf(errorMsg, "Unable to close the connection for socket %d with error: %s", socketDesc, strerror(errno));
        logMessage(LOG_CRIT, errorMsg);
    }

    if (-1 == flock(lockFile, LOCK_UN)) {
        char errorMsg[MAX_BUFFER];
        sprintf(errorMsg, "Unable to unlock the lock file with error: %s", strerror(errno));
        logMessage(LOG_CRIT, errorMsg);
    }

    char lockFileName[MAX_BUFFER];
    sprintf(lockFileName, "/var/spool/%s.lock", name.c_str());
    if (-1 == unlink(lockFileName)) {
        char errorMsg[MAX_BUFFER];
        sprintf(errorMsg, "Unable to remove the lock file with error: %s", strerror(errno));
        logMessage(LOG_CRIT, errorMsg);
    }

    if (-1 == close(lockFile)) {
        char errorMsg[MAX_BUFFER];
        sprintf(errorMsg, "Unable to close the lock file descriptor with error: %s", strerror(errno));
        logMessage(LOG_CRIT, errorMsg);
    }

    logMessage(LOG_INFO, "Server closed");
    if (asDaemon) {
        closelog();
    }
}

void Server::startListening(void (*func)(int, struct sockaddr_in))
{
    if (TCP == type) {
        if (-1 == listen(socketDesc, LISTEN_QUEUE_BACKLOG)) {
            throw Exception("Unable to listen on socket %d with backlog %d with error: %s", socketDesc, LISTEN_QUEUE_BACKLOG, strerror(errno));
        }
        logMessage(LOG_INFO, "Server started listening");
    }

    while (true) {
        if (TCP == type) {
            handleClientTCP(func);
        } else if (UDP == type) {
            handleClientUDP(func);
        }
    }
}

void Server::handleClientTCP(void (*func)(int, struct sockaddr_in))
{
    int incomingSocket;
    struct sockaddr_in incomingAddress;
    socklen_t incomingAddresLength = sizeof(incomingAddress);

    incomingSocket = accept(socketDesc, (struct sockaddr *) &incomingAddress, &incomingAddresLength);
    if (-1 == incomingSocket) {
        char errorMsg[MAX_BUFFER];
        sprintf(errorMsg, "Unable to accept connection from client on %s:%d with error: %s", inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port), strerror(errno));
        logMessage(LOG_CRIT, errorMsg);
    }
    char msg[MAX_BUFFER];
    sprintf(msg, "Server received a connection from client on %s:%d", inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port));
    logMessage(LOG_NOTICE, msg);

    func(incomingSocket, incomingAddress);

    sprintf(msg, "Server served response for client on %s:%d", inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port));
    logMessage(LOG_NOTICE, msg);

    if (-1 == close(incomingSocket)) {
        char errorMsg[MAX_BUFFER];
        sprintf(errorMsg, "Unable to close connection with client on %s:%d with error: with error: %s", inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port), strerror(errno));
        logMessage(LOG_CRIT, errorMsg);
    }
    sprintf(msg, "Server closed connection with client on %s:%d", inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port));
    logMessage(LOG_NOTICE, msg);
}

void Server::handleClientUDP(void (*func)(int, struct sockaddr_in))
{
    char receivedDatagram[MAX_BUFFER];
    struct sockaddr_in incomingAddress;
    socklen_t incomingAddresLength = sizeof(incomingAddress);

    if (recvfrom(socketDesc, receivedDatagram, MAX_BUFFER, 0, (struct sockaddr *) & incomingAddress, &incomingAddresLength) < 0) {
        char errorMsg[MAX_BUFFER];
        sprintf(errorMsg, "Error while receiving datagram from %s:%d with error: %s", inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port), strerror(errno));
        logMessage(LOG_CRIT, errorMsg);
    }

    char msg[MAX_BUFFER];
    sprintf(msg, "Server received a datagram \"%s\" from client on %s:%d", receivedDatagram, inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port));
    logMessage(LOG_NOTICE, msg);

    func(socketDesc, incomingAddress);

    sprintf(msg, "Server served response for client on %s:%d", inet_ntoa(incomingAddress.sin_addr), ntohs(incomingAddress.sin_port));
    logMessage(LOG_NOTICE, msg);
}

void Server::handleSignal(int sig)
{
    char msg[MAX_BUFFER];
    sprintf(msg, "Server received signal %d", sig);
    serverInstance->logMessage(LOG_NOTICE, msg);
    serverInstance->~Server();
    exit(EXIT_SUCCESS);
}

void Server::logMessage(int level, const char *message)
{
    if (asDaemon) {
        syslog(level, "%s", message);
    } else if (level <= LOG_ERR) {
        fprintf(stderr, "%s\n", message);
    } else {
        fprintf(stdout, "%s\n", message);
    }
}

Client::Client(const char *address, char *service, ConnectionType type)
{
    this->type = type;
    unsigned short port;
    if (0 == (port = (unsigned short) atoi(service))) {
        port = getPort(service);
    }
    setServerAddress(address, port);
}

unsigned short Client::getPort(char *serviceName)
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

void Client::connectToServer(void (*func)(int, struct sockaddr_in))
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
