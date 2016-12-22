//
// Created by ahmed on 12/21/16.
//

#include <arpa/inet.h>
#include <cstring>
#include <zconf.h>
#include "UdpWorker.h"
#include "../libs/netutils.h"

UdpWorker::UdpWorker(int client_socket_descriptor, const sockaddr_in &client_address)
{
    this->socket_fd = client_socket_descriptor;
    this->client_addr = client_address;

    cout
            << "Worker Port: " << ntohs(client_address.sin_port)
            << "  IP:" << inet_ntoa(client_address.sin_addr)
            << endl;

    string message = "July";

//    for (int i = 0; i < 10; i++) {
//        ssize_t bytes = sendto(this->socket_fd, "abcd", strlen("abcd"),
//                0, (struct sockaddr *) &client_addr, sizeof(client_addr));
//
//        cout << "Sent " << bytes << " bytes" << endl;
//        usleep(1000 * 1000 * 1000);
//    }

    char buf[128] = {0};
    struct sockaddr_in client;
    unsigned int client_addr_size = sizeof(client);

    // Weird switch to save declaring variables
    switch (recvfrom(client_socket_descriptor, buf, sizeof(buf), 0, (struct sockaddr *) &client,
            &client_addr_size)) {
        case 0:
            log_error("client closed connection");

        case -1:
            log_error("recvfrom");

        default:
            cout << "AFTER REDIRECT:" << string(buf) << endl;
            break;
    }

}
