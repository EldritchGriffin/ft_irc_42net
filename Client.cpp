#include "Client.hpp"

Client::Client(int socket, std::string name, std::string ip, int port)
{
    this->socket = socket;
    this->name = name;
    this->ip = ip;
    this->port = port;
}

Client::~Client()
{
}