//
// Created by ahmed on 12/21/16.
//



#include "ClientObserver.h"
#include "UdpWorker.h"

ClientObserver::ClientObserver(const string &serverIp, const unsigned short &portNumber) :
        welcome_socket(serverIp, portNumber)
{
    this->isReceiving = false;
}


void ClientObserver::StopListening()
{
    if (!this->isReceiving) {
        return;
    }
    this->isReceiving = false;
    welcome_socket.StopReceiving();
}

void ClientObserver::StartListening()
{
    // FIX ME
    if (this->isReceiving)return;

    this->isReceiving = true;


    this->welcome_socket.StartReceiving([](int socket_fd, sockaddr_in client_address) {
        UdpWorker w(socket_fd, client_address);
    });

}

