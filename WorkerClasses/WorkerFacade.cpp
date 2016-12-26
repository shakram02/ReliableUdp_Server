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

    for (int frg_num = 0; frg_num < fragment_count && is_working && (fail_count < MAX_FAIL_COUNT); frg_num++) {

        void **buf_array = (void **) calloc((size_t) WIN_SZ, sizeof(void *));
        DataPacket *pck_arr[WIN_SZ];

        int wind_frg = 0;

        // Create fragments and load them in the data packets array
        for (; wind_frg < WIN_SZ; ++wind_frg) {

            int frag_size = fragmenter.GetNextFragmentSize();
            if (fragmenter.EndOfFile())break;

            (buf_array[wind_frg]) = calloc((size_t) frag_size, sizeof(char));

            if (frag_size < 1) {
                cerr << "Invalid fragment size" << endl;
                // Send an ACK to terminate transmission
                break;
            }

            fragmenter.NextFragment(&(buf_array[wind_frg]));

            pck_arr[wind_frg] = new DataPacket(
                    (buf_array[wind_frg]),
                    (unsigned short) frag_size,
                    (unsigned int) (pack_seq_num++ + wind_frg)
            );
            free((buf_array[wind_frg]));

//            cout << "Frag data:" << (pck_arr[wind_frg])->data << endl;
        }

        // Send all fragments
        for (int k = 0; k < wind_frg; ++k) {
            cout << "Create packet seq # " << pck_arr[k]->seqno
                 //<< ", Data:" << pck_arr[k]->data
                 << endl;

            worker_socket.SendDataPacket(pck_arr[k]);
            delete pck_arr[k];

            std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Wait for packet to be sent
        }

        // Receive all wind ACKs
        for (int l = 0; l < wind_frg; ++l) {
            AckPacket ack;
            worker_socket.ReceiveAckPacket(&ack);
            cout << "Ack:" << ack.ack_num << endl;
        }

        free(buf_array);
    }
}


WorkerFacade::WorkerFacade(sock_descriptor sockfd) : worker_socket(sockfd)
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