
#include <sys/socket.h>
#include "timecalc.h"
#include "../libs/data_packet.h"

#define BUFF_LEN 50

#include "WelcomingSocket.h"
#include "ClientObserver.h"
#include "FileFragmenter.h"

#define SERVER_IP "192.168.1.7"
#define PORT_NUM 62135

void log_error(const char *);

void *get_in_addr(struct sockaddr *);

void TestFileFragmenter()
{
    FileFragmenter ff("mizo.txt", 5);
    char *fragm;

    while (!ff.EndOfFile()) {
        ff.NextFragment(&fragm);
        cout << "frag: " << fragm << endl;
    }

    free(fragm);
}

int main()
{
//    ClientObserver obs(SERVER_IP, PORT_NUM);
//    obs.StartListening();
//
//    string d;
//    cout << "Hit enter to terminate..." << endl;
//    cin >> d;
//
//    obs.StopListening();

    //TestFileFragmenter();
    return 0;
}
