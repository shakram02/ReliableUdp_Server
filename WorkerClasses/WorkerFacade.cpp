//
// Created by ahmed on 12/23/16.
//

#include <cstring>
#include "WorkerFacade.h"
#include "GbnSender.h"
#include "../globaldef.h"


void WorkerFacade::StartWorking()
{
    // TODO incoomplete

    basic_string<char> file_send_header(SERV_FILESZ_HEADER);
    int fragment_count = this->fragmenter.GetFragmentCount();
    string num(to_string(fragment_count));
    file_send_header.append(num);

    cout << "WorkerFacade#Sending number of fragments:" << num << endl;
    worker_socket.SendPacket((void *) file_send_header.c_str(), (unsigned int) file_send_header.length());

    GbnSender sender(WIN_SZ, worker_socket);

    // TODO disable
    //void **buff = (void **) calloc((size_t) fragment_count, sizeof(void *));



    int fail_count = 0;
    int pack_num = 0;


    for (int j = 0; j < fragment_count && is_working && (fail_count < MAX_FAIL_COUNT); j++) {


//        for (int i = 0; i < WIN_SZ; ++i) {
//            int frag_size = fragmenter.GetNextFragmentSize();
//
//            if (frag_size < 1) {
//                cerr << "Invalid fragment size" << endl;
//                return;
//            }
//
//            buff[i] = calloc(frag_size, sizeof(char));
//            fragmenter.NextFragment(&(buff[i]));
//
//            //cout << "Sending:" << (char *) buff[i] << endl;
//
//            DataPacket fragment_packet(buff[i], (unsigned short) frag_size, (unsigned int) (pack_num + i));
//            sender.AddToSendQueue(fragment_packet);
//            cout << "Create packet seq#" << pack_num++ << endl;
//
//            //if (!frag_size)recv_success = false;
//        }

        int frag_size = fragmenter.GetNextFragmentSize();

        if (frag_size < 1) {
            cerr << "Invalid fragment size" << endl;
            return;
        }
        void *baf = calloc((size_t) frag_size, sizeof(char));
        cout << "Frag size:" << fragmenter.NextFragment(&(baf)) << endl;

        //cout << "Sending:" << (char *) buff[i] << endl;

        cout << "Allocating << " << (unsigned short) frag_size << " bytes" << endl;

        DataPacket fragment_packet(baf, (unsigned short) frag_size,
                (unsigned int) (pack_num++));

        sender.AddToSendQueue(fragment_packet);
        sender.SendWindow();

        free(baf);

//        pack_num += WIN_SZ;
//        sender.SendWindow();
//        for (int i = 0; i < WIN_SZ; i++) {
//            free(buff[i]);
//        }

        //cout << "Fragmenter#Fragment size:" << frag_size << " bytes,


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