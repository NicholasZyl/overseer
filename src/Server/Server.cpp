//
// Created by Mikołaj Żyłkowski on 29.06.2017.
//

#include "../Common/Communication.h"
#include "../Common/ErrorHandling.h"
#include "../Common/NetUtils.h"
#include "Server.h"
#include <functional>
#include <sstream>
#include <unistd.h>

#define MAX_BUFFER 1024

using namespace std::placeholders;
using namespace zylkowsk::Common::ErrorHandling;
using namespace zylkowsk::Common::Communication;
using namespace zylkowsk::Common;
using namespace zylkowsk::Server;

Server::Server(HostsRegistrar hostsRegistrar, Hasher::ProcessListHasher processListHasher)
        : registrar(hostsRegistrar), hasher(processListHasher) {}

void Server::run() {
    NetUtils::Server baseServer = NetUtils::Server("Overseer", 51435, NetUtils::TCP, 10, false);
    auto connectionProcessing = std::bind(&Server::processIncomingConnection, this, _1, _2);
    baseServer.startListening(connectionProcessing);
}

void Server::processIncomingConnection(int incomingSocket, struct sockaddr_in incomingAddress) {
    char commandBuffer[MAX_BUFFER];
    ssize_t readLen;
    while (0 < (readLen = read(incomingSocket, commandBuffer, MAX_BUFFER))) {
        commandBuffer[readLen] = 0;
    }

    std::istringstream commandStream(commandBuffer);
    std::string command, commandSuffix, argument;
    commandStream >> command >> commandSuffix >> argument;
    command.append(commandSuffix);

    try {
        if (0 == CMD_REGISTER.compare(command)) {
            processHostRegistration(incomingSocket, std::string(inet_ntoa(incomingAddress.sin_addr)), argument);
        } else if (0 == CMD_SEND_PROCESSES.compare(command)) {
            processHostProcessesMessage(incomingSocket, std::string(inet_ntoa(incomingAddress.sin_addr)), argument);
        } else {
            throw Exception(ERR_CODE_UNKNOWN_CMD, ERR_UNKNOWN_CMD);
        }
    } catch (Exception &e) {
        write(incomingSocket, e.what(), strlen(e.what()));
    }
}

void Server::processHostRegistration(int incomingSocket, std::string hostIp, std::string commandArgument) {
    unsigned int interval;
    if (0 != (interval = (unsigned int) atoi(commandArgument.c_str()))) {
        throw Exception(ERR_CODE_INVALID_INTERVAL, ERR_INVALID_INTERVAL);
    }

    registrar.registerHost(hostIp, interval);

    write(incomingSocket, RESPONSE_REGISTERED.c_str(), strlen(RESPONSE_REGISTERED.c_str()));
}

void Server::processHostProcessesMessage(int incomingSocket, std::string hostIp, std::string commandArgument) {
    if (registrar.processMessageFromHost(hostIp, commandArgument)) {
        write(incomingSocket, RESPONSE_NO_CHANGES_IN_PROCESSES.c_str(), strlen(RESPONSE_NO_CHANGES_IN_PROCESSES.c_str()));
    } else {
        write(incomingSocket, RESPONSE_SEND_PROCESSES.c_str(), strlen(RESPONSE_SEND_PROCESSES.c_str()));
        char commandBuffer[MAX_BUFFER];
        ssize_t readLen;
        while (0 < (readLen = read(incomingSocket, commandBuffer, MAX_BUFFER))) {
            commandBuffer[readLen] = 0;
            fprintf(stdout, "Line read %s\n", commandBuffer);
        }
    }
}
