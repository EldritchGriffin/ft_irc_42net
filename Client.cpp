#include "Client.hpp"
#include "Channel.hpp"

Client::Client()
{
}

Client::Client(int socket, sockaddr_in client_addr)
{
    this->socket = socket;
    this->addr = client_addr;
    this->nickname = "";
    this->username = "";
    this->realname = "";
    this->hostname = "";
    this->servername = "";
    this->pass_state = NOPASS;
    this->nick_state = NONICK;
    this->user_state = NOUSER;
    this->grade = NOT_AUTHENTICATED;
}

Client::~Client()
{
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

void Client::set_pass_state(int state)
{
    this->pass_state = state;
}

int Client::get_socket() const
{
    return this->socket;
}

int Client::get_nick_state() const
{
    return this->nick_state;
}

void Client::set_nick_state(int state)
{
    this->nick_state = state;
}

int Client::get_user_state() const
{
    return this->user_state;
}

void Client::set_user_state(int state)
{
    this->user_state = state;
}

int Client::get_grade() const
{
    return this->grade;
}

void Client::set_grade(int grade)
{
    this->grade = grade;
}

void Client::set_hostname(std::string hostname)
{
    this->hostname = hostname;
}

std::string Client::get_servername() const
{
    return this->servername;
}

void Client::set_servername(std::string servername)
{
    this->servername = servername;
}

std::string Client::get_hostname() const
{
    return this->hostname;
}



