//
// Created by ahmed on 12/17/16.
//

#include "UdpSocketServer.h"
#include "../libs/data_packet.h"

// TODO remove this later
void log_error(const char *func_name)
{
    printf("%s:%s\n", func_name, strerror(errno));
}

UdpSocketServer::UdpSocketServer(const string serverIp, unsigned const short portNumber)
{

    unsigned int namelen;
    struct sockaddr_in server;

    server.sin_family = AF_INET;  /* Server is in Internet Domain */
    server.sin_port = htons(portNumber);
    inet_pton(AF_INET, serverIp.c_str(), &server.sin_addr.s_addr);

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

void UdpSocketServer::StartReceiving(void (*recvHandler)(char *msg, const string senderInfo[], char **reply))
{
    this->isReceiving = true;

    while (this->isReceiving)
    {
        char buf[256] = {0};

        struct sockaddr_in client;
        unsigned int client_address_size = sizeof(client);


        // Weird switch to save declaring variables
        switch (recvfrom(this->socketFd, buf, sizeof(buf), 0, (struct sockaddr *) &client, &client_address_size))
        {
            case 0:
                log_error("client closed connection");
                continue;
            case -1:
                log_error("recvfrom");
                continue;
            default:
                break;
        }

        char *reply;
        string clientInfo[2];
        clientInfo[0] = (inet_ntoa(client.sin_addr));
        clientInfo[1] = to_string(ntohs(client.sin_port));

        recvHandler(buf, clientInfo, &reply);   // Fire the event

        //cout << "#DEBUG sending:" << reply << endl;


        sendto(this->socketFd, reply, sizeof(data_packet), 0, (struct sockaddr *) &client, client_address_size);
    }
}

UdpSocketServer::~UdpSocketServer()
{
    close(socketFd);
}



