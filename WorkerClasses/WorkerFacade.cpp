//
// Created by ahmed on 12/23/16.
//

#include <cstring>
#include <thread>
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

    int fail_count = 0;
    int pack_seq_num = 0;

    //void ***p = new(std::nothrow) void **(fragment_count);
    //unique_ptr<void **, free_delete> sp((void **) calloc(fragment_count, sizeof(char)));



    cout << "Fragment count:" << fragment_count << endl;

    for (int current_frag_num = 0;
         current_frag_num < fragment_count
         && is_working && (fail_count < MAX_FAIL_COUNT); current_frag_num++) {

        void **buf_array = (void **) calloc((size_t) WIN_SZ, sizeof(void *));

        //DataPacket **data_packets_ptrs = (DataPacket **) calloc(WIN_SZ, sizeof(DataPacket *));

        DataPacket *pck_arr[WIN_SZ];

        int window_pckt_count = 0;

        // Create fragments and load them in the data packets array
        for (; window_pckt_count < WIN_SZ; ++window_pckt_count) {

            int frag_size = fragmenter.GetNextFragmentSize();
            if (fragmenter.EndOfFile())break;

            (buf_array[window_pckt_count]) = calloc((size_t) frag_size, sizeof(char));

            if (frag_size < 1) {
                cerr << "Invalid fragment size" << endl;
                // Send an ACK to terminate transmission
                break;
            }

            fragmenter.NextFragment(&(buf_array[window_pckt_count]));

            pck_arr[window_pckt_count] = new DataPacket(
                    (buf_array[window_pckt_count]),
                    (unsigned short) frag_size,
                    (unsigned int) (pack_seq_num + window_pckt_count)
            );
            free((buf_array[window_pckt_count]));

            cout << "Frag data:" << (pck_arr[window_pckt_count])->data << endl;
        }

        // Send all fragments
        for (int k = 0; k < window_pckt_count; ++k) {
            cout << "Create packet seq # " << k << ", Data:" << pck_arr[k]->data << endl;
            worker_socket.SendDataPacket(pck_arr[k]);

            std::this_thread::sleep_for(std::chrono::milliseconds(20)); // Wait for packet to be sent
        }

        // Receive all ACKs
        for (int l = 0; l < window_pckt_count; ++l) {
            AckPacket ack;
            worker_socket.ReceiveAckPacket(&ack);
            cout << "Ack:" << ack.ack_num << endl;
        }

        // Used after sending data
//        for (int i = 0; i < window_pckt_count; ++i) {
//            free((buf_array[i]));
//            //delete (data_packets_ptrs[i]);       // TODO delete this ? or it's freed below
//        }

        pack_seq_num += WIN_SZ;

        free(buf_array);
        //free(data_packets_ptrs);
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