//
// Created by ahmed on 12/21/16.
//



#include "ClientObserver.h"
#include "UdpWorker.h"


void ClientObserver::OnClientConnect(const int redirectSocketFd, const sockaddr_in redirectAddr)
{
    //[redirectSocketFd, redirectAddr] { ClientTask(redirectSocketFd, redirectAddr); }
    //std::thread th(&ClientObserver::ClientTask, this, redirectSocketFd, redirectAddr);

    //this->client_threads.push_back(th);
}

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
        cout
                << "Worker Port: " << ntohs(client_address.sin_port)
                << "  IP:" << inet_ntoa(client_address.sin_addr)
                << endl;

        string message = "July";

        char buf[128] = {0};

        // Used with recvfrom to know the sender
        struct sockaddr_in client;
        unsigned int clientAddrSize = sizeof(client);

        // Weird switch to save declaring variables
        switch (recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *) &client, &clientAddrSize)) {
            case 0:
                log_error("client closed connection");

            case -1:
                log_error("recvfrom");

            default:
                cout << "AFTER REDIRECT:" << string(buf) << endl;
                break;
        }
    });

}

void ClientObserver::ClientTask(int socketFd, sockaddr_in addr)
{

    cout << "Child Socket "
         << ", IP:" << inet_ntoa(addr.sin_addr)
         << "  Port:" << to_string(ntohs(addr.sin_port));
}

