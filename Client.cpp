#include "Client.hpp"
#include "Channel.hpp"

Client::Client()
{
}

Client::Client(int socket, sockaddr_in client_addr)
{
    this->socket = socket;
    this->addr = client_addr;
    this->grade = GUEST;
    this->nickname = "";
    this->username = "";
    this->realname = "";
    this->pass_state = NOPASS;
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

std::string Client::get_nickname() const
{
    return this->nickname;
}

void Client::set_nickname(std::string nickname)
{
    this->nickname = nickname;
}

std::string Client::get_username() const
{
    return this->username;
}

void Client::set_username(std::string username)
{
    this->username = username;
}

std::string Client::get_realname() const
{
    return this->realname;
}

void Client::set_realname(std::string realname)
{
    this->realname = realname;
}

int Client::get_pass_state() const
{
    return this->pass_state;
}

void Client::set_pass_state()
{
    this->pass_state = PASS;
}

int Client::get_socket() const
{
    return this->socket;
}