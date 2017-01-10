
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
    ClientObserver obs(SERVER_IP, PORT_NUM);
    obs.StartListening();

    string d;
    cout << "Hit enter to terminate..." << endl;
    cin >> d;

    obs.StopListening();

//    TestFileFragmenter();
    return 0;
}
