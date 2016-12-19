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
#include "timecalc.h"

#define PORT_NUM "4950"
#define BUFF_LEN 50


void log_error(const char *);

void *get_in_addr(struct sockaddr *);

int main()
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
    if ((err_status = getaddrinfo(NULL, PORT_NUM, &hints, &serverinfo) != 0))
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
        log_error("bind socket");
        return 2;
    }

    freeaddrinfo(serverinfo);

    struct sockaddr_storage client_addr;
    unsigned int sotrage_len = sizeof(client_addr);
    char buffer[BUFF_LEN];  // memset this
    int num_bytes;

    while (1)
    {
        printf("Server:Waiting for a packet...\n");
        num_bytes = (int) recvfrom(sockfd, buffer, BUFF_LEN - 1, 0, (struct sockaddr *) &client_addr, &sotrage_len);
        if (num_bytes == -1)
        {
            log_error("receive from");
            exit(5);
        }

        char ip[INET_ADDRSTRLEN];
        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr),
                ip, INET_ADDRSTRLEN);
        printf("Got packet from:%s\n", ip);
        printf("Server: packet is %d bytes long\n", num_bytes);
        buffer[num_bytes] = '\0';   // Terminate the string
        printf("listener: packet contains \"%s\" at %s\n", buffer, get_time());    //%T

        sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client_addr, sotrage_len);

        if (!strcmp("exit", buffer))
        {
            break;
        }
    }


    return 0;
}

void log_error(const char *func_name)
{
    printf("%s:%s\n", func_name, strerror(errno));
}


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

