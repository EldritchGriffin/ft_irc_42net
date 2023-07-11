#include "server.hpp"
#include "Channel.hpp"
#include "numeric_replies.hpp"

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
        if(channels[i].check_if_user_exist_in_channel(client_caller.get_nickname()) == 1)
        {
            std::string message = ":" + client_caller.get_nickname() + " PART " + channels[i].get_name() + "\r\n";
            channels[i].remove_user(client_caller);
            channels[i].remove_operator(client_caller);
            channels[i].send_message(message, client_socket);
        }
    }
    monitor_channells();
    send(client_socket, msg.c_str(),msg.length(),0);
    clients.erase(client_socket);
    close(client_socket);
}