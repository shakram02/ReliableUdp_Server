//
// Created by ahmed on 12/17/16.
//



#include "WelcomingSocket.h"
#include "../libs/data_packet.h"

#define BUFF_LEN 256

#define HANDSHAKE "hndshk"
#define WELCOME_BUFFLEN strlen(HANDSHAKE)+1

// TODO remove this later
void log_error(const char *func_name)
{
    printf("%s:%s\n", func_name, strerror(errno));
}

WelcomingSocket::WelcomingSocket(const string serverIp, unsigned const short portNumber)
{
    // Set the handshake string
    this->handShake = HANDSHAKE;

    unsigned int namelen;
    struct sockaddr_in server;

    server.sin_family = AF_INET;  /* Server is in Internet Domain */
    server.sin_port = htons(portNumber);
    inet_pton(AF_INET, serverIp.c_str(), &server.sin_addr.s_addr);

    if ((this->socketFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        log_error("socket()");
        exit(1);
    }

    if (bind(this->socketFd, (struct sockaddr *) &server, sizeof(server)) < 0) {
        log_error("bind()");
        exit(2);
    }

    this->serverIp = serverIp;
    namelen = sizeof(server);
    if (getsockname(this->socketFd, (struct sockaddr *) &server, &namelen) < 0) {
        log_error("getsockname()");
        exit(3);
    }

    printf("Port assigned is %d\n", ntohs(server.sin_port));
}

void WelcomingSocket::StartReceiving(std::function<void(int, sockaddr_in)> callback)
{
    this->isReceiving = true;

    while (this->isReceiving) {

        struct sockaddr_in client;
        char buf[WELCOME_BUFFLEN] = {0};
        unsigned int clientAddrSize = sizeof(client);

        // Weird switch to save declaring variables
        switch (recvfrom(this->socketFd, buf, sizeof(buf), 0, (struct sockaddr *) &client, &clientAddrSize)) {
            case 0:
                log_error("client closed connection");
                continue;
            case -1:
                log_error("recvfrom");
                continue;
            default:
                break;
        }

        // Check validity of handshake message
        if (string(buf) != this->handShake) {
            // Ignore unwanted handshake
            cout << "Ignored suspicious client" << endl;
            continue;
        }

        PrintClientDetails(client); // Print details about the connected client

        sockaddr_in redirect_addr;
        unsigned short redirectPort;

        // FIXME close the open socket when done with it
        int redirect_socket_descriptor = CreateClientSocket(redirectPort, redirect_addr);

        string message = string("redirect") + to_string(redirectPort);

        cout << "Sending:" << message << endl;
        // Send redirect message to client
        sendto(this->socketFd, message.c_str(), message.size(),
                0, (struct sockaddr *) &client, clientAddrSize);

        callback(redirect_socket_descriptor, redirect_addr);   // Fire the event

    }
    cout << endl << "Stopped listening for ppl" << endl;
}

int WelcomingSocket::CreateClientSocket(unsigned short &redirect_port, sockaddr_in &redirect_address)
{

    // TODO this will be moved to the newly created process

    int redirect_socket_descriptor;
    redirect_address.sin_family = AF_INET;  /* Server is in Internet Domain */
    redirect_address.sin_port = 0;    // Get any available port
    // Use the same server IP, a clustered system would change the ip to a worker node
    inet_pton(AF_INET, this->serverIp.c_str(), &redirect_address.sin_addr.s_addr);

    if ((redirect_socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        log_error("socket()");
        exit(1);
    }


    if (bind(redirect_socket_descriptor, (struct sockaddr *) &redirect_address, sizeof(redirect_address)) < 0) {
        log_error("bind()");
        exit(2);
    }

    unsigned int namelen = sizeof(redirect_address);
    if (getsockname(redirect_socket_descriptor, (struct sockaddr *) &redirect_address, &namelen) < 0) {
        log_error("getsockname()");
        exit(3);
    }

    redirect_port = ntohs(redirect_address.sin_port);
    printf("Client redirect port is %d\n", redirect_port);

    timeval timeout;
    // Removing memset causes unidentified behaviour as the values are originally garbage
    memset(&timeout, 0, sizeof(timeout));
    timeout.tv_sec = 3;
    if (!setsockopt(redirect_socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeval))) {
        log_error("set receive timeout");
    }

    return redirect_socket_descriptor;
}


WelcomingSocket::~WelcomingSocket()
{
    close(socketFd);
}

void WelcomingSocket::PrintClientDetails(sockaddr_in client_address)
{
    cout << "A client connected"
         << ", IP:" << inet_ntoa(client_address.sin_addr)
         << "  Port:" << to_string(ntohs(client_address.sin_port))
         << endl;
}

void WelcomingSocket::StopReceiving()
{
    this->isReceiving = false;
}
