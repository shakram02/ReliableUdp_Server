//
// Created by ahmed on 12/23/16.
//

#include <cstring>
#include <thread>
#include "WorkerFacade.h"

#include "../globaldef.h"


extern "C"
{
#include "plp.h"
};

struct free_delete
{
    void operator()(void *x)
    { free(x); }
};

void WorkerFacade::StartWorking()
{
    // TODO reply with not found
    // TODO 2D fragments array from the fragmenter
    // TODO Allocate data packets in memory and serialize from memory
    // TODO Checksum
    basic_string<char> file_send_header(SERV_FILESZ_HEADER);
    int total_frg_count = this->fragmenter.GetFragmentCount();
    string num(to_string(total_frg_count));
    file_send_header.append(num);

    cout << "WorkerFacade#Sending number of fragments:" << num << endl;
    worker_socket.SendPacket((void *) file_send_header.c_str(), (unsigned int) file_send_header.length());

    int fail_count = 0;
    int pack_seq_num = 0;

    cout << "Fragment count:" << total_frg_count << endl;

    for (int frg_num = 0; pack_seq_num < total_frg_count && is_working && (fail_count < MAX_FAIL_COUNT); frg_num++) {

        void **buf_array = (void **) calloc((size_t) WND_SZ, sizeof(void *));
        DataPacket *wnd_pckts[WND_SZ];

        int wnd_frg_count = 0;

        // Create fragments and load them in the data packets array
        for (; wnd_frg_count < WND_SZ; ++wnd_frg_count) {

            int frag_size = fragmenter.GetNextFragmentSize();
            if (fragmenter.EndOfFile())break;

            (buf_array[wnd_frg_count]) = calloc((size_t) frag_size, sizeof(char));

            if (frag_size < 1) {
                cerr << "Invalid fragment size" << endl;
                break;
            }

            fragmenter.NextFragment(&(buf_array[wnd_frg_count]));

            cout << "Frag create #" << (pack_seq_num) << endl;

            // TODO watch for pack_seq_num overflow
            wnd_pckts[wnd_frg_count] = new DataPacket(
                    (buf_array[wnd_frg_count]),
                    (unsigned short) frag_size,
                    (unsigned int) (pack_seq_num++)
            );
            free((buf_array[wnd_frg_count]));
        }

        if (!GoBackN(wnd_frg_count, wnd_pckts, total_frg_count)) {
            cout << "GBN failed" << endl;
            free(buf_array);
            return;
        }
        // TODO FIX ME extra loops occur, loop condition is ill
        //cout << "FRG CNT:" << wnd_frg_count << endl;



        free(buf_array);
    }

    if (!EndTransmission(total_frg_count)) {
        cerr << "Err in receiving final ack" << endl;
    } else {
        cout << "Transmission completed." << endl;
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

bool WorkerFacade::EndTransmission(int total_frag_count)
{
    // End transmission
    DataPacket trans_end(NULL, 0, (unsigned int) total_frag_count);

    worker_socket.SendDataPacket(&trans_end);
    std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Wait for packet to be sent

    AckPacket final_ack;
    if (worker_socket.ReceiveAckPacket(&final_ack)) {
        cout << "Final ack num [" << final_ack.ack_num << "] total frag count " << total_frag_count << endl;
    } else {
        cerr << "Failed to receive final ack" << endl;
    }
    return final_ack.ack_num == total_frag_count;
}

bool WorkerFacade::SendWindow(DataPacket *pck_arr_ptr[], int frg_count)
{

    // Send all fragments
    for (int k = 0; k < frg_count; ++k) {

        // PLP Path loss probability
        if (will_be_sent()) {
            worker_socket.SendDataPacket(pck_arr_ptr[k]);
        } else {
            cout << "Dropped packet [" << pck_arr_ptr[k]->seqno << "]" << endl;
        }

//        worker_socket.SendDataPacket(pck_arr_ptr[k]);


        std::this_thread::sleep_for(std::chrono::milliseconds(3)); // Wait for packet to be sent
    }
    return false;
}

void WorkerFacade::DeleteWindow(DataPacket *pck_arr_ptr[], int frg_count)
{
    for (int k = 0; k < frg_count; ++k) {
        delete pck_arr_ptr[k];  // FIXME this should be deleted after the window was successfully sent
    }
}

bool WorkerFacade::GoBackN(int wnd_frg_count, DataPacket **pck_arr_ptr, int file_frg_count)
{
    // NORMAL MODE
//    SendWindow(pck_arr_ptr, wnd_frg_count);
//    for (int l = 0; l < wnd_frg_count; ++l) {
//        AckPacket ack;
//        worker_socket.ReceiveAckPacket(&ack);
//        cout << "Ack:" << ack.ack_num << endl;
//    }

    if (wnd_frg_count < 1) {
        cout << "Empty window" << endl;
        return true;
    }

    int wind_base_id = this->last_acked_pkt_id + 1;
    // las acked + 1 + count - 1
    int wind_last_pck_id = this->last_acked_pkt_id + wnd_frg_count;

    int acknum = 0;
    int fail = 0;

    // GBN
    cout << endl
         << "Startimg GBN, Wind Base: " << wind_base_id
         << " Fragments in window: " << wnd_frg_count
         << " Window end:" << wind_last_pck_id
         << endl;

    SendWindow(pck_arr_ptr, wnd_frg_count);


    // Collect the sent window ACKs
    while (this->last_acked_pkt_id < wind_last_pck_id
           && this->last_acked_pkt_id < (file_frg_count - 1)) {


        AckPacket ack;

        if (!worker_socket.ReceiveAckPacket(&ack)) {
            // Failed to receive
            if (fail++ == WND_SZ * MAX_FAIL_COUNT) {
                cerr << "Client died?" << endl;
                return false;
            }
            cout << "Fail ACK receive timeout, Resend window" << endl;
            SendWindow(pck_arr_ptr, wnd_frg_count);
            continue;
        }

        acknum = ack.ack_num;

        if (acknum == (this->last_acked_pkt_id + 1)) {

            // Slide the window
            cout << "Correct ACK received [" << acknum << "]" << endl;
            this->last_acked_pkt_id++;

        } else {

            cout << "ACK Invalid Received [" << acknum << "]"
                 << " Expected [" << (this->last_acked_pkt_id + 1) << "]"
                 << endl;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(200));  // Take it easy
    }

    /// IMPORTANT Delete the sent window
    DeleteWindow(pck_arr_ptr, wnd_frg_count);
    cout << "Window released" << endl << endl;
    return true;
}
