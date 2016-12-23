//
// Created by ahmed on 12/21/16.
//

#ifndef ENHANCEDUDPSERVER_CLIENTUDPSOCKET_H
#define ENHANCEDUDPSERVER_CLIENTUDPSOCKET_H

#include <string>
#include <iostream>
#include <netinet/in.h>

using namespace std;

/**
 * An instance of this object will be created after a client is assigned a dedicated port
 * and will handle sending and receiving data / ack packets and will notify the above class
 * for any packets that are lost
 */
class WorkerSocket
{
public:
    WorkerSocket(int client_sockfd);

    ~WorkerSocket();

private:
    int socket_fd;
    sockaddr_in client_addr;

    bool AssertRedirection();

    string ReadProtocolString(int count);
};


#endif //ENHANCEDUDPSERVER_CLIENTUDPSOCKET_H