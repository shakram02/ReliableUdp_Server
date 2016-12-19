cmake_minimum_required(VERSION 3.6)
project(enhancedUdpServer)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp SocketServer.cpp SocketServer.h timecalc.h timecalc.cpp ../libs/netutils.c ../libs/netutils.h)
add_executable(enhancedUdpServer ${SOURCE_FILES})