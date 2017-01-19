//
// Created by ahmed on 1/15/17.
//

#ifndef ENHANCEDUDPSERVER_ABSTRACTSENDER_H
#define ENHANCEDUDPSERVER_ABSTRACTSENDER_H


#include <RawUdpSocket.h>
#include <FileTransferState.h>
#include <atomic>
#include "WorkerClasses/FileFragmenter.h"
#include<list>
#include <thread>

typedef std::list<unique_ptr<Packet>> PacketList;

class AbstractSender
{
protected:
    unique_ptr<RawUdpSocket> file_transfer_socket;
    AddressInfo client;
    int last_acked_pkt_id = -1;
    int lost_winds = 0;

    AbstractSender(AddressInfo client_endpoint,
            unique_ptr<RawUdpSocket> &send_socket);


    /**
     * Puts a window to be sent on the UDP port
     * @param window Window to be sent on port
     */
    inline void TransmitWindow(PacketList &window);

    /**
     * Created by Helana on 12/23/16.
     * Calculates the probability that will determine if a packet will be lost
     * @return whether a packet will be lost
     */
    int WillBeSent();

public:

    virtual ~AbstractSender()
    {};

    virtual bool SendWindow(PacketList &window)=0;

    /**
     * Tells the client that the transmission has ended
     * @param total_frag_count Count of sent fragments
     * @return whether the client ACKed file transfer complete
     */
    bool CloseTransmission(int total_frag_count);

    FileTransferState GetCurrentState();
};

void AbstractSender::TransmitWindow(PacketList &window)
{

    for (auto k = window.begin(); k != window.end(); ++k) {

        // PLP Path loss probability
        if (WillBeSent()) {
            // TODO is this non-blocking ?
            this->file_transfer_socket->SendPacket(this->client, *k);
        } else {
            cout << "Dropped packet [" << (*k)->header->seqno << "]" << endl;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(PCKT_SLEEP)); // Wait for packet to be sent
    }
}

#endif //ENHANCEDUDPSERVER_ABSTRACTSENDER_H