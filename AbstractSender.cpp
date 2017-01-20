//
// Created by ahmed on 1/19/17.
//

#include "AbstractSender.h"
#include "server_config.h"

AbstractSender::AbstractSender(AddressInfo client_endpoint,
        unique_ptr<RawUdpSocket> &send_socket) :
        client(client_endpoint), file_transfer_socket(std::move(send_socket))
{}

bool AbstractSender::CloseTransmission(int total_frag_count)
{
    unique_ptr<Packet> trans_end = unique_ptr<Packet>(new Packet((unsigned int) total_frag_count));

    this->file_transfer_socket->SendPacket(client, trans_end);
    unique_ptr<Packet> final_ack;

    if (this->file_transfer_socket->ReceivePacket(final_ack)) {
        // TODO repeat send
        cout << "Final ack num [" << final_ack->header->seqno << "] total frag count " << total_frag_count << endl;
        return final_ack->header->seqno == total_frag_count && final_ack->header->dataLen == 0;
    }
    return false;

}

FileTransferState AbstractSender::GetCurrentState()
{
    return FileTransferState(this->last_acked_pkt_id);
}


int AbstractSender::WillBeSent()
{
    return ((float) rand() / (float) (RAND_MAX)) < LOSS_THRESH;
}
