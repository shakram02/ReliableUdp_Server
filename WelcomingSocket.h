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
#include <string>
#include <array>
#include<cstdint>
#include <functional>


#include "../libs/netutils.h"

using namespace std;

/**
 * This class accepts any newly connected clients and redirects them to a dedicated port
 * that will serve their requests
 */
class WelcomingSocket
{
public:
    /**
     * Creates a new instance of a UDP server, this doesn't start any networking operation
     */
    WelcomingSocket(const string serverIp, unsigned short portNumber);

    ~WelcomingSocket();

    /**
     * Starts receiving clients, this should run on a separate process
     * @param clientRequestHandler Event handler to receive the packets
     */
    void StartReceiving(std::function<void(int, sockaddr_in)> callback);

    void StopReceiving();

private:
    int CreateClientSocket(unsigned short &redirect_port, sockaddr_in &redirect_address);

    string handShake;
    string serverIp;
    bool isReceiving;
    int socketFd;

    void PrintClientDetails(sockaddr_in client_address);
};


#endif //ENHANCEDUDPSERVER_SOCKETSERVER_H
