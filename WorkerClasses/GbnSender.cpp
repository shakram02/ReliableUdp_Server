//
// Created by ahmed on 12/24/16.
//

#include <DataPacket.h>
#include <BinarySerializer.h>
#include "GbnSender.h"

typedef std::pair<void *, unsigned short> PacketInfo;

GbnSender::GbnSender(uint32_t window_sz,
        WorkerSocket &send_sock, uint32_t sock_header_len) : worker_sock(send_sock)
{
    this->window_size = window_sz;
    this->confirmed = 0;
    this->sock_header_length = sock_header_len;
}

bool GbnSender::AddToSendQueue(DataPacket &packet)
{
    if (packet.seqno > (this->confirmed + this->window_size)) {
        // out of bound seq. num
    }

    // Trying to send out of window size
    if (send_vector.size() >= this->window_size)return false;

    void *raw;

    /*
     * Unlike a pointer, once a reference is bound to an object, it can not be "reseated" to another object.
     * The reference itself isn't an object (it has no identity;
     * taking the address of a reference gives you the address of the referent;
     * remember: the reference is its referent).
     */
    BinarySerializer::SerializeDataPacket(&packet, &raw);

    PacketInfo packet_info(raw, packet.seqno);

    // The length field of a packet includes all the fields of the packet
    this->send_vector.push_back(packet_info);
}

void GbnSender::SendWindow()
{
    for (PacketInfo i:this->send_vector) {
        this->worker_sock.SendPacket(i.first, sizeof(DataPacket));
    }
}