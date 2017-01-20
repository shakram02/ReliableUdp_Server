//
// Created by ahmed on 1/15/17.
//

#ifndef ENHANCEDUDPSERVER_GBNSENDER_H
#define ENHANCEDUDPSERVER_GBNSENDER_H


#include "../AbstractSender.h"

class GbnSender : public AbstractSender
{
protected:

public:
    GbnSender(AddressInfo client_endpoint, unique_ptr<RawUdpSocket> send_socket);

    bool SendWindow(PacketList &window) override;
};


#endif //ENHANCEDUDPSERVER_GBNSENDER_H
