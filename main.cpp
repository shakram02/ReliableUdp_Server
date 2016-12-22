
#include <sys/socket.h>
#include "timecalc.h"
#include "../libs/data_packet.h"

#define BUFF_LEN 50

#include "WelcomingSocket.h"
#include "ClientObserver.h"

#define SERVER_IP "192.168.1.7"
#define PORT_NUM 62135

void log_error(const char *);

void *get_in_addr(struct sockaddr *);


int main()
{
//    WelcomingSocket s(SERVER_IP, PORT_NUM);
//    s.StartReceiving([](int32_t sockfd, const sockaddr_in info) {
//        cout << "Lambdad ";
//        cout << sockfd << endl;
//    });

    ClientObserver obs(SERVER_IP, PORT_NUM);
    obs.StartListening();

    string d;
    cout << "Hit enter to terminate..." << endl;
    cin >> d;

    obs.StopListening();

    return 0;
}
