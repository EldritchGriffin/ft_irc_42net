#include "server.hpp"
#include "channel.hpp"
#include "client.hpp"

void join_channel(int client_socket, std::string channel_name, std::string key, Server &server)
{
    std::map<int, Client> clients = server.get_clients();
    std::vector<Channel> channels = server.get_channels();


    Client client_caller = clients[client_socket];
    for(std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        if(it->get_name() == channel_name)
        {
            if (it->get_password() == key)
            {
                it->add_user(client_caller);
                it->send_message(":" + client_caller.get_nickname() + " JOIN " + channel_name);
            }
            else
            {
                std::string msg = "Wrong password for channel " + channel_name + "\r\n";
                send(client_socket, msg.c_str(), msg.length(), 0);
            }
        }
    }
}

void Server::join_cmd(int client_socket, std::string buffer)
{
    Client client_caller = clients[client_socket];

    std::string channel_name = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, channel_name.length() + 1);
    std::string key = buffer.substr(0, buffer.length());
    buffer.erase(0, key.length() + 1);

    if (channel_name.find(',', 0) != std::string::npos)
    {
        while (channel_name.find(',', 0) != std::string::npos)
        {
            std::string tmp = channel_name.substr(0, channel_name.find(',', 0));
            channel_name.erase(0, channel_name.find(',', 0) + 1);
            join_channel(client_socket, tmp, key, *this);
        }
        std::string tmp = channel_name.substr(0);
        join_channel(client_socket, tmp, key, *this);
    }
    else
        join_channel(client_socket, channel_name, key, *this);
}