//
// Created by ahmed on 12/21/16.
//

#include <arpa/inet.h>
#include <cstring>
#include "WorkerSocket.h"
#include "../libs/netutils.h"

#define REDIRECT_SUCCESS "REDIRECT SUCCESSFUL"

WorkerSocket::WorkerSocket(int client_sockfd)
{
    this->socket_fd = client_sockfd;


    if (AssertRedirection()) {
        cout << "Client redirected successfully" << endl;

        string confirmation_msg = "OK";
        int len = sizeof((this->client_addr));

        for (int i = 0; i < 2; i++) {
            sendto(this->socket_fd, confirmation_msg.c_str(), confirmation_msg.size(), 0,
                    (sockaddr *) &(this->client_addr), len);
        }
    } else {
        cerr << "Client failed to redirect" << endl;
    }

    cout << "Worker IP: " << inet_ntoa(this->client_addr.sin_addr)
         << " Receive Port:" << ntohs(this->client_addr.sin_port)
         << " Socket:" << this->socket_fd
         << endl;
}

bool WorkerSocket::AssertRedirection()
{

    string buf = ReadProtocolString((int) strlen(REDIRECT_SUCCESS));
    // We don't need info about the client now
    return string(buf) == REDIRECT_SUCCESS;
}

string WorkerSocket::ReadProtocolString(int count)
{
    char buf[count] = {0};

    sockaddr_in c;
    memset(&c, 0, sizeof(c));
    socklen_t len = sizeof(struct sockaddr_in);

    recvfrom(this->socket_fd, buf, sizeof(buf), 0, (sockaddr *) &(this->client_addr), &len);


    cout << "Client IP: " << inet_ntoa((this->client_addr).sin_addr)
         << " Client Port:" << ntohs((this->client_addr).sin_port)
         << endl;

    return
            string(buf);
}

WorkerSocket::~WorkerSocket()
{
    cout << "Worker disposed" << endl;
    close(this->socket_fd);
}
