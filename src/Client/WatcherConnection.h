//
// Created by Mikołaj Żyłkowski on 26.06.2017.
//

#ifndef PROJEKT_WATCHERCOMMUNICATOR_H
#define PROJEKT_WATCHERCOMMUNICATOR_H


#include "../Common/Hasher.h"
#include "../Common/NetUtils.h"
#include <list>
#include <string>

using namespace zylkowsk::Common;

namespace zylkowsk {
    namespace Client {
        namespace WatcherConnection {
            /**
             *  @class Communicator
             *  A class used to communicate between client and server.
             */
            class Communicator {
                /**
                 *  Base client used for connection.
                 */
                class Client baseClient;

                /**
                 *  Hasher used to prepare processes list hash.
                 */
                Hasher processListHasher;

            public:
                /**
                 *  Constructor of the communicator.
                 *
                 *  @param client Base client used for communicating.
                 *  @param hasher Hasher used for preparing a hash of processes list.
                 */
                Communicator(class Client client, Hasher hasher);

                /**
                 *  Send a command to register a new client on the server.
                 *
                 *  @param interval How often client should send processes list, in seconds.
                 *	@throws zylkowsk::Common::ErrorHandling::Exception If server responds with error.
                 */
                void registerClient(unsigned int interval);

                /**
                 *  Send a command to inform on running processes. First only hash is sent, the list is sent only if needed.
                 *
                 *  @param processes Processes to be sent.
                 *	@throws zylkowsk::Common::ErrorHandling::Exception If server responds with error.
                 */
                void sendProcessesList(std::list<std::string> processes);
            };
        }
    }
}


#endif //PROJEKT_WATCHERCOMMUNICATOR_H
