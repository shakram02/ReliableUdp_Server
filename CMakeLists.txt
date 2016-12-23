cmake_minimum_required(VERSION 3.6)
project(enhancedUdpServer)

set(CMAKE_CXX_STANDARD 11)

#target_compile_options(-lpthread)
find_package(Threads)

set(SOURCE_FILES main.cpp WelcomingSocket.cpp WelcomingSocket.h timecalc.h ../libs/netutils.c ../libs/netutils.h NetworkOperationResult.cpp WorkerSocket.cpp WorkerSocket.h ClientObserver.cpp ClientObserver.h WorkerFacade.cpp WorkerFacade.h FileFragmenter.cpp FileFragmenter.h)
add_executable(enhancedUdpServer ${SOURCE_FILES})

target_link_libraries(enhancedUdpServer ${CMAKE_THREAD_LIBS_INIT})