#include "server.hpp"
#include "Client.hpp"

void Server::auth_client(int client_socket)
{
    if(this->clients[client_socket].get_pass_state() == PASS
    && this->clients[client_socket].get_nick_state() == NICK
    && this->clients[client_socket].get_user_state() == USER)
    {
        this->clients[client_socket].set_grade(AUTHENTICATED);
        send(client_socket, "AUTH OK\r\n", 9, 0); // TODO here we send a welcome message from the server to the new client
        return;
    }
    return;
}

void Server::pass_cmd(int client_socket, std::string buffer)
{
    if(this->clients[client_socket].get_pass_state() == PASS)
    {
        send(client_socket, "Authenticated, PASS REFUSED\r\n", 10, 0);
        return;
    }
    if(buffer.empty() || buffer != this->srv_password)
    {
        send(client_socket, "ERR PASS\r\n", 10, 0);
        return;
    }
    this->clients[client_socket].set_pass_state(PASS);
    send(client_socket, "PASS OK\r\n", 9, 0);
    auth_client(client_socket);
}

void Server::nick_cmd(int client_socket, std::string buffer)
{
    //TODO check if nickname is already taken and if nickname is valid;
    if(buffer.empty())
    {
        send(client_socket, "ERR NICK\r\n", 10, 0);
        return;
    }
    this->clients[client_socket].set_nickname(buffer);
    this->clients[client_socket].set_nick_state(NICK);
    send(client_socket, "NICK OK\r\n", 9, 0);
    auth_client(client_socket);
}

void Server::user_cmd(int client_socket, std::string buffer)
{
    if(this->clients[client_socket].get_pass_state() == NOPASS)
    {
        send(client_socket, "ERR PASS\r\n", 10, 0);
        return;
    }
    if(buffer.empty())
    {
        send(client_socket, "ERR USER\r\n", 10, 0);
        return;
    }
    //TODO check if username is already taken and if username is valid;
    //TODO check if user is already logged in;
    this->clients[client_socket].set_username(buffer); // TODO this here is not final, gotta substr and put every part in its place
    this->clients[client_socket].set_user_state(USER);
    send(client_socket, "USER OK\r\n", 9, 0);
    auth_client(client_socket);
}