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

#include "timecalc.h"
#include "UdpSocketServer.h"
#include <string>

#define SERVER_IP "192.168.1.7"
#define PORT_NUM 62135

void log_error(const char *);

void *get_in_addr(struct sockaddr *);

void on_recv(const string msg, const string senderInfo[], string &reply)
{
    cout << "Got:\"" << msg
         << "\" Address:" << senderInfo[0]
         << " Port:" << senderInfo[1] << " At:" << get_time() << endl;
}

int main()
{
    UdpSocketServer s(SERVER_IP, PORT_NUM);
    s.StartReceiving(on_recv);
    return 0;
}
