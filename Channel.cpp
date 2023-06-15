#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel()
{
}

Channel::Channel(std::string name, std::string topic)
{
    this->name = name;
    this->topic = topic;
    this->password = "";
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

std::string Channel::list_cmd(std::string msg)
{
    for (std::vector<Client>::iterator user_it = users.begin(); user_it != users.end(); user_it++) {
        msg += " " + user_it->get_nickname();
    }
    std::cout << msg << std::endl;
    return(msg);
}

void Channel::kick_user(std::string user)
{
    for (std::vector<Client>::iterator it = this->users.begin(); it != this->users.end(); ++it)
    {
        if (it->get_nickname() == user)
        {
            this->users.erase(it);
            return;
        }
    }
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
    this->send_message(":" + user.get_nickname() + " JOIN " + this->name + "\r\n", user.get_socket());
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

void Channel::send_message(std::string message, int client_socket)
{
    std::vector<Client>::iterator it1;
    for(std::vector<Client>::iterator it = this->users.begin(); it != this->users.end(); ++it)
    {
        it1 = it;
        if (it->get_socket() == client_socket)
            break;
        else if (++it1 == this->users.end())
            return;
    }

    for (std::vector<Client>::iterator it = this->users.begin(); it != this->users.end(); ++it)
    {
        if (it->get_socket() != client_socket)
        {
            send(it->get_socket(), message.c_str(), message.length(), 0);
        }
    }
}

