//
// Created by ahmed on 12/21/16.
//

#include <arpa/inet.h>
#include "WorkerSocket.h"

/**
 * Redirection message, this is a constant message on the client and server side
 */
#define REDIRECT_SUCCESS "REDIRECT SUCCESSFUL"
#define SERV_REDIRECT_OK "OK"
#define MAX_FILE_PATH_LENGTH 256

WorkerSocket::WorkerSocket(int client_sockfd)
{
    this->socket_fd = client_sockfd;

    if (AssertRedirection()) {
        cout << "Client redirected successfully" << endl;
        string confirmation_msg(SERV_REDIRECT_OK);

        int len = sizeof((this->client_addr));

        // TODO refactor
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

    char buf[(int) strlen(REDIRECT_SUCCESS) + 1] = {0};
    socklen_t len = sizeof(struct sockaddr_in);

    recvfrom(this->socket_fd, buf, sizeof(buf), 0, (sockaddr *) &(this->client_addr), &len);
    cout << "WorkerFacade#Redirect Message:" << buf << endl;

    return string(buf) == string(REDIRECT_SUCCESS);
}

WorkerSocket::~WorkerSocket()
{
    cout << "WorkerFacade disposed" << endl;
    if (this->is_serving) {
        close(this->socket_fd);
    }
}

string WorkerSocket::GetRequestedFile()
{
    char buf[MAX_FILE_PATH_LENGTH + 1] = {0};
    socklen_t len = sizeof(struct sockaddr_in);

    // TODO refactor
    recvfrom(this->socket_fd, buf, sizeof(buf), 0, (sockaddr *) &(this->client_addr), &len);

    string prefix("FILE-");
    string client_request(buf);

    if (client_request.compare(0, prefix.size(), prefix)) {
        cerr << "Invalid file request packet" << endl;
        return string(buf);
    } else {
        string file_name = client_request.substr(client_request.find("-") + 1, client_request.size() - 1);
        cout << "Requested file name:" << file_name << endl;
        return file_name;
    }
}

void WorkerSocket::SendPacket(void *data, unsigned int length)
{
    long int num_bytes = sendto(this->socket_fd, data, length, 0,
            (sockaddr *) &(this->client_addr), (socklen_t) (sizeof(this->client_addr)));

    // Udp sendto fails when no space to allocate the buffer is avialable
    if (num_bytes < 1) {
        cerr << "Failed to send UDP packet" << endl;
        cout << endl;
    }
}

bool WorkerSocket::ReceivePacket(unsigned int buffer_size, void **data, int *received_size)
{
    socklen_t len = sizeof(struct sockaddr_in);

    (*received_size) = (int) recvfrom(this->socket_fd,
            (*data), buffer_size,
            0, (sockaddr *) &(this->client_addr), &len);

    if ((*received_size) == 0) {
        log_error("client closed connection");
        return false;
    } else if ((*received_size) == -1) {
        log_error("recvfrom");
        return false;
    }
    return true;
}

bool WorkerSocket::ReceiveAckPacket(AckPacket *deserialized_pckt)
{
    int size = 0;
    void *data = calloc(1, sizeof(AckPacket));
    AckPacket **temp;

    bool result = ReceivePacket(sizeof(AckPacket), &data, &size);

    if (!result) {
        free(data);
        return false;
    }
    if (size != sizeof(AckPacket)) {
        cerr << "Illegal state, received corrupt ACK packet" << endl;
        free(data);
        return false;
    }

    BinarySerializer::DeserializeAckPacket((data), temp);

    deserialized_pckt->chksum = (*temp)->chksum;
    deserialized_pckt->ack_num = (*temp)->ack_num;
    deserialized_pckt->len = (*temp)->len;

    free(data);

    return true;
}

void WorkerSocket::SendDataPacket(DataPacket &packet)
{
    void *data;
    BinarySerializer::SerializeDataPacket(&packet, &data);

    // sizeof(DataPacket) will return a size with the full array of 128 chars,
    // on the receiver size, the size will be re-fit using the length field
    SendPacket(data, sizeof(DataPacket));
}

