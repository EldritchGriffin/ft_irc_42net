#include "server.hpp"
#include "Channel.hpp"
#include "Tools.hpp"
#include "numeric_replies.hpp"

void Server::kick_cmd(int client_socket, std::string buffer)
{
    Client client_caller = clients[client_socket];
    std::string ch = buffer.substr(0,buffer.find(" "));
    buffer.erase(0,ch.length()+1);
    std::string user = buffer.substr(0,buffer.find(" "));
    buffer.erase(0,user.length()+1);
    std::string reason = buffer.substr(0);
    std::vector<std::string> users = split_multiple_targets(user);
    if (ch.empty() && user.empty() && reason.empty())/// need to fix this
    {
        std::string msg = ":" + get_srv_ip() + std::string(ERR_NEEDMOREPARAMS) + " " + client_caller.get_nickname() +" KICK :Not enough parameters\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
    if (check_if_channel_exist(ch) == 0)
    {
        std::string msg = ":" + this->get_srv_ip() + ERR_NOSUCHCHANNEL + ch + " :No such channel\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
    for(size_t i=0;i < users.size();i++)
    {
        user = users[i];
        for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
        {
            if(it->get_name() == ch)
            {
                if (it->check_if_user_exist_in_channel(client_caller.get_nickname()) == 0)
                {
                    std::string msg = ":" + this->get_srv_ip() + " " + ERR_NOTONCHANNEL + " " + it->get_name() + " :You're not on that channel\r\n";
                    send(client_socket, msg.c_str(), msg.length(), 0);
                    return;
                }
                if (it->check_if_user_exist_in_channel(user) == 0)
                {
                    std::string msg = ":" + this->get_srv_ip() + " " + ERR_USERNOTINCHANNEL +" "+ user + " " + ch + " :They aren't on that channel\r\n";
                    send(client_socket, msg.c_str(), msg.length(), 0);
                    // return;
                    break;
                }
                if (it->search_client_in_channel(client_socket) != 2)
                {
                    std::string msg = ":" + this->get_srv_ip() + " " + std::string(ERR_CHANOPRIVSNEEDED)+ " " +  it->get_name() + ch + " :You're not channel operator\r\n";
                    send(client_socket, msg.c_str(), msg.length(), 0);
                    return;
                }
                else if(it->search_client_in_channel(client_socket) == 2)
                {
                    for(std::vector<Client>::iterator it2 = it->get_users().begin(); it2 != it->get_users().end(); ++it2)
                    {
                        if(it2->get_nickname() == user)
                        {
                            if(reason.size() == 0)
                            {
                                std::string msg = ":" + client_caller.get_nickname() + " KICK " + ch + " " + user + "\r\n";
                                it->send_message(msg, it2->get_socket());
                                send(it2->get_socket(), msg.c_str(),msg.length(),0);
                            }
                            else{
                                std::string msg = ":" + client_caller.get_nickname() + " KICK " + ch + " " + user + " " + reason + "\r\n";
                                it->send_message(msg, it2->get_socket());
                                send(it2->get_socket(), msg.c_str(),msg.length(),0);
                            }
                            std::string tmp  = it2->get_nickname();
                            it->kick_user(tmp);
                            it->remove_operator(get_user_obj(tmp));
                            // return;
                            break;
                        }
                    }
                }
            }
        }
    }
}