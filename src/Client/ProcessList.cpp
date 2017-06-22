//
// Created by Mikołaj Żyłkowski on 21.06.2017.
//

#include "../Common/ErrorHandling.h"
#include "ProcessList.h"
#include <errno.h>
#include <dirent.h>
#include <cstdlib>
#include <string.h>

#define MAX_PROCESS_NAME 256

using namespace zylkowsk::Client::ProcessList;
using namespace zylkowsk::Common::ErrorHandling;

const std::string ProcessesReader::procDir = "/proc";
const std::string ProcessesReader::procCmdFile = "comm";

std::list<std::string> ProcessesReader::getRunningProcesses() {
    DIR *procDir;
    struct dirent *processDir;
    if (!(procDir = opendir(ProcessesReader::procDir.c_str()))) {
        throw Exception("Unable to open /proc directory, with error: %s", strerror(errno));
    }
    std::list<std::string> processes;
    while ((processDir = readdir(procDir))) {
        int pid;
        if (0 != (pid = atoi(processDir->d_name))) {
            processes.push_back(getProcessName(pid));
        }
    }
    closedir(procDir);

    return processes;
}

std::string ProcessesReader::getProcessName(int pid) {
    char cmdFileName[FILENAME_MAX];
    sprintf(cmdFileName, "%s/%d/%s", ProcessesReader::procDir.c_str(), pid, ProcessesReader::procCmdFile.c_str());
    FILE *cmdFile = fopen(cmdFileName, "r");
    if (NULL == cmdFile) {
        throw Exception("Unable to open process comm file \"%s\", with error: %s", cmdFileName, strerror(errno));
    }
    char processName[MAX_PROCESS_NAME];
    fscanf(cmdFile, "%s", processName);
    fclose(cmdFile);

    return std::string(processName);
}
