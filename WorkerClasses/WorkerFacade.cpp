//
// Created by ahmed on 12/23/16.
//

#include "WorkerFacade.h"

#define FRAGMENT_SIZE 100
#define MAX_FAIL_COUNT 10

void WorkerFacade::StartWorking()
{
    // TODO incoomplete

    int fail_count = 0;

    cout << "void WorkerFacade::StartWorking()" << endl;

    while (false && !fragmenter.EndOfFile() && is_working && fail_count < MAX_FAIL_COUNT) {
        // Generate the packet * window_size
        char **buff = {0};

        //fragmenter.NextFragment(buff);
        // Send it to the worker_socket, wait for the worker_socket response for the whole window
        // -use Select/Non blocking IO?-

        // Read the worker_socket response(s)
        // ACK -> WillAdvance = true
        // NACK -> WillAdvance = false

        free(buff);
    }

}

WorkerFacade::WorkerFacade(sock_descriptor sockfd) :
        worker_socket(sockfd)
{
    //this->worker_socket.
    this->is_working = true;
}

void WorkerFacade::StopWorking()
{
    this->is_working = false;
}

WorkerFacade::~WorkerFacade()
{
    // TODO releease resources
}