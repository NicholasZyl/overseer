cmake_minimum_required(VERSION 3.5.2)
project(Projekt)

set(CMAKE_CXX_STANDARD 11)

include_directories(/usr/local/lib)
include_directories(/usr/local/include)

# Build Client app
set(CLIENT_SRC src/Client/main.cpp src/Client/ProcessList.cpp src/Common/ErrorHandling.cpp src/Common/Hasher.cpp src/Client/WatcherConnection.cpp src/Common/NetUtils.cpp src/Common/Logger.cpp src/Client/Application.cpp src/Client/Client.h src/Common/Common.h)
add_executable(Client ${CLIENT_SRC})
target_link_libraries (Client ssl)
target_link_libraries (Client crypto)

# Build Server app
set(SERVER_SRC src/Server/main.cpp src/Server/Overseer.cpp src/Common/ErrorHandling.cpp src/Common/Hasher.cpp src/Client/WatcherConnection.cpp src/Common/NetUtils.cpp src/Server/Application.cpp src/Common/Logger.cpp src/Server/Checker.cpp src/Server/Server.h src/Common/Common.h)
add_executable(Server ${SERVER_SRC})
target_link_libraries (Server ssl)
target_link_libraries (Server crypto)
target_link_libraries (Server yaml-cpp)
