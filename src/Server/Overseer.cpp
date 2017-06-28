//
// Created by Mikołaj Żyłkowski on 27.06.2017.
//

#include "../Common/Communication.h"
#include "../Common/ErrorHandling.h"
#include "Overseer.h"
#include <fstream>
#include <sstream>
#include <yaml-cpp/yaml.h>

using namespace zylkowsk::Common::Communication;
using namespace zylkowsk::Common::ErrorHandling;
using namespace zylkowsk::Server::Overseer;

const std::string HostsRegistrar::storageDir = "/var/overseer/";

WatchedHost::WatchedHost(const std::string &hostIp, unsigned int interval) : ip(hostIp),
                                                                             interval(interval),
                                                                             processesListHash(""),
                                                                             processesList({}) {
    nextMessageTime = time(nullptr);
}

WatchedHost::WatchedHost(const std::string &hostIp, unsigned int interval, const time_t nextMessageExpectedTime, const std::string &processesListHash, const std::list<std::string> &processes) : ip(hostIp), interval(interval), nextMessageTime(nextMessageExpectedTime) {
    watchProcesses(processesListHash, processesList);
}

void WatchedHost::watchProcesses(const std::string &hash, const std::list<std::string> &processes) {
    processesListHash = hash;
    processesList = processes;
    updateNextExpectedMessageTime();
}

void WatchedHost::updateNextExpectedMessageTime() {
    nextMessageTime = time(nullptr) + interval;
}

const std::string &WatchedHost::getIp() const {
    return ip;
}

unsigned int WatchedHost::getInterval() const {
    return interval;
}

time_t WatchedHost::getNextMessageTime() const {
    return nextMessageTime;
}

const std::string &WatchedHost::getProcessesListHash() const {
    return processesListHash;
}

const std::list<std::string, std::allocator<std::string>> &WatchedHost::getProcessesList() const {
    return processesList;
}

std::string HostsRegistrar::getHostStorageFileName(const std::string &hostIp) {
    std::stringstream hostFileName;
    hostFileName << storageDir << hostIp << ".yml";

    return hostFileName.str();
}

bool HostsRegistrar::isHostRegistered(std::string hostIp) {
    std::ifstream hostFile(getHostStorageFileName(hostIp).c_str());

    return hostFile.good();
}

void HostsRegistrar::saveWatchedHostData(WatchedHost &host) {
    YAML::Emitter storedHost;
    storedHost << YAML::BeginMap;
    storedHost << YAML::Key << "interval";
    storedHost << YAML::Value << host.getInterval();
    storedHost << YAML::Key << "next_message_expected";
    storedHost << YAML::Value << host.getNextMessageTime();
    storedHost << YAML::Key << "processes_hash";
    storedHost << YAML::Value << host.getProcessesListHash();
    storedHost << YAML::Key << "processes";
    storedHost << YAML::Block << host.getProcessesList();
    storedHost << YAML::EndMap;

    std::ofstream hostFile(getHostStorageFileName(host.getIp()));
    hostFile << storedHost.c_str();
    hostFile.close();
}

WatchedHost HostsRegistrar::getWatchedHostData(const std::string &hostIp) {
    std::string hostFile = getHostStorageFileName(hostIp);
    try {
        YAML::Node storedHost = YAML::LoadFile(hostFile);

        return WatchedHost(hostIp, storedHost["interval"].as<unsigned>(), storedHost["next_message_expected"].as<time_t>(), storedHost["processes_hash"].as<std::string>(), storedHost["processes"].as<std::list<std::string>>());
    } catch (YAML::BadFile) {
        throw Exception(ERR_CODE_UNKNOWN_CMD, ERR_UNKNOWN_CMD.c_str());
    }
}

void HostsRegistrar::registerHost(const std::string &hostIp, const unsigned int interval) {
    if (isHostRegistered(hostIp)) {
        throw Exception(ERR_CODE_HOST_WATCHED, ERR_HOST_WATCHED.c_str());
    }
    WatchedHost host(hostIp, interval);
    saveWatchedHostData(host);
}

bool HostsRegistrar::processMessageFromHost(const std::string &hostIp, const std::string &processesListHash) {
    if (!isHostRegistered(hostIp)) {
        throw Exception(ERR_CODE_HOST_NOT_WATCHED, ERR_HOST_NOT_WATCHED.c_str());
    }
    WatchedHost host = getWatchedHostData(hostIp);

    if (0 == host.getProcessesListHash().compare(processesListHash)) {
        host.updateNextExpectedMessageTime();
        saveWatchedHostData(host);

        return true;
    } else {
        return false;
    }
}

void HostsRegistrar::storeHostChangedProcessesList(const std::string &hostIp, const std::string &processesListHash,
                                                   const std::list<std::string> &processes) {
    WatchedHost host = getWatchedHostData(hostIp);
    host.watchProcesses(processesListHash, processes);
    saveWatchedHostData(host);
}
