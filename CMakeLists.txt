cmake_minimum_required(VERSION 3.5.2)
project(Projekt)

set(CMAKE_CXX_STANDARD 11)

include_directories(/usr/local/lib)

# Build Communicator app
set(CLIENT_SRC src/Client/main.cpp src/Client/ProcessList.cpp src/Client/ProcessList.h src/Common/ErrorHandling.cpp src/Common/ErrorHandling.h src/Common/Hasher.cpp src/Common/Hasher.h src/Client/WatcherConnection.cpp src/Client/WatcherConnection.h src/Common/Communication.h src/Common/NetUtils.cpp src/Common/NetUtils.h)
add_executable(Client ${CLIENT_SRC})
target_link_libraries (Client ssl)
target_link_libraries (Client crypto)