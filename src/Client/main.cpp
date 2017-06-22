//
// Created by Mikołaj Żyłkowski on 22.06.2017.
//

#include "../Common/Hasher.h"
#include "ProcessList.h"
#include <iostream>

using namespace zylkowsk::Client::ProcessList;
using namespace zylkowsk::Common::Hasher;

int main() {
    ProcessesReader processesReader;
    auto processes = processesReader.getRunningProcesses();

    for (auto process : processes) {
        std::cout << process << std::endl;
    }
    ProcessListHasher hasher;
    std::cout << "Hash:";
    std::cout << hasher.hashList(processes);
    std::cout << std::endl;

    return 0;
}