//
// Created by ahmed on 12/23/16.
//

#include "WorkerFacade.h"
#include <cstring>
#include <thread>
#include "../server_config.h"
#include "GbnSender.h"
#include <ProtocolDef.h>


WorkerFacade::WorkerFacade(RawUdpSocket *redirect_socket, AddressInfo client_info)
{
    // Transfer ownership to the unique_ptr
    this->worker_socket = unique_ptr<RawUdpSocket>(redirect_socket);

    this->client_info = unique_ptr<AddressInfo>(new AddressInfo(client_info));

    string buf;
    if (this->worker_socket->ReceiveStringPacket(buf) != ID_FILE_NAME_CLNT) {
        // TODO error, bad file name
        throw std::runtime_error("Bad file name requested");
    }
    string file_name;
    string prefix(FILE_NAME_CLNT);

    if (buf.compare(0, prefix.size(), prefix)) {
        cerr << "Invalid file request packet" << buf << endl;
        return;
    } else {
        file_name = buf.substr(buf.find("-") + 1, buf.size() - 1);
        cout << "Requested file name:" << file_name << endl;
    }

    if (this->fragmenter.SetFilePath(file_name)) {
        this->is_working = true;
    } else {
        // TODO send fail to client
        // TODO reply with not found
        cerr << "Worker#Failed to set the file path" << endl;
    }
}

void WorkerFacade::StopWorking()
{
    this->is_working = false;
}

void WorkerFacade::StartWorking()
{

//    // TODO Create GbnSender class
//    // TODO 2D fragments array from the fragmenter
//    // TODO Checksum

    basic_string<char> file_send_header(FILE_SZ_SRV);
    int total_frg_count = this->fragmenter.GetFragmentCount();
    string num(to_string(total_frg_count));
    file_send_header.append(num);

    cout << "WorkerFacade#Sending number of fragments:" << file_send_header << endl;
    worker_socket->SendStringPacket(*(this->client_info), file_send_header, ID_FILE_SZ_SRV);

    int pack_seq_num = 0, i = 0;

    PacketList wnd_pckts;   // TODO move window size to lib config

    GbnSender sender(*(this->client_info), std::move(worker_socket));

    // TODO separate the GBN sender
    while (pack_seq_num < total_frg_count) {

        // Create window packets
        for (i = 0; i < WND_SIZE; ++i) {

            if (fragmenter.EndOfFile())break;

            // TODO watch for pack_seq_num overflow
            unique_ptr<ByteVector> temp = fragmenter.NextFragment();
            wnd_pckts.push_back(unique_ptr<Packet>(new Packet(temp, (unsigned int) (pack_seq_num++))));
        }

        // TODO replace with generic sender function
        if (sender.SendWindow(wnd_pckts)) { continue; }

        cout << "GBN failed" << endl;
        this->is_working = false;
        return;

//        if (!this->is_working) {
        // TODO Pause / Stop sending packets
        // TODO save the state
//        }
    }

    // TODO move this to the generic sender class
    // TODO if final ack rcv fails, wait till it's send and fail on max fail count,
    // send an ack back to client
    if (sender.CloseTransmission(total_frg_count)) {
        cout << "Transmission completed." << endl;
    } else {
        cerr << "Failed to receive final ack" << endl;
    }
}

WorkerFacade::~WorkerFacade()
{}