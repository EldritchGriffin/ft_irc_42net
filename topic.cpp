#include "server.hpp"
#include "Channel.hpp"
#include "numeric_replies.hpp"

int Channel::search_client_in_channel(int   client_socket)
{
    for(std::vector<Client>::iterator ch = operators.begin(); ch != operators.end(); ch++)
    {
        if (ch->get_socket() == client_socket)
            return (2);
    }
    for(std::vector<Client>::iterator ch = users.begin(); ch != users.end(); ch++)
    {
        if (ch->get_socket() == client_socket)
            return (3);
    }
    return (0);
}

int Channel::search_client_in_channel(std::string client_name)
{
    for(std::vector<Client>::iterator ch = operators.begin(); ch != operators.end(); ch++)
    {
        if (ch->get_nickname() == client_name)
            return (2);
    }
    for(std::vector<Client>::iterator ch = users.begin(); ch != users.end(); ch++)
    {
        if (ch->get_nickname() == client_name)
            return (3);
    }
    return (0);
}

void    Server::get_channel_topic(std::string channel_name, int client_socket)
{
    for(std::vector<Channel>::iterator ch = channels.begin(); ch != channels.end(); ch++)
    {
        int client_existens = ch->search_client_in_channel(client_socket);
        if (ch->get_name() == channel_name)
        {
            if (client_existens)
            {
                std::string message_sender = clients[client_socket].get_nickname();
                std::string msg = ":"+ this->get_srv_ip() +" " + RPL_TOPIC + " " + message_sender + " " + channel_name + " :" + ch->get_topic() + "\r\n";
                send(client_socket, (msg).c_str(), msg.length(), 0);
            }
            return;
        }
    }
    std::string flagu = ERR_NOSUCHCHANNEL;
    std::string erro(":"+ this->get_srv_ip() + " " + flagu + " " + "TOPIC" + " :unknown channel\r\n");
    send(client_socket, erro.c_str(), erro.length() , 0);
}

void    Server::call_ERR_CHANOPRIVSNEEDED(int client_socket, std::string channel_name, std::string cmd)
{
    std::string flagu = ERR_NEEDMOREPARAMS;
    std::string erro = ":" + this->get_srv_ip() + " "  + flagu + " " + cmd + " : You are a common user of this channel {" + channel_name +"} to push your privilege !!\r\n";
    send(client_socket, erro.c_str(), erro.length() , 0);
}

void    Server::set_channel_topic(int client_socket, std::string channel_name, std::string buffer)
{
    for(std::vector<Channel>::iterator ch = channels.begin(); ch != channels.end(); ch++)
    {
        if (ch->get_name() == channel_name)
        {
            int client_existens = ch->search_client_in_channel(client_socket);
            if ((client_existens > 0 && client_existens < 3) || (client_existens == 3 && ch->get_topic_flag() == 1))
            {
                ch->set_topic(buffer);
                std::string user_nam = clients[client_socket].get_nickname();
                std::string msg = ":" + user_nam+"!"+user_nam[0]+"@" + this->get_srv_ip()  + " TOPIC " + channel_name + " :" + buffer + "\r\n";
                send(client_socket, msg.c_str(), msg.length(), 0);
                ch->send_message(msg, client_socket);
            }
            else
                call_ERR_CHANOPRIVSNEEDED(client_socket, channel_name,  "TOPIC");
            return ;
        }
    }
}

std::string Server::get_client_nick_by_socket(int client_socket)
{
    for (unsigned int i = 0; i < clients.size();i++)
    {
        if (client_socket == clients[i].get_socket())
            return (clients[i].get_nickname());
    }
    return ("");
}

void Server::call_ERR_NEEDMOREPARAMS(int client_socket,  std::string cmd)
{
    std::string flagu = ERR_NEEDMOREPARAMS;
    std::string erro(":"+ this->get_srv_ip() +" "  + flagu + " " + cmd + " :Not enough parameters\r\n");
    send(client_socket, erro.c_str(), erro.length() , 0);
}

#define scayho std::string
#define darck std::vector<Channel>::iterator
#define augur std::vector<Client>::iterator

int Server::check_if_on_channel(int client_socket, std::string channel_name)
{
    for (darck it = channels.begin();it != channels.end();it++)
    {
        if (it->get_name() == channel_name) // find the target channel
        {
            if (it->search_client_in_channel(client_socket) > 0) // check if the client belong to the target channel
                return (0);
            else
                return (1);
        }
    }
    return (1);
}

void    Server::call_ERR_NOTONCHANNEL(int client_socket, std::string cmd)
{
    std::string erro(":"+ this->get_srv_ip() + " " + std::string(ERR_NOTONCHANNEL) + " "  + cmd + " :You do not belong to this channel !!\r\n");
    send(client_socket, erro.c_str(), erro.length() , 0);
}

// [TOPIC]  => [TOPIC :]
// [TOPIC #JAHAD]  => [TOPIC : #JAHAD]
// [TOPIC #JAHAD trueking]  => [TOPIC : #JAHAD trueking]
// [TOPIC #JAHAD true king]  => [TOPIC : #JAHAD :true king]
// [TOPIC #JAHAD :true king]  => [TOPIC : #JAHAD ::true king]
// no multiple targets for TOPIC || spaces are ignored

void    Server::unset_channel_topic(std::string channel_name, int client_socket)
{
    for(std::vector<Channel>::iterator ch = channels.begin(); ch != channels.end(); ch++)
    {
        if (ch->get_name() == channel_name)
        {
            ch->set_topic("");
            std::string erro = ":" + clients[client_socket].get_nickname()+"!"+clients[client_socket].get_nickname()[0]+"@" + this->get_srv_ip()  + " TOPIC " + channel_name + " :" + "\r\n";
            send(client_socket, erro.c_str(), erro.length() , 0);
            ch->send_message(erro, client_socket);
        }
    }
}


void Server::topic_cmd(int client_socket, std::string buffer)
{
    size_t pos = 0;
    if (buffer.empty() || buffer == ":")
    {
        call_ERR_NEEDMOREPARAMS(client_socket, "TOPIC"); // [TOPIC]  => [TOPIC :]
        return ;
    }
    std::string channel_name = buffer.substr(0, buffer.find(' '));
    if (check_if_channel_exist(channel_name) == 0)
    {
        call_ERR_NOSUCHCHANNEL(client_socket, channel_name, "TOPIC");
        return ;
    }
    pos = buffer.find(' ');
    if (pos == buffer.npos)
        buffer.erase(0, buffer.npos);
    else
        buffer.erase(0, pos  + 1);
    if (check_if_on_channel(client_socket, channel_name) == 1)
    {
        call_ERR_NOTONCHANNEL(client_socket, "TOPIC");
        return ;
    }
    if (buffer.empty())
    {
        get_channel_topic(channel_name, client_socket);
    }
    else if (buffer == "::")
    {
        unset_channel_topic(channel_name, client_socket);
    }
    else
    {
        if (buffer[0] == ':')
            buffer.erase(buffer.begin());
        set_channel_topic(client_socket, channel_name, buffer);
    }
}

// ERR_NEEDMOREPARAMS  ERR_NOTONCHANNEL RPL_NOTOPIC  RPL_TOPIC    ERR_CHANOPRIVSNEEDED

// to do unset topic => If <topic> is given, the command is a request to change the topic.
//                                           If <topic> is given and is an empty string,
                                            // the server unsets the current topic. If <topic> is not given,
                                            // the server returns a topic burst with the current topic information.

// logtime start_date:2023-05-29
