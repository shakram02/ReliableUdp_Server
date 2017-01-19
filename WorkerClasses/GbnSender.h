//
// Created by ahmed on 1/15/17.
//

#ifndef ENHANCEDUDPSERVER_GBNSENDER_H
#define ENHANCEDUDPSERVER_GBNSENDER_H


#include "../AbstractSender.h"

class GbnSender : protected AbstractSender
{
    /**
     // TODO refactor the file fragmenter into its own layer of the application ?
     */
public:
    GbnSender(string file_name, AddressInfo client_endpoint, unique_ptr<RawUdpSocket> send_socket);

    bool SendFile() override;

    void StopSending(FileTransferState &state) override;
};


#endif //ENHANCEDUDPSERVER_GBNSENDER_H
