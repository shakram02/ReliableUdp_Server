//
// Created by ahmed on 12/23/16.
//

#include "WorkerFacade.h"

#define FRAGMENT_SIZE 2048
#define MAX_FAIL_COUNT 10
#define SERV_FILESZ_HEADER "FILESIZE-"

void WorkerFacade::StartWorking()
{
    // TODO incoomplete

    int fail_count = 0;

    cout << "void WorkerFacade::StartWorking()" << endl;

    basic_string<char> p(SERV_FILESZ_HEADER);
    string num(to_string(this->fragmenter.GetFragmentCount()));
    cout << "NUM:" << num << endl;
    p.append(num);
    //p.append("\0");

    cout << "WorkerFacade#Sending number of fragments:" << p << endl;

    worker_socket.SendPacket(p);

    while (!fragmenter.EndOfFile() && is_working && fail_count < MAX_FAIL_COUNT) {
        // Generate the packet * window_size
        char *buff = {0};

        fragmenter.NextFragment(&buff);
        // Send it to the worker_socket, wait for the worker_socket response for the whole window
        // -use Select/Non blocking IO?-
        worker_socket.SendPacket(basic_string<char>(buff));
        // Read the worker_socket response(s)
        // ACK -> WillAdvance = true
        // NACK -> WillAdvance = false

        free(buff);
    }

}

WorkerFacade::WorkerFacade(sock_descriptor sockfd) :
        worker_socket(sockfd)
{
    string file_name = this->worker_socket.GetRequestedFile();
    cout << "WorkerFacade#Requested filename:" << file_name << endl;
    this->fragmenter.SetFragmentSize(FRAGMENT_SIZE);
    if (!this->fragmenter.SetFilePath(file_name)) {
        cerr << "Worker#Failed to set the file path" << endl;
    }
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