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

int Client::get_grade() const
{
    return this->grade;
}

void Client::set_grade(int grade)
{
    this->grade = grade;
}