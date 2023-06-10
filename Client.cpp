#include "Client.hpp"

Client::Client(int socket, sockaddr_in client_addr)
{
    this->socket = socket;
    this->addr = client_addr;
    this->grade = GUEST;
}

Client::~Client()
{
}