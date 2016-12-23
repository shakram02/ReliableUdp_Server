//
// Created by ahmed on 12/21/16.
//

#ifndef ENHANCEDUDPSERVER_CLIENTUDPSOCKET_H
#define ENHANCEDUDPSERVER_CLIENTUDPSOCKET_H

#include <string>
#include <iostream>
#include <netinet/in.h>

using namespace std;

/**
 * An instance of this object will be created after a client is assigned a dedicated port
 * and will handle sending and receiving data / ack packets and will notify the above class
 * for any packets that are lost
 */
class WorkerSocket
{
public:
    /**
     * Creates a worker socket to handle the client
     * @param client_sockfd socket file descriptor for the connected client
     */
    WorkerSocket(int client_sockfd);

    /**
     * Closes the assigned socket and disposes the object
     */
    ~WorkerSocket();

private:
    /**
     * Client socket file descriptor
     */
    int socket_fd;
    /**
     * The IP address information of the connected client
     */
    sockaddr_in client_addr;

    bool is_serving;

    /**
     * Waits the client message for port redirection
     * @return Whether the client redirected or not
     */
    bool AssertRedirection();
};


#endif //ENHANCEDUDPSERVER_CLIENTUDPSOCKET_H
