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
    this->topic_changers_flag = 0;
    topic_changers_flag = 0;
    invite_changers_flag = 0;
    limit_changers_flag = 0;
    key_changers_flag = 0;
}

void Channel::set_invite_flag(int n)
{
    invite_changers_flag = n;
}

void Channel::set_limit_flag(int n)
{
    limit_changers_flag = n;
}

void Channel::set_limit_value(std::string n)
{
    limit = n;
}

void Channel::set_key_flag(int n)
{
    key_changers_flag = n;
}

int Channel::get_invite_flag() const
{
    return (invite_changers_flag);
}

int Channel::get_limit_flag() const
{
    return (limit_changers_flag);
}

std::string Channel::get_limit_value() const
{
    return (limit);
}

int Channel::get_key_flag() const
{
    return (key_changers_flag);
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

void Channel::set_topic_flag(int g)
{
    this->topic_changers_flag = g;
}

void Channel::set_topic(std::string topic)
{
    this->topic = topic;
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

std::string Channel::get_name() const
{
    return this->name;
}

int Channel::get_topic_flag() const
{
    return this->topic_changers_flag;
}
std::string Channel::get_topic() const
{
    return this->topic;
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

int Channel::search_invited(std::string client_name)
{
    for(std::vector<Client>::iterator ch = invited.begin(); ch != invited.end(); ch++)
    {
        if (ch->get_nickname() == client_name)
            return (1);
    }
    return (0);
}

std::vector<Client> &Channel::get_users()
{
    return this->users;
}

void Channel::set_users(std::vector<Client> users)
{
    this->users = users;
}
