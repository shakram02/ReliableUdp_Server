////
//// Created by ahmed on 12/24/16.
////
//
//#ifndef ENHANCEDUDPSERVER_GBNSENDER_H
//#define ENHANCEDUDPSERVER_GBNSENDER_H
//
//
//#include <vector>
//#include <queue>
//#include "WorkerSocket.h"
//#include <boost/thread/thread.hpp>
//#include <boost/lockfree/queue.hpp>
//
//class GbnSender
//{
//
//    int window_size;
//    int base_seq_num;
//    WorkerSocket &worker_sock;
//    //int sock_header_length;   // The header of the socket and the socket are of constant size
//
//    std::vector<std::pair<void *, unsigned short>> send_vector;
//
//public:
//    // TODO move the socket header in its own struct?
//    GbnSender(unsigned int window_sz, WorkerSocket &send_sock);
//
//    /**
//     * Adds a data packet to this window ( the length of the data is in the packet )
//     * @param packet Constructed data packet to be sent
//     * @return
//     */
//    bool AddToSendQueue(DataPacket packet);
//
//    void SendWindow();
//
//    /**
//     * Receives a sequence of packets and checks their sequence numbers
//     * if all the sequence numbers correspond to packets in the current window
//     * the current window is released and the base seq. number is advanced
//     *
//     * This function should be called on a separate thread as it's a blocking function
//     * @return
//     */
//    bool ReceiveWindow();
//
//};
//
//
//#endif //ENHANCEDUDPSERVER_GBNSENDER_H
