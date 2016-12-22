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
class UdpWorker
{
public:
    UdpWorker(int client_socket_descriptor, const sockaddr_in &client_address);

private:
    int socket_fd;
    sockaddr_in client_addr;

    bool AssertRedirection(int client_sock_fd);
};


#endif //ENHANCEDUDPSERVER_CLIENTUDPSOCKET_H
