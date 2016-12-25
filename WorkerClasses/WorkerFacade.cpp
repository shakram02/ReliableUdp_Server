//
// Created by ahmed on 12/23/16.
//

#include <cstring>
#include "WorkerFacade.h"
#include "GbnSender.h"

#define FRAGMENT_SIZE 27   // MAX UDP transfer is 256 bytes
#define MAX_FAIL_COUNT 10
#define SERV_FILESZ_HEADER "FILESIZE-"

void WorkerFacade::StartWorking()
{
    // TODO incoomplete

    basic_string<char> file_send_header(SERV_FILESZ_HEADER);
    string num(to_string(this->fragmenter.GetFragmentCount()));
    file_send_header.append(num);

    cout << "WorkerFacade#Sending number of fragments:" << num << endl;
    worker_socket.SendPacket((void *) file_send_header.c_str(), (unsigned int) file_send_header.length());

    GbnSender sender(2, worker_socket);

    int fail_count = 0;
    bool recv_success = true;
    int pack_num = 0;
    while (recv_success && !fragmenter.EndOfFile() && is_working && fail_count < MAX_FAIL_COUNT) {

        void *buff = 0;
        int frag_size = fragmenter.NextFragment(&buff);
        DataPacket fragment_packet(buff, (unsigned short) frag_size, pack_num);
        //worker_socket.SendDataPacket(fragment_packet);
        sender.AddToSendQueue(fragment_packet);

        void *buff1 = 0;
        int frag_size1 = fragmenter.NextFragment(&buff1);
        DataPacket fragment_packet1(buff1, (unsigned short) frag_size1, pack_num + 1);
        //worker_socket.SendDataPacket(fragment_packet);
        sender.AddToSendQueue(fragment_packet1);


        sender.SendWindow();
        free(buff);
        free(buff1);


        cout << "Fragmenter#Fragment size:" << frag_size << " bytes, Seq#" << pack_num++ << endl;

        //AckPacket ack_packet;

        //recv_success = worker_socket.ReceiveAckPacket(&ack_packet);
        sender.ReceiveWindow();
        //cout << "ACK:" << ack_packet.ack_num << endl;


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
    // TODO release resources
    worker_socket.~WorkerSocket();
}