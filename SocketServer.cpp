//
// Created by ahmed on 12/17/16.
//

#include "SocketServer.h"


SocketServer::SocketServer(int socketType, int portNumber)
{
    struct addrinfo hints;
    struct addrinfo *serverinfo;
    memset(&hints, 0, sizeof(hints));

    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    hints.ai_family = AF_INET;    // Don't care IPv4 or IPv6
    // Fill in my IP for me - uses ip=0 (the network switch will give me an IP)
    hints.ai_flags = AI_PASSIVE;


    int err_status;

    // Constructs the sockaddr_in values and save me lots of time
    //int_to_string(22);
    if ((err_status = getaddrinfo(NULL, "22", &hints, &serverinfo) != 0))
    {
        printf("getaddrinfo:%s", gai_strerror(err_status));
        exit(1);
    }


    struct addrinfo *res;
    int sockfd;
    char ipstr[INET_ADDRSTRLEN];
    for (res = serverinfo; res != NULL; res = res->ai_next)
    {
        if ((sockfd = socket(res->ai_family, res->ai_socktype,
                res->ai_protocol)) == -1)
        {
            perror("listener: socket");
            continue;
        }
        if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("listener: bind");
            continue;
        }
        break;
    }

    if (res == NULL)
    {
        //log_error("bind socket");
    }

    freeaddrinfo(serverinfo);
}