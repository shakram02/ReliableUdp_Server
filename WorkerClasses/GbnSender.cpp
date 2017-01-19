//
// Created by ahmed on 1/15/17.
//

#include "GbnSender.h"

GbnSender::GbnSender(string file_name,
        AddressInfo client_endpoint,
        unique_ptr<RawUdpSocket> send_socket) :
        AbstractSender(file_name, client_endpoint, send_socket)
{}

bool GbnSender::SendFile()
{

    return false;
}

void GbnSender::StopSending(FileTransferState &state)
{

}
