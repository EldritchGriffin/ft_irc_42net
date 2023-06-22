#include "server.hpp"
#include "Client.hpp"
#include "numeric_replies.hpp"

void Server::auth_client(int client_socket)
{
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

void Server::pass_cmd(int client_socket, std::vector<std::string> args)
{
    if(this->clients[client_socket].get_pass_state() == PASS)
    {
        std::string msg = ":" + this->get_srv_ip() + " " + ERR_ALREADYREGISTRED + " "
        + this->clients[client_socket].get_nickname() + " :You may not reregister\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
    if(args.size() < 1)
    {
        call_ERR_NEEDMOREPARAMS(client_socket);
        return;
    }
    if(args[0] != this->get_srv_password())
    {
        std::string msg = ":" + this->get_srv_ip() + " " + ERR_PASSWDMISMATCH + " "
        + this->clients[client_socket].get_nickname() + " :Password incorrect\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
    this->clients[client_socket].set_pass_state(PASS);
    auth_client(client_socket);
}

void Server::nick_cmd(int client_socket, std::vector<std::string> args)
{
    //TODO check if nickname is already taken and if nickname is valid;
    if(args.size() < 1)
    {
        call_ERR_NEEDMOREPARAMS(client_socket);
        return;
    }
    this->clients[client_socket].set_nickname(args[0]);
    this->clients[client_socket].set_nick_state(NICK);
    auth_client(client_socket);
}

//TODO user is not final;
void Server::user_cmd(int client_socket, std::vector<std::string> args)
{

    //TODO check if username is already taken and if username is valid;
    //TODO check if user is already logged in;
    this->clients[client_socket].set_username(args[0]); // TODO this here is not final, gotta substr and put every part in its place
    this->clients[client_socket].set_user_state(USER);
    // send(client_socket, "OK\n", 3, 0);
    auth_client(client_socket);
}