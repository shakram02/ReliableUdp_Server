//
// Created by ahmed on 12/23/16.
//

#include <cstring>
#include "WorkerFacade.h"

#define FRAGMENT_SIZE 256   // MAX UDP transfer is 256 bytes
#define MAX_FAIL_COUNT 10
#define SERV_FILESZ_HEADER "FILESIZE-"

void WorkerFacade::StartWorking()
{
    // TODO incoomplete

    int fail_count = 0;

    cout << "void WorkerFacade::StartWorking()" << endl;

    basic_string<char> file_send_header(SERV_FILESZ_HEADER);
    string num(to_string(this->fragmenter.GetFragmentCount()));
    file_send_header.append(num);

    cout << "WorkerFacade#Sending number of fragments:" << num << endl;
    worker_socket.SendPacket((void *) file_send_header.c_str(), (unsigned int) file_send_header.length());

    int i = 0;
    while (!fragmenter.EndOfFile() && is_working && fail_count < MAX_FAIL_COUNT) {
        // Generate the packet * window_size

        void *buff = {0};
        int frag_size = fragmenter.NextFragment(&buff);
        // Send it to the worker_socket, wait for the worker_socket response for the whole window
        // -use Select/Non blocking IO?-

        worker_socket.SendPacket(buff, frag_size);
        cout << "Fragmenter#Fragment size:" << frag_size << " bytes, #" << i++ << endl;

        void *rcv_buff = calloc(FRAGMENT_SIZE, sizeof(char));
        worker_socket.ReceivePacket(&rcv_buff, FRAGMENT_SIZE);
        cout << "ACK:" << string((char *) rcv_buff) << endl;

        // Read the worker_socket response(s)
        // ACK -> WillAdvance = true
        // NACK -> WillAdvance = false

        free(buff);
        free(rcv_buff);
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