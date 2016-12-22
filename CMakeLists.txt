cmake_minimum_required(VERSION 3.6)
project(enhancedUdpServer)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp WelcomingSocket.cpp WelcomingSocket.h timecalc.h ../libs/netutils.c ../libs/netutils.h NetworkOperationResult.cpp UdpWorker.cpp UdpWorker.h ClientObserver.cpp ClientObserver.h)
add_executable(enhancedUdpServer ${SOURCE_FILES})