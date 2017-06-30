//
// Created by Mikołaj Żyłkowski on 22.06.2017.
//

#ifndef PROJEKT_HASHER_H
#define PROJEKT_HASHER_H


#include <list>
#include <string>

namespace zylkowsk {
    namespace Common {
        /**
         * @class ProcessListHasher
         * A class used to get hash of the whole process list.
         */
        class Hasher {
            /**
             * Helper function to join all processes names into one string.
             *
             * @param processes List of processes.
             * @return Concatenated processes names.
             */
            std::string joinList(const std::list<std::string> &processes);

        public:
            /**
             * Calculate hash from passed processes list.
             *
             * @param processes List of processes.
             * @return Hash of the processes list.
             */
            std::string hashList(const std::list<std::string> &processes);
        };
    }
}


#endif //PROJEKT_HASHER_H
