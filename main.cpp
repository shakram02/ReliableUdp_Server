
#include <sys/socket.h>
#include "timecalc.h"
#include "../libs/data_packet.h"

#include "WelcomingSocket.h"
#include "ClientObserver.h"
#include "WorkerClasses/FileFragmenter.h"
#include "server_config.h"


void log_error(const char *);

void *get_in_addr(struct sockaddr *);


int main()
{
    string serv_ip(SERVER_IP);
    ClientObserver obs(serv_ip, PORT_NUM);
    obs.StartListening();

    //WelcomingSocket sock(serv_ip, PORT_NUM);

    string d;
    cout << "Hit enter to terminate..." << endl;
    cin >> d;

    obs.StopListening();

//    TestFileFragmenter();
    return 0;
}
