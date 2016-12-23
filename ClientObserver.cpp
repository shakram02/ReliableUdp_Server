//
// Created by ahmed on 12/21/16.
//

#include "ClientObserver.h"

ClientObserver::ClientObserver(const string &serverIp, const unsigned short &portNumber) : welcome_socket(serverIp,
        portNumber)
{
}


void ClientObserver::StopListening()
{
    if (!welcome_socket.is_receiving) {
        return;
    }
    this->isReceiving = false;
    welcome_socket.StopReceiving();
}

void ClientObserver::StartListening()
{
    if (this->welcome_socket.is_receiving)return;

//    this->welcome_socket.StartReceiving([](int socket_fd, sockaddr_in client_address) {
//        WorkerSocket w(socket_fd, client_address);
//        std::thread th(ClientObserver::OnClientConnect, this, socket_fd, client_address);
//        th.detach();
//    });
    //boost::thread th(bind(&ClientObserver::ServeClient, this));
    this->welcome_socket.StartReceiving();

}


void ClientObserver::ServeClient(void *thing)
{
    cerr << "Thread HELLO!!!" << endl;
}

void ClientObserver::NotifyForClient(int sock_fd)
{

    std::thread th([=]() {
        cerr << "Welcomed" << endl;
        WorkerSocket ws(sock_fd);
    });

    th.detach();
}


//obs->ServeClient();

