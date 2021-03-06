//
// Created by ahmed on 12/23/16.
//

#ifndef ENHANCEDUDPSERVER_WORKER_H
#define ENHANCEDUDPSERVER_WORKER_H


#include "FileFragmenter.h"
#include <Packet.h>
#include <RawUdpSocket.h>
#include <list>

typedef std::list<unique_ptr<Packet>> PacketList;

/**
 * Provides an interface to a subsystem that handles file fragmentation
 * and socket delivery using GBN protocol
 */
class WorkerFacade
{
public:
    WorkerFacade(RawUdpSocket *redirect_socket, AddressInfo client_info);

    void StartWorking();

    void StopWorking();

    ~WorkerFacade();

private:
    unique_ptr<RawUdpSocket> worker_socket;
    unique_ptr<AddressInfo> client_info;
    FileFragmenter fragmenter;
    bool is_working;
    unsigned int lost_winds = 0;
    // Starting for -1 makes the ACK process logically straight to think of
    // as it prevents causing an off by one error
    int last_acked_pkt_id = -1;
};


#endif //ENHANCEDUDPSERVER_WORKER_H
