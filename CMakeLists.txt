cmake_minimum_required(VERSION 3.5.2)
project(Projekt)

set(CMAKE_CXX_STANDARD 11)

include_directories(/usr/local/lib)
include_directories(/usr/local/include)

# Build Client app
set(CLIENT_SRC src/Client/main.cpp src/Client/ProcessList.cpp src/Client/ProcessList.h src/Common/ErrorHandling.cpp src/Common/ErrorHandling.h src/Common/Hasher.cpp src/Common/Hasher.h src/Client/WatcherConnection.cpp src/Client/WatcherConnection.h src/Common/Communication.h src/Common/NetUtils.cpp src/Common/NetUtils.h src/Common/Logger.cpp src/Common/Logger.h)
add_executable(Client ${CLIENT_SRC})
target_link_libraries (Client ssl)
target_link_libraries (Client crypto)

# Build Server app
set(SERVER_SRC src/Server/main.cpp src/Server/Overseer.cpp src/Server/Overseer.h src/Common/ErrorHandling.cpp src/Common/ErrorHandling.h src/Common/Hasher.cpp src/Common/Hasher.h src/Client/WatcherConnection.cpp src/Client/WatcherConnection.h src/Common/Communication.h src/Common/NetUtils.cpp src/Common/NetUtils.h src/Server/Application.cpp src/Server/Application.h src/Common/Logger.cpp src/Common/Logger.h)
add_executable(Server ${SERVER_SRC})
target_link_libraries (Server ssl)
target_link_libraries (Server crypto)
target_link_libraries (Server yaml-cpp)
