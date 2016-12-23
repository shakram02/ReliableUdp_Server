//
// Created by ahmed on 12/21/16.
//

#include <arpa/inet.h>
#include <cstring>
#include "WorkerSocket.h"
#include "../libs/netutils.h"

/**
 * Redirection message, this is a constant message on the client and server side
 */
#define REDIRECT_SUCCESS "REDIRECT SUCCESSFUL"

WorkerSocket::WorkerSocket(int client_sockfd)
{
    this->socket_fd = client_sockfd;

    if (AssertRedirection()) {
        cout << "Client redirected successfully" << endl;
        string confirmation_msg = "OK";

        int len = sizeof((this->client_addr));

        sendto(this->socket_fd, confirmation_msg.c_str(),
                confirmation_msg.size(), 0,
                (sockaddr *) &(this->client_addr), (socklen_t) len);

        this->is_serving = true;

    } else {
        cerr << "Client failed to redirect" << endl;
        cout << endl;
        this->is_serving = false;
        close(socket_fd);
    }

    cout << "WorkerFacade#Client IP: " << inet_ntoa((this->client_addr).sin_addr)
         << " Port:" << ntohs((this->client_addr).sin_port)
         << endl;
}

bool WorkerSocket::AssertRedirection()
{

    char buf[(int) strlen(REDIRECT_SUCCESS)] = {0};
    socklen_t len = sizeof(struct sockaddr_in);

    recvfrom(this->socket_fd, buf, sizeof(buf), 0, (sockaddr *) &(this->client_addr), &len);

    cout << "WorkerFacade#Redirect Message:" << buf << endl;
    return string(buf) == REDIRECT_SUCCESS;
}

WorkerSocket::~WorkerSocket()
{
    cout << "WorkerFacade disposed" << endl;
    if (this->is_serving) {
        close(this->socket_fd);
    }
}
