//
// Created by ahmed on 12/21/16.
//

#include <arpa/inet.h>
#include "WorkerSocket.h"
#include "../globaldef.h"


WorkerSocket::WorkerSocket(int client_sockfd)
{
    this->socket_fd = client_sockfd;

    if (AssertRedirection()) {
        cout << "Client redirected successfully" << endl;
        string confirmation_msg_str(SERV_REDIRECT_OK);

        int len = sizeof((this->client_addr));

        // TODO use packets for all communication
        // TODO send packet headers only in case of ACKS
        ByteVector confirmation_msg(confirmation_msg_str.begin(), confirmation_msg_str.end());
        SendPacket(confirmation_msg.data(), (unsigned int) confirmation_msg.size());

        this->is_serving = true;

    } else {
        cerr << "Client failed to redirect" << endl;
        cout << endl;
        this->is_serving = false;
        close(socket_fd);
    }

    cout << "WorkerFacade#Client IP: " << inet_ntoa((this->client_addr).sin_addr)
         << " Port:" << ntohs((this->client_addr).sin_port)
         << endl;
}


bool WorkerSocket::AssertRedirection()
{

    char buf[(int) strlen(REDIRECT_SUCCESS) + 1] = {0};
    socklen_t len = sizeof(struct sockaddr_in);

    recvfrom(this->socket_fd, buf, sizeof(buf), 0, (sockaddr *) &(this->client_addr), &len);
    cout << "WorkerFacade#Redirect Message:" << buf << endl;

    return string(buf) == string(REDIRECT_SUCCESS);
}

WorkerSocket::~WorkerSocket()
{
    if (this->is_serving) {
        cout << "WorkerFacade disposed" << endl;
        this->is_serving = false;
        close(this->socket_fd);
    }
}

string WorkerSocket::GetRequestedFile()
{
    char buf[MAX_FILE_PATH_LENGTH + 1] = {0};
    socklen_t len = sizeof(struct sockaddr_in);

    // TODO refactor
    recvfrom(this->socket_fd, buf, sizeof(buf), 0, (sockaddr *) &(this->client_addr), &len);

    string prefix("FILE-");
    string client_request(buf);

    if (client_request.compare(0, prefix.size(), prefix)) {
        cerr << "Invalid file request packet" << endl;
        return string(buf);
    } else {
        string file_name = client_request.substr(client_request.find("-") + 1, client_request.size() - 1);
        cout << "Requested file name:" << file_name << endl;
        return file_name;
    }
}

void WorkerSocket::SendPacket(const byte *data, unsigned int length)
{
    long int num_bytes = sendto(this->socket_fd,
            data, length,
            0, (sockaddr *) &(this->client_addr), (socklen_t) (sizeof(this->client_addr)));

    // Udp sendto fails when no space to allocate the buffer is avialable
    if (num_bytes < 1) {
        cerr << "Failed to send UDP packet" << endl;
        cout << endl;
    }
}

bool WorkerSocket::ReceiveRawPacket(unsigned int buffer_size, byte **data, int *received_size)
{
    socklen_t len = sizeof(struct sockaddr_in);

    (*received_size) = (int) recvfrom(this->socket_fd,
            (*data), buffer_size,
            0, (sockaddr *) &(this->client_addr), &len);

    if ((*received_size) == 0) {
        log_error("client closed connection");
        return false;
    } else if ((*received_size) == -1) {
        log_error("recvfrom");
        return false;
    }
    return true;
}

bool WorkerSocket::ReceiveAckPacket(unique_ptr<Packet> &ack_packet_ptr)
{
    int size = 0;
    int packetHeaderSize = PacketHeader::Size();

    // C++ --> C
    byte *data = (byte *) calloc(1, (size_t) packetHeaderSize);

    bool result = ReceiveRawPacket((unsigned int) packetHeaderSize, &data, &size);

    if (!result) {
        cerr << "Failed to receive the data packet" << endl;
    }

    if (result && size != packetHeaderSize) {
        cerr << "Illegal state, received corrupt ACK packet" << endl;
    }

    if (result && size == packetHeaderSize) {

        // TODO investigate
        ack_packet_ptr = unique_ptr<Packet>(Packet::Create(data, (unsigned short) size));
    }
    free(data);

    return result;
}

// FIXME
void WorkerSocket::SendPacket(Packet &packet)
{
    // FIXME C++ to C transition
    unique_ptr<ByteVector> bytes = packet.Serialize();

    // sizeof(DataPacket) will return a size with the full array of 128 chars,
    // on the receiver size, the size will be re-fit using the length field
    SendPacket(bytes->data(), (unsigned int) bytes->size());
}

