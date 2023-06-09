#include "server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "numeric_replies.hpp"

void    Channel::update_topic_mode(Client client_socket, std::string mode, std::string srv_ip)
{
    int droit = search_client_in_channel(client_socket.get_nickname());
    if (droit != 1 && droit != 2)
    {
        std::string erro(":"+ srv_ip + " " + std::string(ERR_CHANOPRIVSNEEDED) + " TOPIC :You do not have permission to update the mode +-t !!\r\n");
        if (droit != 3)
        {
            std::string erro(":" + srv_ip + std::string(ERR_NOTONCHANNEL) + " TOPIC :You do not belong to this channel !!\r\n");
            send(client_socket.get_socket(), erro.c_str(), erro.length() , 0);
        }
        else
            send(client_socket.get_socket(), erro.c_str(), erro.length() , 0);
    }
    else if (mode == "+t")
    {
        if (topic_changers_flag)
            return ;
        topic_changers_flag = 1;
        std::string msg(":" + client_socket.get_nickname() + "!~h@" + srv_ip + " MODE " + name + " +t " + "\r\n");
        send(client_socket.get_socket() , msg.c_str(), msg.length() , 0);
        send_message(msg, client_socket.get_socket());
    }
    else if ("-t")
    {
        if (!topic_changers_flag)
            return ;
        std::string msg(":" + client_socket.get_nickname() + "!~h@" + srv_ip + " MODE " + name + " -t " + "\r\n");
        topic_changers_flag = 0;
        send(client_socket.get_socket() , msg.c_str(), msg.length() , 0);
        send_message(msg, client_socket.get_socket());
    }
}

void    Server::mode_topic(int client_socket, std::string channel_name, std::string mode, std::string srv_ip)
{
    std::vector<Channel> &Channels_copy = get_channels();
    for (std::vector<Channel>::iterator p = Channels_copy.begin(); p != Channels_copy.end();p++)
    {
        if (channel_name == p->get_name())
        {
            p->update_topic_mode(clients[client_socket], mode, this->get_srv_ip());
            return ;
        }
    }
    std::string erro(":" + srv_ip + " " + std::string(ERR_NOSUCHCHANNEL) + " MODE : Channel not found !!\r\n");
    send(client_socket , erro.c_str(), erro.length() , 0);
}

void    Channel::update_invite_mode(Client client_socket, std::string mode, std::string srv_ip)
{
    int droit = search_client_in_channel(client_socket.get_nickname());
    if (droit != 1 && droit != 2)
    {
        std::string flagu = ERR_CHANOPRIVSNEEDED;
        std::string erro(":" + srv_ip + " " + flagu + " MODE :You do not have permission to update the mode +-i !!\r\n");
        if (droit != 3)
        {
            flagu = ERR_NOTONCHANNEL;
            std::string erro(":" + srv_ip + " " + flagu + " MODE :You do not belong to this channel !!\r\n");
            send(client_socket.get_socket(), erro.c_str(), erro.length() , 0);
        }
        else
            send(client_socket.get_socket(), erro.c_str(), erro.length() , 0);
    }
    else if (mode == "+i")
    {
        if (invite_changers_flag)
            return ;
        invite_changers_flag = 1;
        std::string msg(":" + client_socket.get_nickname() + "!~h@" + srv_ip + " MODE " + name + " +i " + "\r\n");
        send(client_socket.get_socket() , msg.c_str(), msg.length() , 0);
        send_message(msg, client_socket.get_socket());
    }
    else if ("-i")
    {
        if (!invite_changers_flag)
            return ;
        std::string msg(":" + client_socket.get_nickname() + "!~h@" + srv_ip + " MODE " + name + " -i " + "\r\n");
        invite_changers_flag = 0;
        send(client_socket.get_socket() , msg.c_str(), msg.length() , 0);
        send_message(msg, client_socket.get_socket());
    }
}

