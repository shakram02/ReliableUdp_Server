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

    bool SendWindow(DataPacket *pck_arr_ptr[], int frg_count);

    bool GoBackN(int wnd_frg_count, DataPacket **pck_arr_ptr, int file_frg_count);

    ~WorkerFacade();

private:
    WorkerSocket worker_socket;
    FileFragmenter fragmenter;
    bool is_working;

    // Starting for -1 makes the ACK process logically straight to think of
    // as it prevents causing an off by one error
    int last_acked_pkt_id = -1;

    void DeleteWindow(DataPacket **pck_arr_ptr, int frg_count);
};


#endif //ENHANCEDUDPSERVER_WORKER_H
