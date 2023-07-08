#include "server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Tools.hpp"
#include "numeric_replies.hpp"

void sendUserList(const std::string& channel, int client_socket, Server& server) {
    std::vector<Channel>& channels = server.get_channels();
    std::map<int, Client>& clients = server.get_clients();

    std::string userList = "= " + channel + " :";
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->get_name() == channel) {
            std::vector<Client>& users = it->get_users();
            for (std::vector<Client>::iterator it2 = users.begin(); it2 != users.end(); ++it2) {
                if(it->search_client_in_channel(it2->get_nickname()) == 1 || it->search_client_in_channel(it2->get_nickname()) == 2)
                    userList += "@" + it2->get_nickname() + " ";
                else
                    userList += it2->get_nickname() + " ";
            }
            break;
        }
    }

    std::string message = ":" + server.get_srv_ip() + " " + RPL_NAMREPLY + " " + clients[client_socket].get_nickname() + " " + userList + "\r\n";
    send(client_socket, message.c_str(), message.length(), 0);

    message = ":" + server.get_srv_ip() + " " + RPL_ENDOFNAMES + " " + clients[client_socket].get_nickname() + " " + channel + " :End of NAMES list\r\n";
    send(client_socket, message.c_str(), message.length(), 0);
}

void    create_channel(int client_socket, std::string channel_name, std::string key, Server &server)
{
    std::map<int, Client> &clients = server.get_clients();
    std::vector<Channel> &channels = server.get_channels();

    Client client_caller = clients[client_socket];
    Channel new_channel(channel_name, "this is a topic");
    new_channel.set_admin(client_caller);
    new_channel.set_password(key);
    if (key != "")
    {
        new_channel
    }
    
    new_channel.add_user(client_caller);
    channels.push_back(new_channel);
    std::string message = ":" +  client_caller.get_nickname() + " JOIN " + channel_name + "\r\n";
    send(client_socket, message.c_str(), message.length(), 0);
    sendUserList(channel_name, client_socket, server);
}

void join_channel(int client_socket, std::string channel_name, std::string key, Server& server) {
    std::map<int, Client>& clients = server.get_clients();
    std::vector<Channel>& channels = server.get_channels();

    Client client_caller = clients[client_socket];
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->get_name() == channel_name) {
            if (it->get_password() == key) {
                it->add_user(client_caller);
                std::string message = ":" + client_caller.get_nickname() + " JOIN " + channel_name + "\r\n";
                send(client_socket, message.c_str(), message.length(), 0);
                message = ":" + server.get_srv_ip() + " " + RPL_TOPIC + " " +
                                      client_caller.get_nickname() + " " + channel_name + " :" + it->get_topic() + "\r\n";
                send(client_socket, message.c_str(), message.length(), 0);
                sendUserList(channel_name, client_socket, server);
                return;
            }
        }
    }
    create_channel(client_socket, channel_name, key, server);
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
        if(key_targets.size() == 0 || i >= key_targets.size())
            key_targets.push_back("");
        join_channel(client_socket, channel_targets[i], key_targets[i], *this);
    }
        print_vector(this->channels.at(this->channels.size() - 1).get_users());
} 