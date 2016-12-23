//
// Created by ahmed on 12/23/16.
//

#ifndef ENHANCEDUDPSERVER_WORKER_H
#define ENHANCEDUDPSERVER_WORKER_H

#include "WorkerSocket.h"
#include "FileFragmenter.h"

typedef int sock_descriptor;

/**
 * Provides an interface to a subsystem that handles file fragmentation
 * and socket delivery using GBN protocol
 */
class WorkerFacade
{
public:
    WorkerFacade(sock_descriptor sockfd, string file_name);

    void StartWorking();

    void StopWorking();

    ~WorkerFacade();
private:
    WorkerSocket socket;
    FileFragmenter fragmenter;
    bool is_working;
};


#endif //ENHANCEDUDPSERVER_WORKER_H
