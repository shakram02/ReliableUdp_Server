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
#include "../libs/netutils.h"

class UdpSocketServer
{
public:
    UdpSocketServer(int portNumber);
    ~UdpSocketServer();
    void StartReceiving();

private:
    sockaddr_in endpoint;
    bool isReceiving;
    int socketFd;
};


#endif //ENHANCEDUDPSERVER_SOCKETSERVER_H
