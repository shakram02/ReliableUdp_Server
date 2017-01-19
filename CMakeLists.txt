cmake_minimum_required(VERSION 3.6)
project(enhancedUdpServer)

set(CMAKE_CXX_STANDARD 11)

#target_compile_options(-lpthread)
find_package(Threads)
find_package(Boost COMPONENTS system filesystem REQUIRED)

# NOTE change to the directory that matches the file directory
set(Udp_Lib_Directory ../../clg_ReliableUdp_Lib)
set(PROJECT_LINK_LIBS libUdpSocketLib.so)

link_directories(${Udp_Lib_Directory}/cmake-build-debug)

#set(SOURCE_FILES main.cpp WelcomingSocket.cpp WelcomingSocket.h timecalc.h WorkerClasses/WorkerSocket.cpp WorkerClasses/WorkerSocket.h ClientMediator.cpp ClientMediator.h WorkerClasses/WorkerFacade.cpp WorkerClasses/WorkerFacade.h WorkerClasses/FileFragmenter.cpp WorkerClasses/FileFragmenter.h PathLossProbability.h)
file(GLOB SOURCE_FILES "*.cpp" "WorkerClasses/*.cpp")

include_directories(${Udp_Lib_Directory}/include)
add_executable(enhancedUdpServer ${SOURCE_FILES} server_config.h WorkerClasses/GbnSender.cpp WorkerClasses/GbnSender.h AbstractSender.h AbstractSender.cpp)

target_link_libraries(enhancedUdpServer ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(enhancedUdpServer ${PROJECT_LINK_LIBS} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})