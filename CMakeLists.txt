cmake_minimum_required(VERSION 3.6)
project(enhancedUdpServer)

set(CMAKE_CXX_STANDARD 11)

#target_compile_options(-lpthread)
find_package(Threads)

set(PROJECT_LINK_LIBS libUdpSocketLib.so)
link_directories(../../UdpSocketLib/cmake-build-debug)


#set(SOURCE_FILES main.cpp WelcomingSocket.cpp WelcomingSocket.h timecalc.h NetworkOperationResult.cpp WorkerClasses/WorkerSocket.cpp WorkerClasses/WorkerSocket.h ClientObserver.cpp ClientObserver.h WorkerClasses/WorkerFacade.cpp WorkerClasses/WorkerFacade.h WorkerClasses/FileFragmenter.cpp WorkerClasses/FileFragmenter.h PathLossProbability.h)
file(GLOB SOURCE_FILES "*.cpp" "WorkerClasses/*.cpp")

include_directories(../../UdpSocketLib/include)
add_executable(enhancedUdpServer ${SOURCE_FILES} WorkerClasses/GbnSender.cpp WorkerClasses/GbnSender.h globaldef.h)

target_link_libraries(enhancedUdpServer ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(enhancedUdpServer ${PROJECT_LINK_LIBS})