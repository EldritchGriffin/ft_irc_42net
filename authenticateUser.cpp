#include "server.hpp"
#include "Client.hpp"
#include "numeric_replies.hpp"

void Server::auth_client(int client_socket)
{
    if(this->clients[client_socket].get_grade() == AUTHENTICATED)
        return;
    if(this->clients[client_socket].get_pass_state() == PASS
    && this->clients[client_socket].get_nick_state() == NICK
    && this->clients[client_socket].get_user_state() == USER)
    {
        this->clients[client_socket].set_grade(AUTHENTICATED);
        std::string msg = ":" + this->get_srv_ip() + " " + RPL_WELCOME + " " 
        + this->clients[client_socket].get_nickname() + " :" + WLCM_MSG + "\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
    return;
}

void Server::pass_cmd(int client_socket, std::string buffer)
{
    if(this->clients[client_socket].get_pass_state() == PASS)
    {
        std::string msg = ":" + this->get_srv_ip() + " " + ERR_ALREADYREGISTRED + " " " :Unauthorized command (already registered)\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
    if(buffer.empty() || buffer != this->srv_password)
    {
        std::string msg = ":" + this->get_srv_ip() + " " + ERR_PASSWDMISMATCH + " " " :Password incorrect\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
    this->clients[client_socket].set_pass_state(PASS);
    auth_client(client_socket);
}

void Server::nick_cmd(int client_socket, std::string buffer)
{
    if(buffer.empty() || buffer == ":")
    {
        std::string msg = ":" + this->get_srv_ip() + " " + ERR_NONICKNAMEGIVEN + " * " + ":No nickname given\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
    for(std::map<int, Client>::iterator it = this->clients.begin(); it != this->clients.end(); it++)
    {
        if(it->second.get_nickname() == buffer)
        {
            std::string msg = ":" + this->get_srv_ip() + " " + ERR_NICKNAMEINUSE + " " + buffer + ":Nickname is already in use\r\n";
            send(client_socket, msg.c_str(), msg.length(), 0);
            return;
        }
    }
    std::string msg = ":" + this->clients[client_socket].get_nickname() + " NICK :" + buffer + "\r\n";
    send(client_socket, msg.c_str(), msg.length(), 0);
    this->clients[client_socket].set_nickname(buffer);
    this->clients[client_socket].set_nick_state(NICK);
    auth_client(client_socket);
}

int process_user(std::string buffer, Client &client)
{
    std::string username;
    std::string hostname;
    std::string servername;
    std::string realname;

    if(buffer.empty())
    {
        std::string msg = ":" + client.get_nickname() + " " + ERR_NEEDMOREPARAMS + " USER :Not enough parameters\r\n";
        send(client.get_socket(), msg.c_str(), msg.length(), 0);
        return 1;
    }
    username = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, username.length() + 1);
    hostname = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, hostname.length() + 1);
    servername = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, servername.length() + 1);
    realname = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, realname.length() + 1);
    if(username.empty() || hostname.empty() || servername.empty() || realname.empty())
    {
        std::string msg = ":" + client.get_nickname() + " " + ERR_NEEDMOREPARAMS + " USER :Not enough parameters\r\n";
        send(client.get_socket(), msg.c_str(), msg.length(), 0);
        return 1;
    }
    client.set_username(username);
    client.set_hostname(hostname);
    client.set_servername(servername);
    client.set_realname(realname);
    return 0;
}

void Server::user_cmd(int client_socket, std::string buffer)
{
    if(this->clients[client_socket].get_user_state() == USER)
    {
        std::string msg = ":" + this->get_srv_ip() + " " + ERR_ALREADYREGISTRED + " " + this->clients[client_socket].get_nickname() + " :Unauthorized command (already registered)\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
    if(process_user(buffer, this->clients[client_socket]))
        return;
    this->clients[client_socket].set_user_state(USER);
    auth_client(client_socket);
}