//
// Created by ahmed on 12/21/16.
//

#ifndef ENHANCEDUDPSERVER_CLIENTOBSERVER_H
#define ENHANCEDUDPSERVER_CLIENTOBSERVER_H

#include <vector>
#include <thread>
#include <netinet/in.h>
#include "WelcomingSocket.h"

using namespace std;

/**
 * The top class that redirects clients from the welcome gate to the dedicated
 * socket
 */
class ClientObserver
{
public :
    void StartListening();

    void StopListening();

    ClientObserver(const string &serverIp, const unsigned short &portNumber);


private:
    void OnClientConnect(const int redirectSocketFd, const sockaddr_in redirectAddr);

    void ClientTask(int socketFd, sockaddr_in addr);

    WelcomingSocket welcome_socket;
    bool isReceiving;
    std::thread mainThread;
    vector<thread> client_threads;


};


#endif //ENHANCEDUDPSERVER_CLIENTOBSERVER_H
