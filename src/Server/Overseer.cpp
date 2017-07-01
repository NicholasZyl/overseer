//
// Created by Mikołaj Żyłkowski on 27.06.2017.
//

#include "../Common/Common.h"
#include "Server.h"
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <yaml-cpp/yaml.h>

using namespace zylkowsk::Common;
using namespace zylkowsk::Server::Overseer;

const std::string HostsRegistrar::storageDir = "/var/overseer/";

WatchedHost::WatchedHost(const std::string &hostIp, unsigned int interval) : ip(hostIp),
                                                                             interval(interval),
                                                                             processesListHash(""),
                                                                             processesList({}) {
    nextMessageTime = time(nullptr);
}

WatchedHost::WatchedHost(const std::string &hostIp, unsigned int interval, const time_t nextMessageExpectedTime, const std::string &processesListHash, const std::list<std::string> &processes) : ip(hostIp), interval(interval), nextMessageTime(nextMessageExpectedTime), processesListHash(processesListHash), processesList(processesList) {}

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

HostsRegistrar::HostsRegistrar() {
    if (-1 == mkdir(storageDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
        if (EEXIST != errno) {
            throw Exception("Cannot create storage dir");
        }
    }
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
    storedHost << YAML::Value << YAML::Block << host.getProcessesList();
    storedHost << YAML::EndMap;

    std::ofstream hostFile(getHostStorageFileName(host.getIp()));
    hostFile << storedHost.c_str();
    hostFile.close();
}

WatchedHost HostsRegistrar::getWatchedHostData(const std::string &hostIp) {
    std::string hostFile = getHostStorageFileName(hostIp);
    try {
        YAML::Node storedHost = YAML::LoadFile(hostFile);

        return WatchedHost(hostIp, storedHost["interval"].as<unsigned>(),
                           storedHost["next_message_expected"].as<time_t>(),
                           storedHost["processes_hash"].as<std::string>(),
                           storedHost["processes"].as<std::list<std::string>>());
    } catch (YAML::BadFile) {
        throw Exception(ERR_CODE_UNKNOWN_CMD, ERR_UNKNOWN_CMD);
    }
}

void HostsRegistrar::registerHost(const std::string &hostIp, const unsigned int interval) {
    if (isHostRegistered(hostIp)) {
        throw Exception(ERR_CODE_HOST_WATCHED, ERR_HOST_WATCHED);
    }
    WatchedHost host(hostIp, interval);
    saveWatchedHostData(host);
}

bool HostsRegistrar::processMessageFromHost(const std::string &hostIp, const std::string &processesListHash) {
    if (!isHostRegistered(hostIp)) {
        throw Exception(ERR_CODE_HOST_NOT_WATCHED, ERR_HOST_NOT_WATCHED);
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

std::list<WatchedHost> HostsRegistrar::getWatchedHosts() {
    DIR *hostsStorage;
    struct dirent *hostStorageFile;
    if (!(hostsStorage = opendir(storageDir.c_str()))) {
        throw Exception("Unable to open %s directory, with error: %s", storageDir.c_str(), strerror(errno));
    }
    std::list<WatchedHost> hosts;
    while ((hostStorageFile = readdir(hostsStorage))) {
        std::string hostFileName = std::string(hostStorageFile->d_name);
        std::size_t ext = hostFileName.find(".yml");
        if (std::string::npos == ext) {
            continue;
        }
        std::string host = hostFileName.substr(0, ext);
        hosts.push_back(getWatchedHostData(host));
    }
    closedir(hostsStorage);

    return hosts;
}
