//
// Created by ahmed on 12/21/16.
//

#include <arpa/inet.h>
#include <cstring>
#include "WorkerSocket.h"
#include "../../libs/netutils.h"

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
    } else {
        string file_name = client_request.substr(client_request.find("-") + 1, client_request.size() - 1);
        cout << "Requested file name:" << file_name << endl;
    }

    return string(buf);
}