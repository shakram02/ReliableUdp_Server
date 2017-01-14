//
// Created by ahmed on 12/21/16.
//

#include "ClientMediator.h"
#include "WorkerClasses/WorkerFacade.h"

ClientMediator::ClientMediator(string &serverIp, const unsigned short &portNumber) :
        welcome_socket(serverIp, portNumber)
{

}

void ClientMediator::StopListening()
{
    if (!welcome_socket.is_receiving) {
        return;
    }
    welcome_socket.StopReceiving();
}

void ClientMediator::StartListening()
{
    if (this->welcome_socket.is_receiving)return;
    this->welcome_socket.StartReceiving();
}

void ClientMediator::NotifyForClient(RawUdpSocket *redirect_socket, AddressInfo client_info)
{
    std::thread th([redirect_socket, client_info]() {
        time_t t = time(0);   // get time now
        struct tm *now = localtime(&t);

        cout << "Welcomed at "
             << now->tm_hour << ":"
             << now->tm_min << ":"
             << now->tm_sec << endl;

        // TODO Spawn a worker subsystem
        WorkerFacade worker_subsystem(redirect_socket, client_info);
        worker_subsystem.StartWorking();
    });

    th.detach();
}

