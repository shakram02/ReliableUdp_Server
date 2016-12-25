//
// Created by ahmed on 12/24/16.
//

#ifndef ENHANCEDUDPSERVER_GBNSENDER_H
#define ENHANCEDUDPSERVER_GBNSENDER_H


#include <vector>
#include <queue>
#include "WorkerSocket.h"

class GbnSender
{
    int window_size;
    int confirmed;
    WorkerSocket &worker_sock;
    int sock_header_length;

    std::vector<std::pair<void *, unsigned short>> send_vector;

public:
    // TODO move the socket header in its own struct?
    GbnSender(unsigned int window_sz, WorkerSocket &send_sock, unsigned int socket_header_len);

    /**
     * Adds a data packet to this window ( the length of the data is in the packet )
     * @param packet Constructed data packet to be sent
     * @return
     */
    bool AddToSendQueue(DataPacket &packet);

    void SendWindow();

    bool ReceiveWindow();


};


#endif //ENHANCEDUDPSERVER_GBNSENDER_H
