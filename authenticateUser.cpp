#include "server.hpp"
#include "Client.hpp"
#include "numeric_replies.hpp"

void Server::welcome_user(int client_socket)
{
    std::string msg = ":" + this->get_srv_ip() + " " + RPL_WELCOME + " " + this->clients[client_socket].get_nickname() + " :𝕎𝕖𝕝𝕔𝕠𝕞𝕖 𝕥𝕠 𝕥𝕙𝕖 𝕀ℝℂ 𝕤𝕖𝕣𝕧𝕖𝕣. 𝕄𝕒𝕕𝕖 𝕓𝕪: 𝕒𝕖𝕝𝕪𝕒𝕜𝕠𝕦, 𝕒𝕓𝕖𝕝𝕒𝕙𝕔𝕖. 𝕒𝕓𝕤𝕖𝕝𝕒." + this->clients[client_socket].get_nickname() + "!" + this->clients[client_socket].get_username() + "@" + this->clients[client_socket].get_hostname() + "\r\n";
    send(client_socket, msg.c_str(), msg.length(), 0);
    msg = ":" + this->get_srv_ip() + " " + RPL_YOURHOST + " " + this->clients[client_socket].get_nickname() + " :Your host is " + this->get_srv_ip() + ", running version 1.0\r\n";
    send(client_socket, msg.c_str(), msg.length(), 0);
    msg = ":" + this->get_srv_ip() + " " + RPL_CREATED + " " + this->clients[client_socket].get_nickname() + " :This server was created when we locked team XD\r\n";
    send(client_socket, msg.c_str(), msg.length(), 0);
    msg = ":" + this->get_srv_ip() + " " + RPL_MYINFO + " " + this->clients[client_socket].get_nickname() + " :this server is a project of 42_cursus\r\n";
    send(client_socket, msg.c_str(), msg.length(), 0);
    msg = ":" + this->get_srv_ip() + " " + RPL_MOTDSTART + " " + this->clients[client_socket].get_nickname() + " :- " + this->get_srv_ip() + " Get ready to experience the wonkiest irc server in existence (no server to server communication XD)\r\n";
    send(client_socket, msg.c_str(), msg.length(), 0);
    msg = ":" + this->get_srv_ip() + " " + RPL_MOTD + " " + this->clients[client_socket].get_nickname() + " :- Remember, he's not dumb, he's just beyond our understanding " + this->clients[client_socket].get_nickname() + "!" + this->clients[client_socket].get_username() + "@" + this->clients[client_socket].get_hostname() + "\r\n";
    send(client_socket, msg.c_str(), msg.length(), 0);
    msg = ":" + this->get_srv_ip() + " " + RPL_ENDOFMOTD + " " + this->clients[client_socket].get_nickname() + " :End of message of the day\r\n";
    send(client_socket, msg.c_str(), msg.length(), 0);
    return;
}

void Server::auth_client(int client_socket)
{
    if(this->clients[client_socket].get_grade() == AUTHENTICATED)
        return;
    if(this->clients[client_socket].get_pass_state() == PASS
    && this->clients[client_socket].get_nick_state() == NICK
    && this->clients[client_socket].get_user_state() == USER)
    {
        this->clients[client_socket].set_grade(AUTHENTICATED);
        welcome_user(client_socket);
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
    // client.set_hostname(hostname);
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