#include "server.hpp"
#include "Channel.hpp"
#include "Tools.hpp"
#include "numeric_replies.hpp"

void Server::list_cmd(int client_socket, std::string buffer)
{
    std::string ch = buffer.substr(0,buffer.find(" "));
    if (ch == "")
    {
        for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
        {
            std::string msg = ":" + this->get_srv_ip() + " 322 " + clients[client_socket].get_nickname() + " " + it->get_name() + " " + std::to_string(it->get_users().size()) + " :" + it->get_topic() + "\r\n";
            send(client_socket, msg.c_str(),msg.length(),0);
        }
        std::string msg =":"+ this->get_srv_ip() + " 323 " + clients[client_socket].get_nickname() + " :End of /LIST\r\n";
        send(client_socket,msg.c_str(), msg.length(), 0);
    }
    else
    {
        for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
        {
            if(it->get_name() == ch)
            {
                std::string msg1= "321 RPL_LISTSTART :"+ ch + " " +it->get_topic()+"\r\n";
                send(client_socket, msg1.c_str(), msg1.length(), 0);
                std::string msg = ":" + this->get_srv_ip() + " 322 " + clients[client_socket].get_nickname() + " " + it->get_name() + " " + std::to_string(it->get_users().size()) + " :" + it->get_topic() + "\r\n";
                send(client_socket, msg.c_str(),msg.length(),0);
                std::string msg2 =":"+ this->get_srv_ip() + " 323 " + clients[client_socket].get_nickname() + " :End of /LIST\r\n";
                send(client_socket,msg2.c_str(), msg2.length(), 0);
                return;
            }
        }
        call_ERR_NOSUCHCHANNEL(client_socket, ch,"LIST");
    }
}