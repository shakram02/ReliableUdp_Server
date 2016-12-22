//
// Created by ahmed on 12/21/16.
//

#include <arpa/inet.h>
#include <cstring>
#include <zconf.h>
#include "UdpWorker.h"
#include "../libs/netutils.h"

#define REDIRECT_SUCCESS "REDIRECT SUCCESSFUL"

UdpWorker::UdpWorker(int client_socket_descriptor, const sockaddr_in &client_address)
{
    this->socket_fd = client_socket_descriptor;
    this->client_addr = client_address;

    cout
            << "Worker Port: " << ntohs(client_address.sin_port)
            << "  IP:" << inet_ntoa(client_address.sin_addr)
            << endl;

    string message = "July";
    if (AssertRedirection(client_socket_descriptor)) {
        cout << "Client redirected successfully" << endl;
    } else {
        cerr << "Client failed to redirect" << endl;
    }

}

bool UdpWorker::AssertRedirection(int client_sock_fd)
{
    char buf[strlen(REDIRECT_SUCCESS)] = {0};

    recvfrom(client_sock_fd, buf, sizeof(buf), 0, NULL, NULL);

    // We don't need info about the client now
    return string(buf) == REDIRECT_SUCCESS;

}
