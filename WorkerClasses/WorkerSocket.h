//
// Created by ahmed on 12/21/16.
//

#ifndef ENHANCEDUDPSERVER_CLIENTUDPSOCKET_H
#define ENHANCEDUDPSERVER_CLIENTUDPSOCKET_H

#include <string>
#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <DataPacket.h>
#include <Checksum.h>
#include <AckPacket.h>
#include <BinarySerializer.h>
#include <netutils.h>


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

    string GetRequestedFile();

    bool SendPacket(void *data, unsigned int length);

    bool ReceivePacket(AckPacket &derserialized_pckt, unsigned int buffer_size, unsigned int expected_seqno);

    // TODO Receives raw data, if nothing is received the *data is freed, don't free
    /**
     * Receives raw data, and places them in (*data)
     * @param data received data, this is always allocated
     * @param buffer_length excpected length of the incoming data
     * @param received_size Actual size of received data
     * @return false if nothing is received
     */
    bool ReceivePacket(unsigned int buffer_length, void **data, int *received_size);

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
