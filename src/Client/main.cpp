//
// Created by Mikołaj Żyłkowski on 22.06.2017.
//

#include "ProcessList.h"
#include <iostream>

using namespace zylkowsk::Client::ProcessList;

int main() {
    ProcessesReader processesReader;
    auto processes = processesReader.getRunningProcesses();

    for (auto process : processes) {
        std::cout << process << std::endl;
    }

    return 0;
}