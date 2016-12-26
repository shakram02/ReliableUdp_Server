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
    WorkerFacade(sock_descriptor sockfd);

    void StartWorking();

    void StopWorking();

    bool EndTransmission(int total_frag_count);

    bool SendWindow(DataPacket* pck_arr_ptr[],int frg_count);

    ~WorkerFacade();

private:
    WorkerSocket worker_socket;
    FileFragmenter fragmenter;
    bool is_working;
};


#endif //ENHANCEDUDPSERVER_WORKER_H
