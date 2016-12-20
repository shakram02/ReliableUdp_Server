//
// Created by ahmed on 12/17/16.
//

#ifndef ENHANCEDUDPSERVER_SOCKETSERVER_H
#define ENHANCEDUDPSERVER_SOCKETSERVER_H

#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <memory.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <cstdlib>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "../libs/netutils.h"
#include <string>

using namespace std;

class UdpSocketServer
{
public:
    /**
     * Creates a new instance of a UDP server, this doesn't start any networking operation
     */
    UdpSocketServer(const string serverIp, unsigned const short portNumber);

    ~UdpSocketServer();

    /**
     * Starts receiving clients, this should run on a separate process
     * @param clientRequestHandler Event handler to receive the packets
     */
    void StartReceiving(void (*clientRequestHandler)(const unsigned int redirectSocketFd,
            const sockaddr_in redirectAddr));

private:
    int CreateClientSocket(unsigned short &redirectPort, sockaddr_in &redirectAddr);

    string handShake;
    string serverIp;
    bool isReceiving;
    int socketFd;

    void PrintClientDetails(sockaddr_in clientAddr);

    void callwithredirect(unsigned int fd, sockaddr_in in);
};


#endif //ENHANCEDUDPSERVER_SOCKETSERVER_H
