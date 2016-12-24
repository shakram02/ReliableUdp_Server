cmake_minimum_required(VERSION 3.6)
project(enhancedUdpServer)

set(CMAKE_CXX_STANDARD 11)

#target_compile_options(-lpthread)
find_package(Threads)

set(SOURCE_FILES main.cpp WelcomingSocket.cpp WelcomingSocket.h timecalc.h ../libs/netutils.c ../libs/netutils.h NetworkOperationResult.cpp WorkerClasses/WorkerSocket.cpp WorkerClasses/WorkerSocket.h ClientObserver.cpp ClientObserver.h WorkerClasses/WorkerFacade.cpp WorkerClasses/WorkerFacade.h WorkerClasses/FileFragmenter.cpp WorkerClasses/FileFragmenter.h PathLossProbability.h)
add_executable(enhancedUdpServer ${SOURCE_FILES})

target_link_libraries(enhancedUdpServer ${CMAKE_THREAD_LIBS_INIT})