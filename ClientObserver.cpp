//
// Created by ahmed on 12/21/16.
//

#include "ClientObserver.h"
#include "WorkerClasses/WorkerFacade.h"

ClientObserver::ClientObserver(const string &serverIp, const unsigned short &portNumber) : welcome_socket(serverIp,
        portNumber)
{

}

void ClientObserver::StopListening()
{
    if (!welcome_socket.is_receiving) {
        return;
    }
    welcome_socket.StopReceiving();
}

void ClientObserver::StartListening()
{
    if (this->welcome_socket.is_receiving)return;
    this->welcome_socket.StartReceiving();
}

void ClientObserver::NotifyForClient(int sock_fd)
{

    std::thread th([=]() {
        time_t t = time(0);   // get time now
        struct tm *now = localtime(&t);

        cout << "Welcomed at "
             << now->tm_hour << ":"
             << now->tm_min << ":"
             << now->tm_sec << endl;

        // TODO Spawn a worker subsystem
        WorkerFacade worker_subsystem(sock_fd);
        //pthread_attr_setstacksize()
        worker_subsystem.StartWorking();
    });

    th.detach();
    //th.join();
    //client_threads.push_back(th);
}

