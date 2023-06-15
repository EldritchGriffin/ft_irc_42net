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

void Server::pass_cmd(int client_socket, std::string buffer)
{
    if(this->clients[client_socket].get_pass_state() == PASS)
    {
        // send(client_socket, "Authenticated, PASS REFUSED\n", 28, 0);
        return;
    }
    if(buffer.empty() || buffer != this->srv_password)
    {
        // send(client_socket, "ERR PASS\n", 9, 0);
        return;
    }
    this->clients[client_socket].set_pass_state(PASS);
    auth_client(client_socket);
}

void Server::nick_cmd(int client_socket, std::string buffer)
{
    //TODO check if nickname is already taken and if nickname is valid;
    if(buffer.empty())
    {
        // send(client_socket, "ERR NICK\n", 9, 0);
        return;
    }
    this->clients[client_socket].set_nickname(buffer);
    this->clients[client_socket].set_nick_state(NICK);
    // send(client_socket, "OK\n", 3, 0);
    auth_client(client_socket);
}

//TODO user is not final;
void Server::user_cmd(int client_socket, std::string buffer)
{
    if(buffer.empty())
    {
        // send(client_socket, "ERR USER\n", 9, 0);
        return;
    }
    //TODO check if username is already taken and if username is valid;
    //TODO check if user is already logged in;
    this->clients[client_socket].set_username(buffer); // TODO this here is not final, gotta substr and put every part in its place
    this->clients[client_socket].set_user_state(USER);
    // send(client_socket, "OK\n", 3, 0);
    auth_client(client_socket);
}