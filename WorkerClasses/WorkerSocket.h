//
// Created by ahmed on 12/21/16.
//

#ifndef ENHANCEDUDPSERVER_CLIENTUDPSOCKET_H
#define ENHANCEDUDPSERVER_CLIENTUDPSOCKET_H

#include <string>
#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <Packet.h>
#include <Checksum.h>
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

    /**
     * Sends raw binary data to the UDP port
     * @param data Raw binary data to be sent
     * @param length Length of the binary data
     */
    void SendPacket(const byte *data, unsigned int length); // TODO make private

    /**
     * Sends a wrapped data packet to the client
     * @param packet Packet containing data to be sent to the client
     */
    void SendPacket(Packet &packet);

    /**
     * Receives an ACK packet from the client
     * @param ack_packet_ptr Pointer to an ACK packet to be filled with data
     * @return Whether the receive process was sucessful
     */
    bool ReceiveAckPacket(unique_ptr<Packet> &ack_packet_ptr);


    /**
     * Receives raw data, and places them in (*data)
     * @param data received data, this is always allocated
     * @param buffer_length excpected length of the incoming data
     * @param received_size Actual size of received data
     * @return false if nothing is received
     */
    bool ReceiveRawPacket(unsigned int buffer_length, byte **data, int *received_size);

    /**
     * Client socket file descriptor
     */
    int socket_fd;

private:

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
