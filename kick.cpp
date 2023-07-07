#include "server.hpp"
#include "Channel.hpp"
#include "numeric_replies.hpp"

void Server::kick_cmd(int client_socket, std::string buffer)
{
    Client client_caller = clients[client_socket];
    if (client_caller.get_grade() != AUTHENTICATED)
    {
        std::string msg = ":" + get_srv_ip() + std::string(ERR_NOTREGISTERED) + " " + client_caller.get_nickname() +" :You have not registered\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
    std::string ch = buffer.substr(0,buffer.find(" "));
    buffer.erase(0,ch.length()+1);
    std::string user = buffer.substr(0,buffer.find(" "));
    buffer.erase(0,user.length()+1);
    std::string reason = buffer.substr(0);
    if (ch.empty() && user.empty() && reason.empty())
    {
        std::string msg = ":" + get_srv_ip() + std::string(ERR_NEEDMOREPARAMS) + " " + client_caller.get_nickname() +" KICK :Not enough parameters\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
    for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
    {
        if(it->get_name() == ch)
        {
            if (it->search_client_in_channel(client_socket) != 2 && it->search_client_in_channel(client_socket) != 1)
            {
                std::string msg = ":" + this->get_srv_ip() + " " + std::string(ERR_CHANOPRIVSNEEDED)+ " " +  it->get_name() + ch + " :You're not channel operator\r\n";
                send(client_socket, msg.c_str(), msg.length(), 0);
                return;
            }
            else if(it->search_client_in_channel(client_socket) == 1 || it->search_client_in_channel(client_socket) == 2)
            {
                for(std::vector<Client>::iterator it2 = it->get_users().begin(); it2 != it->get_users().end(); ++it2)
                {
                    if(it2->get_nickname() == user)
                    {
                        std::string msg = ":" + client_caller.get_nickname() + "!" + client_caller.get_username() + "@" + client_caller.get_hostname() + " KICK " + it->get_name() + " " + user + " :" + reason + "\r\n";
                        it->send_message(msg, it2->get_socket());
                        it->kick_user(it2->get_nickname());
                        return;
                    }
                }
                std::string msg = ":" + this->get_srv_ip() + ERR_NOSUCHNICK + user + " :No such nick/channel\r\n";
                send(client_socket, msg.c_str(), msg.length(), 0);
                return;
            }
            std::string msg = ":" + this->get_srv_ip() + ERR_NOTONCHANNEL + it->get_name() + " :You're not on that channel\r\n";
            send(client_socket, msg.c_str(), msg.length(), 0);
            return;
        }
    }
    std::string msg = ":" + this->get_srv_ip() + ERR_NOSUCHCHANNEL + ch + " :No such channel\r\n";
    send(client_socket, msg.c_str(), msg.length(), 0);
}