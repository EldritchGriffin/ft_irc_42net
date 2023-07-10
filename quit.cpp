#include "server.hpp"
#include "Channel.hpp"
#include "numeric_replies.hpp"

void Server::quit_cmd(int client_socket)
{
    Client client_caller = clients[client_socket];
    for(std::map<int,Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
    {
        if(it->first == client_socket)
        {
            std::string msg = ":" + client_caller.get_nickname()  + "@" + this->get_srv_ip() + " QUIT :Client Quit\r\n";
            for(size_t i = 0; i < this->channels.size(); i++)
            {
                this->part_cmd(client_socket, channels[i].get_name());
            }
            send(client_socket, msg.c_str(), msg.length(), 0);
            close(it->first);
            clients.erase(it);
            return;
        }
    }
}