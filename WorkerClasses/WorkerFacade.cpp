//
// Created by ahmed on 12/23/16.
//

#include <cstring>
#include "WorkerFacade.h"
#include "GbnSender.h"
#include "../globaldef.h"

struct free_delete
{
    void operator()(void *x)
    { free(x); }
};

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

    //void ***p = new(std::nothrow) void **(fragment_count);
    //unique_ptr<void **, free_delete> sp((void **) calloc(fragment_count, sizeof(char)));

    //vector<shared_ptr
    void **buf_array = (void **) calloc((size_t) WIN_SZ, sizeof(void *));

    cout << "Fragment count:" << fragment_count << endl;

    for (int j = 0; j < fragment_count && is_working && (fail_count < MAX_FAIL_COUNT); j++) {

        int window_pckt_count = 0;
        for (; window_pckt_count < WIN_SZ; ++window_pckt_count) {

            int frag_size = fragmenter.GetNextFragmentSize();
            if (fragmenter.EndOfFile())break;

            (buf_array[j]) = calloc((size_t) frag_size, sizeof(char));

            if (frag_size < 1) {
                cerr << "Invalid fragment size" << endl;
                // Send an ACK to terminate transmission
                break;
            }

            //void *buf = calloc((size_t) frag_size, sizeof(char));
            //fragmenter.NextFragment(&buf);

            fragmenter.NextFragment(&buf_array[j]);

            cout << "Frag size:" << frag_size << endl;

            //DataPacket fragment_packet(buf, (unsigned short) frag_size, (unsigned int) (pack_seq_num + window_pckt_count));
            DataPacket fragment_packet(buf_array[j], (unsigned short) frag_size,
                    (unsigned int) (pack_seq_num + window_pckt_count));

            cout << "Create packet seq # " << (pack_seq_num + window_pckt_count) << endl;
            worker_socket.SendDataPacket((fragment_packet));

            AckPacket ack;
            worker_socket.ReceiveAckPacket(&ack);
            cout << "Ack:" << ack.ack_num << endl;

            //sender.AddToSendQueue(fragment_packet);
            //sender.SendWindow();
            //bool acked = sender.ReceiveWindow();

//            if (!acked) {
//                fail_count++;
//
//                // TODO remove this
//                // TODO remove this
//                // TODO remove this
//                // TODO remove this
//                return;
//            }

            free(buf_array[window_pckt_count]);
            //free(fragment_packet);
        }

        pack_seq_num += WIN_SZ;
//        for (int k = 0; k < window_pckt_count; ++k) {
//            free(buf_array[k]);
//        }
    }
    free(buf_array);
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