//
// Created by ahmed on 12/23/16.
//

#include "WorkerFacade.h"

#define FRAGMENT_SIZE 100
#define MAX_FAIL_COUNT 10

void WorkerFacade::StartWorking()
{
    int fail_count = 0;

    cout << "void WorkerFacade::StartWorking()" << endl;

    while (!fragmenter.EndOfFile() && is_working && fail_count < MAX_FAIL_COUNT) {
        // Generate the packet * window_size
        char **buff = {0};

//        fragmenter.NextFragment(buff);
        // Send it to the socket, wait for the socket response for the whole window
        // -use Select/Non blocking IO?-

        // Read the socket response(s)
        // ACK -> WillAdvance = true
        // NACK -> WillAdvance = false

        free(buff);
    }

}

WorkerFacade::WorkerFacade(sock_descriptor sockfd, string file_name) :
        socket(sockfd), fragmenter(file_name, FRAGMENT_SIZE)
{
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