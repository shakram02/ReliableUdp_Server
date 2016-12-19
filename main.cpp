
#include <sys/socket.h>
#include "timecalc.h"
#include "../libs/data_packet.h"

#define BUFF_LEN 50

#include "UdpSocketServer.h"

#define SERVER_IP "192.168.1.7"
#define PORT_NUM 62135

void log_error(const char *);

void *get_in_addr(struct sockaddr *);

void on_recv(char *msg, const string senderInfo[], char **reply)
{
    data_packet packet;
    packet.seqno = 12;
    packet.len = 24;

    memset(packet.data, 0, sizeof(packet.data));
    memcpy(packet.data, "HALOLOA", sizeof(packet.data));

    *reply = reinterpret_cast<char *>(&packet);

    data_packet *unpacked = reinterpret_cast<data_packet *>(msg);
    cout << "Got:\"" << unpacked->data
         << "\" Address:" << senderInfo[0]
         << " Port:" << senderInfo[1] << " At:" << get_time() << endl;
}

int main()
{
    UdpSocketServer s(SERVER_IP, PORT_NUM);


    s.StartReceiving(on_recv);
    return 0;
}
