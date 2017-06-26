//
// Created by Mikołaj Żyłkowski on 26.06.2017.
//

#ifndef PROJEKT_COMMUNICATION_H
#define PROJEKT_COMMUNICATION_H

#include <string>

namespace zylkowsk {
    namespace Common {
        /**
         *  @namespace Communication
         *  Containing constants related to communication between Client and Server.
         */
        namespace Communication {
            const std::string CMD_REGISTER{"WATCH REGISTER"};
            const std::string CMD_SEND_PROCESSES{"WATCH APPLICATIONS"};

            const std::string RESPONSE_REGISTERED{"OK, HOST WATCHED"};
            const std::string RESPONSE_NO_CHANGES_IN_PROCESSES{"OK, NO CHANGES"};
            const std::string RESPONSE_SEND_PROCESSES{"OK, START LIST, END WITH EMPTY LINE"};
            const std::string RESPONSE_PROCESSES_SAVED{"OK, LIST WATCHED"};

            const std::string ERR_INVALID_INTERVAL{"FAIL, INVALID INTERVAL"};
            const std::string ERR_HOST_WATCHED{"FAIL, HOST ALREADY WATCHED"};
            const std::string ERR_HOST_NOT_WATCHED{"FAIL, HOST NOT WATCHED"};
            const std::string ERR_INVALID_LIST{"FAIL, LIST INVALID"};
            const std::string ERR_UNKNOWN_CMD{"FAIL, UNKNOWN COMMAND"};

            const int ERR_CODE_INVALID_INTERVAL = 1;
            const int ERR_CODE_HOST_WATCHED = 2;
            const int ERR_CODE_HOST_NOT_WATCHED = 3;
            const int ERR_CODE_INVALID_LIST = 4;
            const int ERR_CODE_UNKNOWN_CMD = 9;
        }
    }
}


#endif //PROJEKT_COMMUNICATION_H
