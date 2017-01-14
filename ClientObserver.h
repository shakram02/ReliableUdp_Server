//
// Created by ahmed on 12/21/16.
//

#ifndef ENHANCEDUDPSERVER_CLIENTOBSERVER_H
#define ENHANCEDUDPSERVER_CLIENTOBSERVER_H

#include <vector>
#include <thread>
#include <netinet/in.h>
#include "WelcomingSocket.h"

//using namespace boost;
using namespace std;

/**
 * The top class that redirects clients from the welcome gate to the dedicated
 * socket
 */
class ClientObserver
{
public :
    /**
     * Start receiving clients and redirecting them
     */
    void StartListening();

    /**
     * Stop receiving clients
     */
    void StopListening();

    /**
     * Creates an instance of the client observer to handle incoming clients on spearate threads
     * @param serverIp The IP that the server will take (this machine)
     * @param portNumber The port number of the welcoming port,
     *          this number should be a constant and must be know for the clients
     */
    ClientObserver(string &serverIp, const unsigned short &portNumber);

    static void NotifyForClient(RawUdpSocket *redirect_socket, AddressInfo client_info);

private:

    /**
     * Main receiving socket that redirects the clients to their dedicated ports
     */
    WelcomingSocket welcome_socket;
    /**
     * Vector holding all client threads
     * Report if a client fails?
     */
    //static vector<std::thread> client_threads;
};


#endif //ENHANCEDUDPSERVER_CLIENTOBSERVER_H
