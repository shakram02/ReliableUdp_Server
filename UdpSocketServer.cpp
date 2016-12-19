//
// Created by ahmed on 12/17/16.
//

#include "UdpSocketServer.h"
#include "timecalc.h"


UdpSocketServer::UdpSocketServer(char *serverIp, int portNumber)
{

    unsigned int namelen;
    struct sockaddr_in server;


    server.sin_family = AF_INET;  /* Server is in Internet Domain */
    server.sin_port = htons(portNumber);
    inet_pton(AF_INET, serverIp, &server.sin_addr.s_addr);

    if ((this->socketFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        log_error("socket()");
        exit(1);
    }


    if (bind(this->socketFd, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        log_error("bind()");
        exit(2);
    }

    namelen = sizeof(server);
    if (getsockname(this->socketFd, (struct sockaddr *) &server, &namelen) < 0)
    {
        log_error("getsockname()");
        exit(3);
    }

    printf("Port assigned is %d\n", ntohs(server.sin_port));
}

void UdpSocketServer::StartReceiving(void (*recvHandler)(string msg, string &reply))
{
    this->isReceiving = true;

    while (this->isReceiving)
    {
        char buf[32] = {0};

        struct sockaddr_in client;
        unsigned int client_address_size = sizeof(client);

        if (recvfrom(this->socketFd, buf, sizeof(buf), 0, (struct sockaddr *) &client,
                &client_address_size) < 0)
        {
            log_error("recvfrom()");
            exit(4);
        }


        printf("Received message %s - port %d internet address %s at %s\n",
                buf,
                ntohs(client.sin_port),
                inet_ntoa(client.sin_addr), get_time());

        string reply;
        recvHandler(buf, reply);   // Fire the event

        cout << "#DEBUG sending:" << reply << endl;
        sendto(this->socketFd, reply.c_str(), reply.size(), 0, (struct sockaddr *) &client, client_address_size);

    }
}

UdpSocketServer::~UdpSocketServer()
{
    close(socketFd);
}

