//
// Created by ahmed on 12/17/16.
//



#include "WelcomingSocket.h"
#include "ClientMediator.h"
#include "server_config.h"
#include <ProtocolMsgId.h>

WelcomingSocket::WelcomingSocket(string &serverIp, unsigned const short portNumber)
{
    this->is_receiving = false;

    AddressInfo info(serverIp, portNumber);
    RawUdpSocket *sockptr = new RawUdpSocket(info);
    sockptr->Bind();

    this->main_sock = unique_ptr<RawUdpSocket>(sockptr);

    cout << "Port:" << this->main_sock->GetPortNumber() << endl;
    cout << "IP:" << this->main_sock->GetIpAdress() << endl;
}

void WelcomingSocket::StartReceiving()
{
    this->is_receiving = true;
    // TODO operate this on a separate thread
    while (this->is_receiving) {


        // TODO move this to a separate function
        AddressInfo client_main_socket;
        string buf;
        int seqno = this->main_sock->ReceiveStringPacket(client_main_socket, buf);

        cout << "MSG SEQNO:" << seqno << endl;
        // Check validity of handshake message
        if (seqno != ID_HAND_SHAKE) {
            // Ignore unwanted handshake
            cout << "Ignored suspicious client" << endl;
            continue;
        }

        // Redirect the client with the new socket
        unique_ptr<RawUdpSocket> redirect_socket = CreateClientSocket(client_main_socket.ip);
        unsigned short redirect_port = redirect_socket->GetPortNumber();

        // Send redirect message using the main socket
        string redirect_msg = string("redirect") + to_string(redirect_port);
        main_sock->SendStringPacket(client_main_socket, redirect_msg, ID_GO_REDIRECT);

        // Wait for client to redirect
        std::this_thread::sleep_for(std::chrono::milliseconds(TIMEO_REDIRECT_MILLIS));

        AddressInfo client_file_socket;
        // Update client info when it sends the redirection confirmation message
        if (redirect_socket->ReceiveString(client_file_socket) == string(REDIRECT_SUCCESS)) {
            // TODO start thread here ?
            cout << "Client file socket port:" << client_file_socket.port_number << endl;
            string assert_redirection(SERV_REDIRECT_OK);
            redirect_socket->SendString(client_file_socket, assert_redirection);

            ClientMediator::NotifyForClient(redirect_socket.release(), client_file_socket);
        } else {
            cerr << "Fail to redirect client" << endl;
        }
        cout << endl;
    }
    cout << endl << "Stopped listening for ppl" << endl;
}

unique_ptr<RawUdpSocket> WelcomingSocket::CreateClientSocket(string &client_ip)
{
    AddressInfo redirect_socket_info(client_ip, 0);
    RawUdpSocket *redirect_socket = new RawUdpSocket(redirect_socket_info);

    redirect_socket->Bind();
    redirect_socket->SetReceiveTimeout(TIMEO_SEC, TIMEO_MICRO_SEC);
    redirect_socket->SetSendTimeout(TIMEO_SEC, TIMEO_MICRO_SEC);

    return unique_ptr<RawUdpSocket>(redirect_socket);
}

WelcomingSocket::~WelcomingSocket()
{
    this->main_sock->~RawUdpSocket();
}

void WelcomingSocket::StopReceiving()
{
    this->is_receiving = false;
}
