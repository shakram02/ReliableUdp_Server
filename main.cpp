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

#define BUFF_LEN 50

#include "UdpSocketServer.h"
#include <string>

#define SERVER_IP "192.168.1.7"
#define PORT_NUM 62135

void log_error(const char *);

void *get_in_addr(struct sockaddr *);

void on_recv(string msg, string& reply)
{
    cout << "Got:" << msg << endl;
    (reply) = "backreplasdasdasdasdy";
}

int main()
{
    UdpSocketServer s(SERVER_IP, PORT_NUM);
    s.StartReceiving(on_recv);
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

