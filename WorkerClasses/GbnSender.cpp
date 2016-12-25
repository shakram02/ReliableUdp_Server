//
// Created by ahmed on 12/24/16.
//

#include <DataPacket.h>
#include <BinarySerializer.h>
#include "GbnSender.h"
#include <chrono>
#include <thread>

#include "../globaldef.h"

typedef std::pair<void *, unsigned short> PacketInfo;

GbnSender::GbnSender(uint32_t window_sz,
        WorkerSocket &send_sock) : worker_sock(send_sock)
{
    this->window_size = window_sz;
    this->base_seq_num = 0;
}

bool GbnSender::AddToSendQueue(DataPacket &packet)
{
    if (packet.seqno > (this->base_seq_num + this->window_size)) {
        // out of bound seq. num
    }

    // Trying to send out of window size
    if (send_vector.size() >= this->window_size) {
        cerr << "Send queue is now bigger than window size, call receive" << endl;
        return false;
    }

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
    // Send it to the worker_socket, wait for the worker_socket response for the whole window
    // -use Select/Non blocking IO?-

    // Read the worker_socket response(s)
    // ACK -> WillAdvance = true
    // NACK -> WillAdvance = false

    for (PacketInfo i:this->send_vector) {
        this->worker_sock.SendPacket(i.first, sizeof(DataPacket));
        this_thread::sleep_for(chrono::milliseconds(SLEEP_MILLIS));
    }
}

bool GbnSender::ReceiveWindow()
{

    for (PacketInfo i:this->send_vector) {
        AckPacket ack;

        if (!worker_sock.ReceiveAckPacket(&ack)) {
            // TODO alert
            return false;
        }

        cout << "Received Window ack:" << ack.ack_num << endl;
    }

    this->send_vector.clear();

    return false;
}


