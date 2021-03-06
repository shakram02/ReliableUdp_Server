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
#include <RawUdpSocket.h>

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
    WelcomingSocket(string &serverIp, unsigned short portNumber);

    ~WelcomingSocket();

    /**
     * Starts receiving clients, this should run on a separate process
     * @param clientRequestHandler Event handler to receive the packets
     */
    void StartReceiving();

    void StopReceiving();

    /**
     * Tells the state of the welcoming socket
     */
    bool is_receiving;
private:
    unique_ptr<RawUdpSocket> CreateClientSocket(string &client_ip);

    unique_ptr<RawUdpSocket> main_sock;
};


#endif //ENHANCEDUDPSERVER_SOCKETSERVER_H
