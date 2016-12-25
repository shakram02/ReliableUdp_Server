//
// Created by ahmed on 12/23/16.
//

#include <cstring>
#include "WorkerFacade.h"
#include "GbnSender.h"
#include "../globaldef.h"


void WorkerFacade::StartWorking()
{
    basic_string<char> file_send_header(SERV_FILESZ_HEADER);
    int fragment_count = this->fragmenter.GetFragmentCount();
    string num(to_string(fragment_count));
    file_send_header.append(num);

    cout << "WorkerFacade#Sending number of fragments:" << num << endl;
    worker_socket.SendPacket((void *) file_send_header.c_str(), (unsigned int) file_send_header.length());

    GbnSender sender(WIN_SZ, worker_socket);


    int fail_count = 0;
    int pack_seq_num = 0;

    for (int j = 0; j < fragment_count && is_working && (fail_count < MAX_FAIL_COUNT); j++) {

        for (int i = 0; i < WIN_SZ; ++i) {
            int frag_size = fragmenter.GetNextFragmentSize();

            if (frag_size < 1) {
                cerr << "Invalid fragment size" << endl;
                break;
            }

            void *buf = calloc((size_t) frag_size, sizeof(char));
            fragmenter.NextFragment(&buf);

            cout << "Frag size:" << frag_size << endl;
            cout << "Data:" << (char *) buf << endl;

            DataPacket fragment_packet(buf, (unsigned short) frag_size, (unsigned int) (pack_seq_num + i));

            cout << "Create packet seq # " << (pack_seq_num + i) << endl;
            worker_socket.SendDataPacket(fragment_packet);
//            sender.AddToSendQueue(fragment_packet);
//            sender.SendWindow();
//            pack_seq_num += WIN_SZ;
//            sender.ReceiveWindow();

            free(buf);
        }


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