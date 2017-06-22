cmake_minimum_required(VERSION 3.5.2)
project(Projekt)

set(CMAKE_CXX_STANDARD 11)

# Build NetUtils library
set(PAP_UTILS_SRC src/Common/NetUtils.cpp src/Common/NetUtils.h)
add_library(pap_utils ${PAP_UTILS_SRC})

# Build Client app
set(CLIENT_SRC src/Client/main.cpp src/Client/ProcessList.cpp src/Client/ProcessList.h src/Common/ErrorHandling.cpp src/Common/ErrorHandling.h)
add_executable(Client ${CLIENT_SRC})
#target_link_libraries (Client pap_utils)