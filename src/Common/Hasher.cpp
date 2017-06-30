//
// Created by Mikołaj Żyłkowski on 22.06.2017.
//

#include "Hasher.h"
#include <openssl/sha.h>
#include <sstream>
#include <iostream>

using namespace zylkowsk::Common;

std::string Hasher::joinList(const std::list<std::string> &processes) {
    std::stringstream joinedList;
    for (auto process : processes) {
        joinedList << process;
    }

    return joinedList.str();
}

std::string Hasher::hashList(const std::list<std::string> &processes) {
    auto joinedList = joinList(processes);
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)joinedList.c_str(), joinedList.length(), hash);
    char hashString[SHA_DIGEST_LENGTH*2];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(&hashString[i * 2], "%02x", hash[i]);
    }

    return std::string(hashString);
}
