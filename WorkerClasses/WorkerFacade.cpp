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

WorkerFacade::WorkerFacade(RawUdpSocket *redirect_socket, AddressInfo client_info)
{
    // Transfer ownership to the unique_ptr
    this->worker_socket = unique_ptr<RawUdpSocket>(redirect_socket);

    this->client_info = unique_ptr<AddressInfo>(new AddressInfo(client_info));

    string buf = this->worker_socket->ReceiveString();
    string file_name;
    string prefix("FILE-");

    if (buf.compare(0, prefix.size(), prefix)) {
        cerr << "Invalid file request packet" << buf << endl;
    } else {
        file_name = buf.substr(buf.find("-") + 1, buf.size() - 1);
        cout << "Requested file name:" << file_name << endl;
    }

    this->fragmenter.SetFragmentSize(DATA_FRAGMENT_SIZE);

    if (this->fragmenter.SetFilePath(file_name)) {
        this->is_working = true;
    } else {
        // TODO send fail to client

        cerr << "Worker#Failed to set the file path" << endl;
    }
}

void WorkerFacade::StartWorking()
{
//    // TODO reply with not found
//    // TODO Create GbnSender class
//    // TODO 2D fragments array from the fragmenter
//    // TODO Checksum

    basic_string<char> file_send_header(SERV_FILESZ_HEADER);
    int total_frg_count = this->fragmenter.GetFragmentCount();
    string num(to_string(total_frg_count));
    file_send_header.append(num);

    cout << "WorkerFacade#Sending number of fragments:" << file_send_header << endl;
    worker_socket->SendString(*(this->client_info), file_send_header);

    int pack_seq_num = 0, wnd_arr_idx = 0;
    // TODO this loop is huge
    // TODO separate the GBN sender
    while (pack_seq_num < total_frg_count) {

        unique_ptr<Packet> wnd_pckts[WND_SIZE];   // TODO move window size to lib config

        // Create window packets
        for (wnd_arr_idx = 0; wnd_arr_idx < WND_SIZE; ++wnd_arr_idx) {

            int frag_size = fragmenter.GetNextFragmentSize();
            if (fragmenter.EndOfFile())break;

            if (frag_size < 1) {
                cerr << "Invalid fragment size" << endl;
                break;
            }

            // TODO watch for pack_seq_num overflow
            unique_ptr<ByteVector> temp = fragmenter.NextFragment();
            wnd_pckts[wnd_arr_idx] = unique_ptr<Packet>(new Packet(temp, (unsigned int) (pack_seq_num++)));
        }

        // TODO replace with generic sender function
        if (!GoBackN(wnd_arr_idx, wnd_pckts, total_frg_count)) {
            cout << "GBN failed" << endl;
            return;
        }
    }

    // TODO move this to the generic sender class
    if (!EndTransmission(total_frg_count)) {
        cerr << "Err in receiving final ack" << endl;
    } else {
        cout << "Transmission completed." << endl;
    }
}

void WorkerFacade::StopWorking()
{
    this->is_working = false;
}


bool WorkerFacade::EndTransmission(int total_frag_count)
{

    unique_ptr<ByteVector> placeholder = nullptr;
    unique_ptr<Packet> trans_end =
            unique_ptr<Packet>(new Packet(placeholder, (unsigned int) total_frag_count));

    worker_socket->SendPacket(*client_info, trans_end);
    unique_ptr<Packet> final_ack;
    if (worker_socket->ReceivePacket(final_ack)) {
        cout << "Final ack num [" << final_ack->header->seqno << "] total frag count " << total_frag_count << endl;
        return final_ack->header->seqno == total_frag_count && final_ack->header->dataLen == 0;
    } else {
        cerr << "Failed to receive final ack" << endl;
        return false;
    }
}

void WorkerFacade::SendWindow(unique_ptr<Packet> pck_arr_ptr[], int frg_count)
{

    for (int k = 0; k < frg_count; ++k) {

        // PLP Path loss probability
        if (will_be_sent()) {
            worker_socket->SendPacket(*client_info, pck_arr_ptr[k]);
        } else {
            cout << "Dropped packet [" << pck_arr_ptr[k]->header->seqno << "]" << endl;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(PCKT_SLEEP)); // Wait for packet to be sent
    }

}

bool WorkerFacade::GoBackN(int wnd_frg_count, unique_ptr<Packet> pck_arr_ptr[], int file_frg_count)
{

    if (wnd_frg_count < 1) {
        cout << "Empty window" << endl;
        return true;
    }

    int wind_last_pck_id = this->last_acked_pkt_id + wnd_frg_count;

    int acknum = 0;
    int fail = 0;


//    cout << endl
//         << " Fragments in window: " << wnd_frg_count
//         << " Window end:" << wind_last_pck_id
//         << endl;

    SendWindow(pck_arr_ptr, wnd_frg_count);

    // Collect the sent window ACKs
    while (this->last_acked_pkt_id < wind_last_pck_id
           && this->last_acked_pkt_id < (file_frg_count - 1)) {


        unique_ptr<Packet> ack;

        if (!worker_socket->ReceivePacket(ack)) {
            // Failed to receive
            if (fail++ == WND_SIZE * MAX_FAIL_COUNT) {
                cerr << "Client died?" << endl;
                return false;
            }
            cout << "Fail ACK receive timeout, Resend window"
                 << ", lost windows [" << ++this->lost_winds << "] \r" << endl;
            SendWindow(pck_arr_ptr, wnd_frg_count);
            continue;
        }

        acknum = ack->header->seqno;

        if (acknum == (this->last_acked_pkt_id + 1)) {

            // Slide the window
            // cout << "Correct ACK received [" << acknum << "]" << endl;
            this->last_acked_pkt_id++;

        } else {

            cout << "ACK Invalid Received [" << acknum << "]"
                 << " Expected [" << (this->last_acked_pkt_id + 1) << "]"
                 << endl;
        }
    }
    return true;
}

WorkerFacade::~WorkerFacade()
{}