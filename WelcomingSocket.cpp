//
// Created by ahmed on 12/17/16.
//



#include "WelcomingSocket.h"
#include "ClientObserver.h"
#include "server_config.h"


#define BUFF_LEN 256

#define HANDSHAKE "hndshk"
#define WELCOME_BUFFLEN strlen(HANDSHAKE)+1

// TODO remove this later
void log_error(const char *func_name)
{
    fprintf(stderr, "%s:%s\n", func_name, strerror(errno));
}

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
        string buf = this->main_sock->ReceiveString(client_main_socket);
        // Check validity of handshake message
        if (buf != string(HANDSHAKE)) {
            // Ignore unwanted handshake
            cout << "Ignored suspicious client" << endl;
            continue;
        }

        // Redirect the client with the new socket
        unique_ptr<RawUdpSocket> redirect_socket = CreateClientSocket(client_main_socket.ip);
        unsigned short redirect_port = redirect_socket->GetPortNumber();

        // Send redirect message using the main socket
        string redirect_msg = string("redirect") + to_string(redirect_port);
        main_sock->SendString(client_main_socket, redirect_msg);

        // Wait for client to redirect
        std::this_thread::sleep_for(std::chrono::milliseconds(TIMEO_REDIRECT_MILLIS));

        AddressInfo client_file_socket;
        // Update client info when it sends the redirection confirmation message
        if (redirect_socket->ReceiveString(client_file_socket) == string(REDIRECT_SUCCESS)) {
            // TODO start thread here ?
            cout << "Client file socket port:" << client_file_socket.port_number << endl;
            string assert_redirection(SERV_REDIRECT_OK);
            redirect_socket->SendString(client_file_socket, assert_redirection);

            ClientObserver::NotifyForClient(redirect_socket.release(), client_file_socket);
        } else {
            cout << "Fail to redirect client" << endl;
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
