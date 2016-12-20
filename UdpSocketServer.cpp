//
// Created by ahmed on 12/17/16.
//

#include "UdpSocketServer.h"
#include "../libs/data_packet.h"

#define BUFF_LEN 256

#define HANDSHAKE "hndshk"
#define WELCOME_BUFFLEN strlen(HANDSHAKE)+1

// TODO remove this later
void log_error(const char *func_name)
{
    printf("%s:%s\n", func_name, strerror(errno));
}

UdpSocketServer::UdpSocketServer(const string serverIp, unsigned const short portNumber)
{
    this->handShake = HANDSHAKE;

    unsigned int namelen;
    struct sockaddr_in server;

    server.sin_family = AF_INET;  /* Server is in Internet Domain */
    server.sin_port = htons(portNumber);
    inet_pton(AF_INET, serverIp.c_str(), &server.sin_addr.s_addr);

    if ((this->socketFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        log_error("socket()");
        exit(1);
    }


    if (bind(this->socketFd, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        log_error("bind()");
        exit(2);
    }

    this->serverIp = serverIp;
    namelen = sizeof(server);
    if (getsockname(this->socketFd, (struct sockaddr *) &server, &namelen) < 0)
    {
        log_error("getsockname()");
        exit(3);
    }

    printf("Port assigned is %d\n", ntohs(server.sin_port));
}

void UdpSocketServer::StartReceiving(void (*clientRequestHandler)(const unsigned int redirectSocketFd,
        const sockaddr_in redirectAddr))
{
    this->isReceiving = true;

    while (this->isReceiving)
    {

        struct sockaddr_in client;
        char buf[WELCOME_BUFFLEN] = {0};
        unsigned int clientAddrSize = sizeof(client);

        // Weird switch to save declaring variables
        switch (recvfrom(this->socketFd, buf, sizeof(buf), 0, (struct sockaddr *) &client, &clientAddrSize))
        {
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
        if (string(buf) != this->handShake)
        {
            // Ignore unwanted handshake
            cout << "Ignored suspicious client" << endl;
            continue;
        }

        PrintClientDetails(client);

        sockaddr_in redirectAddr;
        unsigned short redirectPort;
        unsigned int redirectSockFd = CreateClientSocket(redirectPort, redirectAddr);
        string message = string("redirect") + to_string(redirectPort);

        cout << "Sending:" << message << endl;
        // Send redirect message to client
        sendto(this->socketFd, message.c_str(), message.size(),
                0, (struct sockaddr *) &client, clientAddrSize);

        // Alert the RequestHandler, create a child process
        clientRequestHandler(redirectSockFd, redirectAddr);   // Fire the event
        callwithredirect(redirectSockFd, redirectAddr); // In the child process
    }
}

int UdpSocketServer::CreateClientSocket(unsigned short &redirectPort, sockaddr_in &redirectAddr)
{

    // TODO this will be moved to the newly created process

    int redirectSockFd;
    redirectAddr.sin_family = AF_INET;  /* Server is in Internet Domain */
    redirectAddr.sin_port = 0;    // Get any available port
    // Use the same server IP, a clustered system would change the ip to a worker node
    inet_pton(AF_INET, this->serverIp.c_str(), &redirectAddr.sin_addr.s_addr);

    if ((redirectSockFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        log_error("socket()");
        exit(1);
    }


    if (bind(redirectSockFd, (struct sockaddr *) &redirectAddr, sizeof(redirectAddr)) < 0)
    {
        log_error("bind()");
        exit(2);
    }

    unsigned int namelen = sizeof(redirectAddr);
    if (getsockname(redirectSockFd, (struct sockaddr *) &redirectAddr, &namelen) < 0)
    {
        log_error("getsockname()");
        exit(3);
    }

    redirectPort = ntohs(redirectAddr.sin_port);
    printf("Client redirect port is %d\n", redirectPort);

    timeval timeout;
    // Removing memset causes unidentified behaviour as the values are originally garbage
    memset(&timeout, 0, sizeof(timeout));
    timeout.tv_sec = 3;
    if (!setsockopt(redirectSockFd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeval)))
    {
        log_error("set receive timeout");
    }

    return redirectSockFd;
}


UdpSocketServer::~UdpSocketServer()
{
    close(socketFd);
}

void UdpSocketServer::PrintClientDetails(sockaddr_in clientAddr)
{
    string clientInfo[2];
    clientInfo[0] = (inet_ntoa(clientAddr.sin_addr));
    clientInfo[1] = to_string(ntohs(clientAddr.sin_port));

    cout << "A client connected"
         << ", IP:" << clientInfo[0]
         << "  Port:" << clientInfo[1]
         << endl;
}

void UdpSocketServer::callwithredirect(unsigned int fd, sockaddr_in in)
{
    struct sockaddr_in client;
    char buf[WELCOME_BUFFLEN] = {0};
    unsigned int clientAddrSize = sizeof(client);

    // Weird switch to save declaring variables
    switch (recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *) &client, &clientAddrSize))
    {
        case 0:
            log_error("client closed connection");

        case -1:
            log_error("recvfrom");

        default:
            cout << "AFTER REDIRECT:" << string(buf) << endl;
            break;
    }

}
