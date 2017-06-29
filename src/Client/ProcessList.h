//
// Created by Mikołaj Żyłkowski on 21.06.2017.
//

#ifndef PROJEKT_PROCESSLIST_H
#define PROJEKT_PROCESSLIST_H


#include <list>
#include <stdio.h>
#include <string>

namespace zylkowsk {
    namespace Client {
        namespace ProcessList {
            /**
             *  @class ProcessesReader
             *  A class used to read all running processes on the host.
             */
            class ProcessesReader {
                /**
                 *  Default directory to check for running processes.
                 */
                static const std::string procDir;

                /**
                 *  Default file containing process name.
                 */
                static const std::string procCmdFile;

                /**
                 *  Extract name for the process with given PID.
                 *
                 *  @param pid Process id.
                 *  @return Name of the process.
                 */
                std::string getProcessName(int pid);

            public:
                /**
                 *  Get list of all running processes names.
                 *
                 *  @return List of processes names.
                 */
                std::list<std::string> getRunningProcesses();
            };
        }
    }
}


#endif //PROJEKT_PROCESSLIST_H
