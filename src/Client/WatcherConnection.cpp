//
// Created by Mikołaj Żyłkowski on 26.06.2017.
//

#include "../Common/Communication.h"
#include "../Common/ErrorHandling.h"
#include "WatcherConnection.h"
#include <unistd.h>

#define MAX_BUFFER 2048

using namespace zylkowsk::Client::WatcherConnection;
using namespace zylkowsk::Common::Communication;
using namespace zylkowsk::Common::ErrorHandling;
using namespace zylkowsk::Common::NetUtils;

Communicator::Communicator(class Client client, ProcessListHasher hasher) : baseClient(client), processListHasher(hasher) {}

void Communicator::registerClient(unsigned int interval) {
    baseClient.connectToServer([&interval](int socket, struct sockaddr_in serverAddress) {
        char cmd[MAX_BUFFER];
        sprintf(cmd, "%s %d", CMD_REGISTER.c_str(), interval);
        write(socket, cmd, strlen(cmd));
        fprintf(stdout, "%s\n", cmd);

        char responseBuffer[MAX_BUFFER];
        ssize_t readLen;
        while (0 < (readLen = read(socket, responseBuffer, MAX_BUFFER))) {
            responseBuffer[readLen] = 0;
        }
        auto response = std::string(responseBuffer);

        if (0 == response.compare(ERR_INVALID_INTERVAL)) {
            throw Exception(ERR_CODE_INVALID_INTERVAL, ERR_INVALID_INTERVAL);
        } else if (0 == response.compare(ERR_HOST_WATCHED)) {
            throw Exception(ERR_CODE_HOST_WATCHED, ERR_HOST_WATCHED);
        }
    });
}

void Communicator::sendProcessesList(std::list<std::string> processes) {
    auto hasher = processListHasher;
    baseClient.connectToServer([&processes, &hasher](int socket, struct sockaddr_in serverAddress)->void {
        char cmd[MAX_BUFFER];
        sprintf(cmd, "%s %s", CMD_SEND_PROCESSES.c_str(), hasher.hashList(processes).c_str());
        write(socket, cmd, strlen(cmd));

        char responseBuffer[MAX_BUFFER];
        ssize_t readLen;
        if (0 < (readLen = read(socket, responseBuffer, MAX_BUFFER))) {
            responseBuffer[readLen] = '\0';
        }
        auto response = std::string(responseBuffer);

        if (0 == response.compare(RESPONSE_SEND_PROCESSES)) {
            for (auto process : processes) {
                process.append("\n");
                write(socket, process.c_str(), strlen(process.c_str()));
            }
            write(socket, "\n", strlen("\n"));

            memset(responseBuffer, 0, sizeof(responseBuffer));
            if (0 < (readLen = read(socket, responseBuffer, MAX_BUFFER))) {
                responseBuffer[readLen] = '\0';
            }
            response = std::string(responseBuffer);

            if (0 == response.compare(ERR_INVALID_LIST)) {
                throw Exception(ERR_CODE_INVALID_LIST, ERR_INVALID_LIST);
            }
        } else if (0 == response.compare(ERR_HOST_NOT_WATCHED)) {
            throw Exception(ERR_CODE_HOST_NOT_WATCHED, ERR_HOST_NOT_WATCHED);
        }
    });
}
