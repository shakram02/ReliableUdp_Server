//
// Created by ahmed on 1/15/17.
//

#include "GbnSender.h"


GbnSender::GbnSender(AddressInfo client_endpoint,
        unique_ptr<RawUdpSocket> send_socket) : AbstractSender(client_endpoint, send_socket)
{

}

bool GbnSender::SendWindow(PacketList &window)
{
// TODO when timer expires, retransmit all UN-ACKED packets
    // the implementation here sends also the acked packets ( the whole window )

    if (window.empty()) {
        cout << "Empty window" << endl;
        return true;
    }

    int acknum = 0;
    int fail = 0;

//    cout << endl
//         << " Fragments in window: " << wnd_frg_count
//         << " Window end:" << wind_last_pck_id
//         << endl;

    // Primary send
    TransmitWindow(window);

    while (window.size() != 0) {

        unique_ptr<Packet> ack;

        if (!file_transfer_socket->ReceivePacket(ack)) {
            // Failed to receive
            if (fail++ == WND_SIZE * MAX_FAIL_COUNT) {
                cerr << "Client died?" << endl;
                return false;
            }
            cout << "Fail ACK receive timeout, Resend window"
                 << ", lost windows [" << ++this->lost_winds << "] \r" << endl;
            TransmitWindow(window);
            continue;
        }

        acknum = ack->header->seqno;

        // TODO put in function validate received packet
        if (acknum == (this->last_acked_pkt_id + 1)) {

            // Slide the window
            // cout << "Correct ACK
            // received [" << acknum << "]" << endl;


            window.pop_front();
            this->last_acked_pkt_id++;

        } else {

            cout << "Invalid ACK Received [" << acknum << "]"
                 << " Expected [" << (this->last_acked_pkt_id + 1) << "]"
                 << endl;
        }
    }
    return true;
}
