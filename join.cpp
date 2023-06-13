#include "server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Tools.hpp"

void    create_channel(int client_socket, std::string channel_name, std::string key, Server &server)
{
    std::map<int, Client> clients = server.get_clients();
    std::vector<Channel> channels = server.get_channels();

    Client client_caller = clients[client_socket];
    Channel new_channel(channel_name, "");
    new_channel.set_admin(client_caller);
    new_channel.set_password(key);
    new_channel.add_user(client_caller);
    channels.push_back(new_channel);
    server.set_channels(channels);
    std::string msg = ":" + client_caller.get_nickname() + " JOINED " + channel_name + "\n";
    send(client_socket, msg.c_str(), msg.length(), 0);
}

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
                it->send_message(":" + client_caller.get_nickname() + " JOIN " + channel_name, client_socket);
                return;
            }
            else
            {
                std::string msg = "Wrong password for channel " + channel_name + "\n";
                send(client_socket, msg.c_str(), msg.length(), 0);
                return;
            }
        }
    }
    create_channel(client_socket, channel_name, key, server);
    send(client_socket, "Channel created\n", 16, 0);
}

void Server::join_cmd(int client_socket, std::string buffer)
{
    Client client_caller = clients[client_socket];


    std::string channel_name = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, channel_name.length() + 1);
    std::string key = buffer.substr(0, buffer.length());
    buffer.erase(0, key.length() + 1);

    std::vector<std::string> channel_targets = split_multiple_targets(channel_name);
    std::vector<std::string> key_targets = split_multiple_targets(key);

    for(unsigned int i = 0; i < channel_targets.size(); i++)
    {
        join_channel(client_socket, channel_targets[i], key_targets[i], *this);
    }
}