
#include <sys/socket.h>
#include "timecalc.h"
#include "../libs/data_packet.h"

#define BUFF_LEN 50

#include "UdpSocketServer.h"

#define SERVER_IP "192.168.1.7"
#define PORT_NUM 62135

void log_error(const char *);

void *get_in_addr(struct sockaddr *);


int main()
{
    UdpSocketServer s(SERVER_IP, PORT_NUM);
    s.StartReceiving([](const unsigned int sockfd, const sockaddr_in info) {
        cout << "Lambdad ";
        cout << sockfd << endl;
    });
    return 0;
}
