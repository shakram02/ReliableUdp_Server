#include "WelcomingSocket.h"
#include "ClientObserver.h"
#include "server_config.h"

int main()
{
    string serv_ip(SERVER_IP);
    ClientObserver obs(serv_ip, PORT_NUM);
    obs.StartListening();

    string d;
    cout << "Hit enter to terminate..." << endl;
    cin >> d;

    obs.StopListening();
    return 0;
}
