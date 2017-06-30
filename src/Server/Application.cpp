//
// Created by Mikołaj Żyłkowski on 29.06.2017.
//

#include "../Common/Communication.h"
#include "../Common/ErrorHandling.h"
#include "../Common/NetUtils.h"
#include "Application.h"
#include <functional>
#include <sstream>
#include <syslog.h>
#include <unistd.h>

#define MAX_BUFFER 2048

using namespace std::placeholders;
using namespace zylkowsk::Common;
using namespace zylkowsk::Server;

Application::Application(HostsRegistrar hostsRegistrar, Hasher hasher, HostsSubmissionMonitor monitor, Logger logger)
        : registrar(hostsRegistrar), hasher(hasher), monitor(monitor), logger(logger) {}

void Application::run(int port, unsigned int  processesLimit, bool asDaemon) {
    pid_t child = fork();
    if (0 == child) {
        monitor.start();
    } else if (0 < child) {
        class Server server("Overseer", port, TCP, processesLimit, asDaemon, logger);
        auto connectionProcessing = std::bind(&Application::processIncomingConnection, this, _1, _2);
        server.startListening(connectionProcessing);
    } else {
        throw Exception("Unable to create child process for Checker module with error: %s", strerror(errno));
    }
}

void Application::processIncomingConnection(int incomingSocket, struct sockaddr_in incomingAddress) {
    char commandBuffer[MAX_BUFFER];
    ssize_t readLen;
    if (0 < (readLen = read(incomingSocket, commandBuffer, MAX_BUFFER))) {
        commandBuffer[readLen] = '\0';
    }

    std::istringstream commandStream(commandBuffer);
    std::string command, commandSuffix, argument;
    commandStream >> command >> commandSuffix >> argument;
    command.append(" ").append(commandSuffix);

    try {
        if (0 == CMD_REGISTER.compare(command)) {
            processHostRegistration(incomingSocket, std::string(inet_ntoa(incomingAddress.sin_addr)), argument);
        } else if (0 == CMD_SEND_PROCESSES.compare(command)) {
            processHostProcessesMessage(incomingSocket, std::string(inet_ntoa(incomingAddress.sin_addr)), argument);
        } else {
            throw Exception(ERR_CODE_UNKNOWN_CMD, ERR_UNKNOWN_CMD);
        }
    } catch (Exception &e) {
        std::stringstream logMessage;
        logMessage << "Error occurred: " << e.what();
        logger.log(LOG_NOTICE, logMessage.str());

        write(incomingSocket, e.what(), strlen(e.what()));
    }
}

void Application::processHostRegistration(int incomingSocket, std::string hostIp, std::string commandArgument) {
    unsigned int interval;
    if (0 == (interval = (unsigned int) atoi(commandArgument.c_str()))) {
        throw Exception(ERR_CODE_INVALID_INTERVAL, ERR_INVALID_INTERVAL);
    }
    registrar.registerHost(hostIp, interval);

    std::stringstream logMessage;
    logMessage << "Registered a new host: " << hostIp;
    logger.log(LOG_NOTICE, logMessage.str());

    write(incomingSocket, RESPONSE_REGISTERED.c_str(), strlen(RESPONSE_REGISTERED.c_str()));
}

void Application::processHostProcessesMessage(int incomingSocket, std::string hostIp, std::string commandArgument) {
    if (registrar.processMessageFromHost(hostIp, commandArgument)) {
        std::stringstream logMessage;
        logMessage << "Watched host " << hostIp << ": no changes in processes list";
        logger.log(LOG_NOTICE, logMessage.str());

        write(incomingSocket, RESPONSE_NO_CHANGES_IN_PROCESSES.c_str(), strlen(RESPONSE_NO_CHANGES_IN_PROCESSES.c_str()));
    } else {
        std::stringstream logMessage;
        logMessage << "Watched host " << hostIp << ": processes list changed";
        logger.log(LOG_NOTICE, logMessage.str());

        write(incomingSocket, RESPONSE_SEND_PROCESSES.c_str(), strlen(RESPONSE_SEND_PROCESSES.c_str()));
        char messageBuffer[MAX_BUFFER];
        ssize_t readLen;
        std::stringstream message("");
        do {
            readLen = read(incomingSocket, messageBuffer, MAX_BUFFER);
            message << messageBuffer;
        } while (0 < readLen && !('\n' == messageBuffer[readLen - 2] && '\n' == messageBuffer[readLen - 1]));

        std::string process;
        std::list<std::string> processes;
        while (std::getline(message, process, '\n')) {
            if (0 != process.compare("")) {
                processes.push_back(process);
            }
        }

        const std::string calculatedHash = hasher.hashList(processes);
        if (0 == commandArgument.compare(calculatedHash)) {
            registrar.storeHostChangedProcessesList(hostIp, calculatedHash, processes);

            std::stringstream logMessage;
            logMessage << "Watched host " << hostIp << ": changed list saved";
            logger.log(LOG_NOTICE, logMessage.str());
            write(incomingSocket, RESPONSE_PROCESSES_SAVED.c_str(), strlen(RESPONSE_PROCESSES_SAVED.c_str()));
        } else {
            std::stringstream logMessage;
            logMessage << "Watched host " << hostIp << ": malformed list received";
            logger.log(LOG_NOTICE, logMessage.str());

            write(incomingSocket, ERR_INVALID_LIST.c_str(), strlen(ERR_INVALID_LIST.c_str()));
        }
    }
}
