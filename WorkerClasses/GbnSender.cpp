////
//// Created by ahmed on 12/24/16.
////
//
//#include <DataPacket.h>
//#include <BinarySerializer.h>
//#include "GbnSender.h"
//#include <chrono>
//#include <thread>
//
//
//#include "../globaldef.h"
//
//typedef std::pair<void *, unsigned short> PacketInfo;
//
//GbnSender::GbnSender(uint32_t window_sz,
//        WorkerSocket &send_sock) : worker_sock(send_sock)
//{
//    this->window_size = window_sz;
//    this->base_seq_num = 0;
//}
//
//bool GbnSender::AddToSendQueue(DataPacket packet)
//{
//    if (packet.seqno > (this->base_seq_num + this->window_size)) {
//        // out of bound seq. num
//    }
//
//    // Trying to send out of window size
//    if (send_vector.size() >= this->window_size) {
//        cerr << "Send queue is now bigger than window size, call receive" << endl;
//        return false;
//    }
//
//    //ProjectionPointer raw;
//    //BinarySerializer::SerializeDataPacket(&packet, &raw);
//
//    //cout << "Sending:" << packet.data << endl;
//
//
//    // 16 is the number of chars in the char array
//    void *temp_free_on_send = calloc(16, sizeof(char)); // FIXME LEAKING
//    memcpy(temp_free_on_send, packet.data, 16);
//
//    cout << "RWX:" << (char *) temp_free_on_send << endl;
//
//    PacketInfo packet_info(temp_free_on_send, packet.seqno);
//
//    // The length field of a packet includes all the fields of the packet
//    this->send_vector.push_back(packet_info);
//}
//
//void GbnSender::SendWindow()
//{
//    // Read the worker_socket response(s)
//    // ACK -> WillAdvance = true
//    // NACK -> WillAdvance = false
//
//    for (PacketInfo i:this->send_vector) {
//        cout << "Sending:" << (char *) i.first << endl;
//
//        this->worker_sock.SendPacket(i.first, sizeof(DataPacket));
//
//        //free(i.first);
//        this_thread::sleep_for(chrono::milliseconds(SLEEP_MILLIS));
//    }
//}
//
//bool GbnSender::ReceiveWindow()
//{
//    int count = (int) this->send_vector.size();
//    /**
//     * Needs to run on separate thread with the send
//     */
//    for (int i = 0; i < count; i++) {
//
//        AckPacket ack;
//
//        if (!worker_sock.ReceiveAckPacket(&ack)) {
//            // TODO alert
//            cerr << "Failed to ack window" << endl;
//            return false;
//        }
//        cout << "Received Window ack:" << ack.ack_num << endl;
//    }
//
//    //boost::this_thread::interruption_point();
//    this->send_vector.clear();
//
//    return false;
//}
//
//
