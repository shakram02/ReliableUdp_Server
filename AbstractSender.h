//
// Created by ahmed on 1/15/17.
//

#ifndef ENHANCEDUDPSERVER_ABSTRACTSENDER_H
#define ENHANCEDUDPSERVER_ABSTRACTSENDER_H


#include <RawUdpSocket.h>
#include <FileTransferState.h>
#include <atomic>
#include "WorkerClasses/FileFragmenter.h"

class AbstractSender
{
protected:
    unique_ptr<RawUdpSocket> file_transfer_socket;
    unique_ptr<FileFragmenter> reader;
    std::atomic<bool> is_sending;
    AddressInfo client;

    AbstractSender(string file_name, AddressInfo client_endpoint,
            unique_ptr<RawUdpSocket> &send_socket) :
            client(client_endpoint), file_transfer_socket(std::move(send_socket))
    { this->reader = unique_ptr<FileFragmenter>(new FileFragmenter(file_name)); }

public:

    virtual ~AbstractSender()
    {};

    virtual bool SendFile()=0;

    virtual void StopSending(FileTransferState &state)=0;
};


#endif //ENHANCEDUDPSERVER_ABSTRACTSENDER_H
