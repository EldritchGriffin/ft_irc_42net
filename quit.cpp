#include "server.hpp"
#include "Channel.hpp"
#include "numeric_replies.hpp"

void Server::remove_fd(int fd)
{
    for(size_t i = 0; i < this->pollfds.size(); i++)
    {
        if(this->pollfds[i].fd == fd)
        {
            this->pollfds.erase(this->pollfds.begin() + i);
            break;
        }
    }
}

void Server::monitor_channells()
{
    if(this->channels.size() == 0)
        return;
    for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); it++)
    {
        if(it->get_users().size() == 0)
        {
            this->channels.erase(it);
            it = this->channels.begin();
        }
        if(this->channels.size() == 1)
            break;
    }
    if(channels[0].get_users().size() == 0)
        this->channels.erase(channels.begin());
}

void Server::quit_cmd(int client_socket)
{
    Client client_caller = clients[client_socket];
    std::string msg = ":" + client_caller.get_nickname() + " QUIT\r\n";
    for(size_t i = 0; i < this->channels.size(); i++)
    {
        if(channels[i].search_client_in_channel(client_caller.get_nickname()) != 0)
        {
            part_cmd(client_socket, channels[i].get_name());
        }
    }
    send(client_socket, msg.c_str(),msg.length(),0);
    remove_fd(client_socket);
    clients.erase(client_socket);
    close(client_socket);
}