#include "server.hpp"
#include "Channel.hpp"
#include "Tools.hpp"
#include "numeric_replies.hpp"

void print_vector(std::vector<std::string> vec)
{
    for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ++it)
    {
        std::cout << *it << '\n';
    }
}

void Server::part_cmd(int client_socket,std::string buffer){
    Client client_caller = clients[client_socket];
    std::string ch = buffer.substr(0,buffer.find(" "));
    buffer.erase(0,ch.length()+1);
    std::string reson = buffer.substr(0);
    std::vector<std::string> channel = split_multiple_targets(ch);
    if (ch == "")
        call_ERR_NEEDMOREPARAMS(client_socket, "PART");
    else{
        for(size_t i=0; i < channel.size();i++)
        {
            ch = channel[i];
            for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); it++)
            {
                if(it->get_name() == ch)
                {
                    if (this->check_if_channel_exist(it->get_name()) == 0)
                    {
                        call_ERR_NOSUCHCHANNEL(client_socket, ch,"PART");
                        return;
                    }
                    if (it->check_if_user_exist_in_channel(client_caller.get_nickname()) == 0)
                    {
                        call_ERR_NOTONCHANNEL(client_socket, "PART");
                        return;
                    }
                    if(it->search_client_in_channel(client_socket) != 0 &&  it->get_users().size() == 1)
                    {
                        std::string msg = ":" + client_caller.get_nickname() + " PART " + ch + " " + reson + "\r\n";
                        it->send_message(msg, client_socket);
                        send(client_socket, msg.c_str(),msg.length(),0);
                        it->remove_user(client_caller);
                        it->remove_operator(client_caller);
                        this->channels.erase(it);
                        break;
                    }
                    else if(it->search_client_in_channel(client_socket) != 0 && it->get_users().size() > 1)
                    {
                        std::string msg = ":" + client_caller.get_nickname() + " PART " + ch + " " + reson + "\r\n";
                        it->send_message(msg, client_socket);
                        send(client_socket, msg.c_str(),msg.length(),0);
                        it->remove_user(client_caller);
                        it->remove_operator(client_caller);
                        break;
                    }

                }
            }
        }
    }
}