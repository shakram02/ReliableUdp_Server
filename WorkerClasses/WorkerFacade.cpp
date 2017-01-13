//
// Created by ahmed on 12/23/16.
//

#include <cstring>
#include <thread>
#include "WorkerFacade.h"

#include "../server_config.h"


extern "C"
{
#include "plp.h"
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
    worker_socket.SendPacket((byte *) file_send_header.c_str(), (unsigned int) file_send_header.length());

    int pack_seq_num = 0, wnd_arr_idx = 0;

    cout << "Fragment count:" << total_frg_count << endl;

    unique_ptr<ByteVector> buf_array[total_frg_count];

    // TODO this loop is huge
    // TODO separate the GBN sender
    while (pack_seq_num < total_frg_count) {

        unique_ptr<Packet> wnd_pckts[WND_SZ];   // TODO move window size to lib config

        // Create fragments and load them in the data packets array
        for (wnd_arr_idx = 0; wnd_arr_idx < WND_SZ; ++wnd_arr_idx) {

            int frag_size = fragmenter.GetNextFragmentSize();
            if (fragmenter.EndOfFile())break;


            if (frag_size < 1) {
                cerr << "Invalid fragment size" << endl;
                break;
            }

            buf_array[wnd_arr_idx] = fragmenter.NextFragment();

            // TODO watch for pack_seq_num overflow
            wnd_pckts[wnd_arr_idx] = unique_ptr<Packet>(new Packet(
                    buf_array[wnd_arr_idx],
                    (unsigned int) (pack_seq_num++)
            ));
        }

        if (!GoBackN(wnd_arr_idx, wnd_pckts, total_frg_count)) {
            cout << "GBN failed" << endl;
            return;
        }
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

    this->fragmenter.SetFragmentSize(DATA_FRAGMENT_SIZE);

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
    unique_ptr<ByteVector> placeholder = nullptr;
    Packet trans_end(placeholder, (unsigned int) total_frag_count);

    worker_socket.SendPacket(trans_end);
    std::this_thread::sleep_for(std::chrono::microseconds(PCKT_SLEEP)); // Wait for packet to be sent

    unique_ptr<Packet> final_ack;
    if (worker_socket.ReceiveAckPacket(final_ack)) {
        cout << "Final ack num [" << final_ack->header->seqno << "] total frag count " << total_frag_count << endl;
        return final_ack->header->seqno == total_frag_count;
    } else {
        cerr << "Failed to receive final ack" << endl;
        return false;
    }
}

void WorkerFacade::SendWindow(unique_ptr<Packet> pck_arr_ptr[], int frg_count)
{
    // Send all fragments
    for (int k = 0; k < frg_count; ++k) {

        // PLP Path loss probability
        if (will_be_sent()) {
            worker_socket.SendPacket(*(pck_arr_ptr[k]));
        } else {
            cout << "Dropped packet [" << pck_arr_ptr[k]->header->seqno << "]" << endl;
        }

    }
    std::this_thread::sleep_for(std::chrono::microseconds(PCKT_SLEEP)); // Wait for packet to be sent
}

bool WorkerFacade::GoBackN(int wnd_frg_count, unique_ptr<Packet> pck_arr_ptr[], int file_frg_count)
{
    if (wnd_frg_count < 1) {
        cout << "Empty window" << endl;
        return true;
    }

    int wind_base_id = this->last_acked_pkt_id + 1;
    int wind_last_pck_id = this->last_acked_pkt_id + wnd_frg_count;

    int acknum = 0;
    int fail = 0;

    // GBN
//    cout << endl
//         << "Startimg GBN, Wind Base: " << wind_base_id
//         << " Fragments in window: " << wnd_frg_count
//         << " Window end:" << wind_last_pck_id
//         << endl;

    SendWindow(pck_arr_ptr, wnd_frg_count);


    // Collect the sent window ACKs
    while (this->last_acked_pkt_id < wind_last_pck_id
           && this->last_acked_pkt_id < (file_frg_count - 1)) {


        unique_ptr<Packet> ack;

        if (!worker_socket.ReceiveAckPacket(ack)) {
            // Failed to receive
            if (fail++ == WND_SZ * MAX_FAIL_COUNT) {
                cerr << "Client died?" << endl;
                return false;
            }
            cout << "Fail ACK receive timeout, Resend window" << endl;
            SendWindow(pck_arr_ptr, wnd_frg_count);
            continue;
        }

        acknum = ack->header->seqno;

        if (acknum == (this->last_acked_pkt_id + 1)) {

            // Slide the window
            cout << "Correct ACK received [" << acknum << "]" << endl;
            this->last_acked_pkt_id++;

        } else {

            cout << "ACK Invalid Received [" << acknum << "]"
                 << " Expected [" << (this->last_acked_pkt_id + 1) << "]"
                 << endl;
        }
    }

    //cout << "Window released" << endl << endl;
    return true;
}
