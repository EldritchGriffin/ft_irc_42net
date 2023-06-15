#include "server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Tools.hpp"

void    create_channel(int client_socket, std::string channel_name, std::string key, Server &server)
{
    std::map<int, Client> clients = server.get_clients();
    std::vector<Channel> channels = server.get_channels();

    Client client_caller = clients[client_socket];
    Channel new_channel(channel_name, "this is a topic");
    new_channel.set_admin(client_caller);
    new_channel.set_password(key);
    new_channel.add_user(client_caller);
    channels.push_back(new_channel);
    server.set_channels(channels);
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
                std::string message = ":" + server.get_srv_ip() + " 332 " + client_caller.get_nickname() + " " + channel_name + " :" + it->get_topic() + "\r\n";
                send(client_socket, message.c_str(), message.length(), 0);
                std::cout << client_caller.get_nickname() << " Channel JOINED: " << channel_name << std::endl;
                return;
            }
            return ;
        }
    }
    create_channel(client_socket, channel_name, key, server);
    std::cout << client_caller.get_nickname() << " Channel created: " << channel_name << std::endl;
}

void Server::join_cmd(int client_socket, std::string buffer)
{
    Client client_caller = clients[client_socket];


    std::string channel_name = buffer.substr(0, buffer.find(" "));
    std::cout << "channel_name: " << channel_name << std::endl;
    buffer.erase(0, channel_name.length() + 1);
    std::string key = buffer.substr(0, buffer.length());
    buffer.erase(0, key.length() + 1);

    std::vector<std::string> channel_targets = split_multiple_targets(channel_name);
    std::vector<std::string> key_targets = split_multiple_targets(key);

    for(unsigned int i = 0; i < channel_targets.size(); i++)
    {
        if(key_targets.size() == 0 || i >= key_targets.size())
            key_targets.push_back("");
        join_channel(client_socket, channel_targets[i], key_targets[i], *this);
    }
}