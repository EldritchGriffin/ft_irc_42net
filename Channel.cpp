#include "Channel.hpp"
#include "Client.hpp"


Channel::Channel(std::string name, std::string topic)
{
    this->name = name;
    this->topic = topic;
}

Channel::~Channel()
{
}

void Channel::set_password(std::string password)
{
    this->password = password;
}

std::string Channel::get_password() const
{
    return this->password;
}

void Channel::set_name(std::string name)
{
    this->name = name;
}

void Channel::set_topic(std::string topic)
{
    this->topic = topic;
}

void Channel::set_admin(Client admin)
{
    this->admin = admin;
}

std::string Channel::get_name() const
{
    return this->name;
}

std::string Channel::get_topic() const
{
    return this->topic;
}

Client Channel::get_admin() const
{
    return this->admin;
}

void Channel::add_user(Client user)
{
    this->users.push_back(user);
}

void Channel::add_operator(Client user)
{
    this->operators.push_back(user);
}

void Channel::remove_user(Client user)
{
    for (std::vector<Client>::iterator it = this->users.begin(); it != this->users.end(); ++it)
    {
        if (it->get_nickname() == user.get_nickname())
        {
            this->users.erase(it);
            return;
        }
    }
}

void Channel::remove_operator(Client user)
{
    for (std::vector<Client>::iterator it = this->operators.begin(); it != this->operators.end(); ++it)
    {
        if (it->get_nickname() == user.get_nickname())
        {
            this->operators.erase(it);
            return;
        }
    }
}

void Channel::send_message(std::string message)
{
    for (std::vector<Client>::iterator it = this->users.begin(); it != this->users.end(); ++it)
    {
        send(it->get_socket(), message.c_str(), message.length(), 0);
    }
    for (std::vector<Client>::iterator it = this->operators.begin(); it != this->operators.end(); ++it)
    {
        send(it->get_socket(), message.c_str(), message.length(), 0);
    }
    send(this->admin.get_socket(), message.c_str(), message.length(), 0);
}