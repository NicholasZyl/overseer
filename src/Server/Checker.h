//
// Created by Mikołaj Żyłkowski on 01.07.2017.
//

#ifndef PROJEKT_CHECKER_H
#define PROJEKT_CHECKER_H


#include "../Common/Logger.h"
#include "Overseer.h"

using namespace zylkowsk::Common;
using namespace zylkowsk::Server::Overseer;

namespace zylkowsk {
    namespace Server {
        namespace Checker {
            /**
             * @class HostsSubmissionMonitor
             * Class that monitors if every watched host send processes within expected time.
             */
            class HostsSubmissionMonitor {
                /**
                 * How often monitor should scan watched hosts.
                 */
                unsigned int interval;

                /**
                 * Registrar used to get all watched processes
                 */
                HostsRegistrar registrar;

                /**
                 * Logger instance
                 */
                Logger logger;

            public:
                /**
                 * Constructor with all needed dependencies injected.
                 * @param interval How often scan hosts.
                 * @param registrar Registrar instance.
                 * @param logger Logger instance.
                 */
                HostsSubmissionMonitor(unsigned int interval, HostsRegistrar registrar, Logger logger);

                /**
                 * Start scanning watched hosts.
                 */
                void start();
            };
        }
    }
}


#endif //PROJEKT_CHECKER_H