void    callUNKNOWNMODE(int client_socket, std::string flg, std::string srv_ip)
{
    std::string erro(":" + srv_ip + " " + std::string(ERR_UNKNOWNMODE) + " " + flg +" :is unknown mode char to me for\r\n");
    send(client_socket , erro.c_str(), erro.length() , 0);
}


void    Server::mode_invite(int client_socket, std::string channel_name, std::string mode, std::string srv_ip)
{
    std::vector<Channel> &Channels_copy = get_channels();
    for (std::vector<Channel>::iterator p = Channels_copy.begin(); p != Channels_copy.end();p++)
    {
        if (channel_name == p->get_name())
        {
            p->update_invite_mode(clients[client_socket], mode, get_srv_ip());
            return ;
        }
    }
    std::string flagu = ERR_NOSUCHCHANNEL;
    std::string erro(":" + srv_ip + " " + flagu + " MODE : Channel not found !!\r\n");
    send(client_socket , erro.c_str(), erro.length() , 0);
}

std::vector<std::string> split(std::string buffer)
{
    std::vector<std::string> result;
    std::string tmp;
    std::stringstream ss(buffer);
    while (ss >> tmp)
    {
        result.push_back(tmp);
    }
    return result;
}

void    mode_operator(int client_socket, std::string channel_name, std::string mode, std::string arg,Server * srv)
{
    std::vector<Channel> &channels = srv->get_channels();
    std::map<int , Client> &user = srv->get_clients();
    if(mode == "+o")
    {
        for(std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
        {
            if(it->get_name() == channel_name)
            {
                for(std::vector<Client>::iterator it2 = it->get_users().begin(); it2 != it->get_users().end(); ++it2)
                {
                    // if (it->search_client_in_channel(it2->get_nickname()) == 2)
                    //     return;
                    if(it2->get_nickname() == arg)
                    {
                        it->add_operator(*it2);
                        std::string msgg = ":" + user[client_socket].get_nickname() + " MODE "+ channel_name +" +o " + it2->get_nickname() + "\r\n";
                        std::string msg = ":" + user[client_socket].get_nickname() + " " + RPL_YOUREOPER + " MODE : You are now channel operator\r\n";
                        send(it2->get_socket() , msg.c_str(), msg.length() , 0);
                        send(client_socket, msgg.c_str(),msgg.length(),0);
                        it->send_message(msgg, client_socket);
                        return ;
                    }
                }
                return ;
            }
        }
        std::string erro = ":" + srv->get_srv_ip() + " " + ERR_NOTONCHANNEL +" MODE: Channel not found !!\r\n";
        send(client_socket , erro.c_str(), erro.length() , 0);
    }
    else if(mode == "-o")
    {
        for(std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
        {
            if(it->get_name() == channel_name)
            {
                for(std::vector<Client>::iterator it2 = it->get_users().begin(); it2 != it->get_users().end(); ++it2)
                {
                    // if (it->search_client_in_channel(it2->get_nickname()) == 3)
                    //     return;
                    if(it2->get_nickname() == arg)
                    {
                        it->remove_operator(*it2);
                        std::string msgg = ":" + user[client_socket].get_nickname() + " MODE "+ channel_name +" -o " + it2->get_nickname() + "\r\n";
                        std::string msg = ":" + user[client_socket].get_nickname() + " " + RPL_YOUREOPER + " MODE : You are now channel operator\r\n";
                        send(it2->get_socket() , msg.c_str(), msg.length() , 0);
                        send(client_socket, msgg.c_str(),msgg.length(),0);
                        it->send_message(msgg, client_socket);
                        return ;
                    }
                }
                return ;
            }
        }
        std::string erro = ":" + srv->get_srv_ip() + " " + ERR_NOTONCHANNEL +" MODE: Channel not found !!\r\n";
        send(client_socket , erro.c_str(), erro.length() , 0);
    }
}


void    Server::mode_key(int client_socket, std::string channel_name, std::string mode, std::string arg, std::string srv_ip)
{
    for(std::vector<Channel>::iterator s = channels.begin(); s != channels.end(); s++)
    {
        if (s->get_name() == channel_name)
        {
            std::string msg;
            if ((mode == "+k" && s->get_key_flag()) || (mode == "-k" && s->get_key_flag() == 0))
                return ;
            else if (mode == "+k" && s->get_key_flag() == 0)
            {
                s->set_key_flag(1);
                s->set_password(arg);
                msg = ":" + clients[client_socket].get_nickname() + "!~h@" + srv_ip + " MODE " + s->get_name() + " +k " + arg + "\r\n";
            }
            else if (mode == "-k" && s->get_password() == arg)
            {
                s->set_key_flag(0);
                s->set_password("");
                msg = ":" + clients[client_socket].get_nickname() + "!~h@" + srv_ip + " MODE " + s->get_name() + " -k " + "\r\n";
            }
            else if (mode == "-k" && s->get_password() != arg)
            {
                std::string erro(":" + srv_ip + " 467 " + s->get_name() + ": " + s->get_name() + " Channel key already set\r\n");
                send(client_socket , erro.c_str(), erro.length() , 0);
                return ;
            }
            send(client_socket, msg.c_str(), msg.length(), 0);
            s->send_message(msg, client_socket);
            return ;
        }
    }
}

void    Server::mode_limit(int client_socket, std::string channel_name, std::string mode, int arg, std::string srv_ip)
{
    for(std::vector<Channel>::iterator  chs = channels.begin();chs != channels.end();chs++)
    {
        if (chs->get_name() == channel_name)
        {
            std::string msg;
            if (mode == "+l" && (chs->get_limit_flag() == 0 || arg != atoi(chs->get_limit_value().c_str())))
            {
                std::string str;
                std::stringstream ss;  
                ss << arg;  
                ss >> str; 
                chs->set_limit_value(str);
                chs->set_limit_flag(1);
                msg = ":" + clients[client_socket].get_nickname() + "!~h@" + srv_ip + " MODE " + chs->get_name() + " +l " + str + "\r\n";
            }
            else if (mode == "-l" && chs->get_limit_flag() == 1)
            {
                msg = ":" + clients[client_socket].get_nickname() + "!~h@" + srv_ip + " MODE " + chs->get_name() + " -l " + "\r\n";
                chs->set_limit_value("");
                chs->set_limit_flag(0);
            }
            send(client_socket, msg.c_str(), msg.length(), 0);
            chs->send_message(msg, client_socket);
            return ;
        }
    }
}

void Server::mode_flag(int client_socket, std::string buffer)
{
    Client client_caller = clients[client_socket];
    if (client_caller.get_grade() != AUTHENTICATED)
    {
        std::string msg = ":" + get_srv_ip() + std::string(ERR_NOTREGISTERED) + " " + client_caller.get_nickname() +" :You have not registered\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
    std::vector<std::string> arg = split(buffer);
    if (arg.empty())
    {
        call_ERR_NEEDMOREPARAMS(client_socket,"MODE");
        return;
    }
    std::string channel_name = arg[0];
    arg.erase(arg.begin());
    if (arg.empty())
    {
        for(std::vector<Channel>::iterator ch = channels.begin(); ch != channels.end(); ch++)
        {
            if (channel_name == ch->get_name())
            {
                std::string options = "+";
                std::string arguments;
                if (ch->get_invite_flag())
                {
                    options += "i";
                }
                if (ch->get_key_flag())
                {
                    options += "k";
                    arguments += ch->get_password() + " ";
                }
                if (ch->get_limit_flag())
                {
                    options += "l";
                    arguments += ch->get_limit_value() + " ";
                }
                if (ch->get_topic_flag())
                {
                    options += "t";
                }
                if (options == "+")
                    options = std::string();
                std::string msg = ":" + this->get_srv_ip() + " " + std::string(RPL_CHANNELMODEIS) + " MODE " + clients[client_socket].get_nickname() + " " + channel_name + " " + options + " " + arguments + "\r\n";
                send(client_socket , msg.c_str(), msg.length() , 0);
                return ;
            }
        }
        std::string flagu = ERR_NOSUCHCHANNEL;
        std::string erro(":" + this->get_srv_ip() + " " + flagu + " MODE : " + channel_name + " Channel not found !!\r\n");
        send(client_socket , erro.c_str(), erro.length() , 0);
        return;
    }
    std::string mode = arg[0];
    arg.erase(arg.begin());
    int i=0;
    std::string option_sign;
    std::string option_param;
    while(mode[0])
    {
        if (mode[0] == '+')
            option_sign = "+";
        else if (mode[0] == '-')
            option_sign = "-";
        else if (mode[0] == 'i'){
            option_param = option_sign + mode.at(0);
            if (option_param == "+i" || option_param == "-i")
            {
                mode_invite(client_socket, channel_name, option_param, get_srv_ip());
            }
        }
        else if (mode[0] == 't'){
            option_param = option_sign + mode.at(0);
            if (option_param == "+t" || option_param == "-t")
            {
                mode_topic(client_socket, channel_name, option_param, get_srv_ip());
            }
        }
        else if (mode[0] == 'l'){
            option_param = option_sign + mode.at(0);
            if (option_param == "+l" || option_param == "-l")
            {
                if (arg.size() < 1 && option_param == "+l")
                {
                    std::string flagu = ERR_INVALIDMODEPARAM;
                    std::string erro(":" + this->get_srv_ip() + " " + flagu + " MODE : " + channel_name + " l " + " You must specify a parameter for the limit mode !!\r\n");
                    send(client_socket , erro.c_str(), erro.length() , 0);
                }
                else if (atoi(arg[0].c_str()) < 1 && option_param == "+l")
                {
                    std::string flagu = ERR_INVALIDMODEPARAM;
                    std::string erro(":" + this->get_srv_ip() + " " + flagu + " MODE : " + channel_name + " l " + arg[0] + " Invalid limit mode parameter\r\n");
                    send(client_socket , erro.c_str(), erro.length() , 0);

                } 
                else
                {
                    mode_limit(client_socket, channel_name, option_param, atoi(arg[0].c_str()), get_srv_ip());
                    if (option_param == "+l")
                        arg.erase(arg.begin());
                }
            }
        }
        else if (mode[0] == 'k'){
            option_param = option_sign + mode.at(0);
            if (option_param == "+k" || option_param == "-k")
            {
                if (arg.size() < 1)
                {
                    std::string flagu = ERR_INVALIDMODEPARAM;
                    std::string erro(":" + this->get_srv_ip() + " " + flagu + " MODE : " + channel_name + " k " + " You must specify a parameter for the key mode !!\r\n");
                    send(client_socket , erro.c_str(), erro.length() , 0);
                }
                else
                {
                    mode_key(client_socket, channel_name, option_param,arg[0], get_srv_ip());
                    arg.erase(arg.begin());
                }
            }
        }
        else if (mode[0] == 'o'){
            option_param = option_sign + mode.at(0);
            if (option_param == "+o" || option_param == "-o")
            {
                if (arg.size() < 1)
                    call_ERR_NEEDMOREPARAMS(client_socket,"MODE");
                else if (check_if_user_exist(arg[0]) == 0)
                    call_ERR_NOSUCHNICK(client_socket, arg[0]);
                else
                {
                    mode_operator(client_socket, channel_name, option_param, arg[0], this);
                    arg.erase(arg.begin());
                }
            }
        }
        else if (mode[0] != 'n' && mode[0] != 's')
            callUNKNOWNMODE(client_socket, mode, get_srv_ip());
        mode.erase(mode.begin());
        i++;
    }
}

// i t k ol
//   Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>]
//                [<ban mask>]



    // i - invite-only channel flag;
    // t - topic settable by channel operator only flag; => scayho
    // k - set a channel key (password).
    // o - give/take channel operator privileges;
    // l - set the user limit to channel;

// +ilkjt