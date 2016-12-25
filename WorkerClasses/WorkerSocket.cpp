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

bool WorkerSocket::SendPacket(void *data, unsigned int length)
{
    sendto(this->socket_fd, data, length, 0,
            (sockaddr *) &(this->client_addr), (socklen_t) (sizeof(this->client_addr)));
    return false;
}

bool WorkerSocket::ReceiveAckPacket(AckPacket **deserialized_pckt)
{
    void *data = calloc(sizeof(AckPacket), sizeof(char));
    socklen_t len = sizeof(struct sockaddr_in);

    int received = (int) recvfrom(this->socket_fd, (data),
            sizeof(AckPacket), 0, (sockaddr *) &(this->client_addr), &len);

    if (received == 0) {
        log_error("client closed connection");
    } else if (received == -1) {
        log_error("recvfrom");
    }


    if (received < 1) {
        // Client closed, timeout
        free(data);
        return false;
    } else {

        // FIXME fix this
        //BinarySerializer::DeserializeAckPacket(data, deserialized_pckt);
        free(data);
        return true;
    }
}

bool WorkerSocket::ReceivePacket(unsigned int buffer_size, void *data, int *received_size)
{
    socklen_t len = sizeof(struct sockaddr_in);

    (*received_size) = (int) recvfrom(this->socket_fd,
            data, buffer_size,
            0, (sockaddr *) &(this->client_addr), &len);

    // TODO realloc the data* to avoid memory waste

    return *received_size >= 1;
}