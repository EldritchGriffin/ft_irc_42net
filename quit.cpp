#include "server.hpp"
#include "Channel.hpp"
#include "numeric_replies.hpp"

void Server::quit_cmd(int client_socket)
{
    Client client_caller = clients[client_socket];
    if (client_caller.get_grade() != AUTHENTICATED)
    {
        std::string msg = ":" + get_srv_ip() + std::string(ERR_NOTREGISTERED) + " " + client_caller.get_nickname() +" :You have not registered\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
    for(std::map<int,Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
    {
        if(it->first == client_socket)
        {
            std::string msg = ":" + client_caller.get_nickname()  + "@" + this->get_srv_ip() + " QUIT :Client Quit\r\n";
            for(std::vector<Channel>::iterator it1 = this->channels.begin(); it1 != this->channels.end(); ++it1)
            {
                this->part_cmd(client_socket, it1->get_name());
            }
            send(client_socket, msg.c_str(), msg.length(), 0);
            close(it->first);
            clients.erase(it);
            return;
        }
    }
}