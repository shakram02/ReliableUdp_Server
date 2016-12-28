//
// Created by ahmed on 12/23/16.
//

#include <cstring>
#include <thread>
#include "WorkerFacade.h"

#include "../globaldef.h"

struct free_delete
{
    void operator()(void *x)
    { free(x); }
};

void WorkerFacade::StartWorking()
{
    // TODO reply with not found
    // TODO 2D fragments array from the fragmenter
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

    for (int frg_num = 0; pack_seq_num < fragment_count && is_working && (fail_count < MAX_FAIL_COUNT); frg_num++) {

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
                break;
            }

            fragmenter.NextFragment(&(buf_array[wind_frg]));

            cout << "Frag create #" << (pack_seq_num) << endl;
            pck_arr[wind_frg] = new DataPacket(
                    (buf_array[wind_frg]),
                    (unsigned short) frag_size,
                    (unsigned int) (pack_seq_num++)
            );
            free((buf_array[wind_frg]));
        }

        if (!GoBackN(wind_frg, pck_arr)) {
            cout << "GBN failed" << endl;
            free(buf_array);
            return;
        }
        // TODO FIX ME extra loops occur, loop condition is ill
        //cout << "FRG CNT:" << wind_frg << endl;
        free(buf_array);
    }

    if (!EndTransmission(fragment_count)) {
        cerr << "Err in receiving final ack" << endl;
    }
    cout << "Transmission completed." << endl;
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

bool WorkerFacade::EndTransmission(int total_frag_count)
{
    // End transmission
    DataPacket trans_end(NULL, 0, (unsigned int) total_frag_count);
    worker_socket.SendDataPacket(&trans_end);

    AckPacket final_ack;
    worker_socket.ReceiveAckPacket(&final_ack);
    return final_ack.ack_num == total_frag_count;
}

bool WorkerFacade::SendWindow(DataPacket *pck_arr_ptr[], int frg_count)
{

    // Send all fragments
    for (int k = 0; k < frg_count; ++k) {
        cout << "Create packet seq # " << pck_arr_ptr[k]->seqno
             //<< ", Data:" << pck_arr[k]->data
             << endl;

        worker_socket.SendDataPacket(pck_arr_ptr[k]);
        //delete pck_arr_ptr[k];  // FIXME this should be deleted after the window was successfully sent

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Wait for packet to be sent
    }
    return false;
}

void WorkerFacade::DeleteWindow(DataPacket *pck_arr_ptr[], int frg_count)
{
    for (int k = 0; k < frg_count; ++k) {
        delete pck_arr_ptr[k];  // FIXME this should be deleted after the window was successfully sent
    }
}

bool WorkerFacade::GoBackN(int frg_count, DataPacket **pck_arr_ptr)
{
    // NORMAL MODE
//    SendWindow(pck_arr_ptr, frg_count);
//    for (int l = 0; l < frg_count; ++l) {
//        AckPacket ack;
//        worker_socket.ReceiveAckPacket(&ack);
//        cout << "Ack:" << ack.ack_num << endl;
//    }

    if (frg_count < 1) {
        cout << "Empty window" << endl;
        return true;
    }

    int wind_base = this->last_acked_pkt + 1;
    int wind_end = this->last_acked_pkt + 1 + frg_count;

    int acknum = 0;
    int fail = 0;

    // GBN
    cout << endl
         << "Startimg GBN, Wind Base: " << wind_base
         << " Fragments in window: " << frg_count
         << endl;


    SendWindow(pck_arr_ptr, frg_count);
    int i = 0;
    do {
        cout << "Start ACK receive" << endl;

        AckPacket ack;
        if (!worker_socket.ReceiveAckPacket(&ack)) {
            // Failed to receive
            if (fail++ == MAX_FAIL_COUNT) {
                cerr << "Client died?" << endl;
                return false;
            }
            cout << "ACK receive failed, retransmitting window" << endl;
            SendWindow(pck_arr_ptr, frg_count);
            continue;
        }

        acknum = ack.ack_num;
        cout << "ACK received #" << acknum << endl;

        if (acknum == (this->last_acked_pkt + 1)) {

            // Slide the window
            cout << "ACK correct, slide the window ,received ACK#" << acknum << endl;
            this->last_acked_pkt++;

        } else if (acknum <= this->last_acked_pkt) {

            cout << "ACK Before window base #" << acknum << endl;
            SendWindow(pck_arr_ptr, frg_count);
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Wait for packet to be sent

        } else {

            cout << "ACK Invalid Received #" << acknum
                 << " Expected #" << (this->last_acked_pkt + 1)
                 << endl;
        }
        cout << "End of single GBN loop "
             << ", Awaited " << (this->last_acked_pkt + 1)
             << " Window end: " << wind_end
             << endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));  // Take it easy
    } while (this->last_acked_pkt < wind_end && i++ < frg_count);

    /// IMPORTANT Delete the sent window
    DeleteWindow(pck_arr_ptr, frg_count);
    cout << "Window released" << endl << endl;
    return true;

//    int last_ack = -1;
//    while (awaited_ack < wind_end) {
//
//        while (last_ack != awaited_ack) {
//            AckPacket ack;
//            bool ontime = worker_socket.ReceiveAckPacket(&ack);
//
//            if (ontime) {
//                last_ack = ack.ack_num;
//            }
//
//            if (last_ack == awaited_ack && ontime) {
//                awaited_ack++;
//            } else {
//                // Send window
//                std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Wait for packet to be sent
//            }
//        }
//
//    }
    return false;
}
