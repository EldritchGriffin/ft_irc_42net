#include "server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "numeric_replies.hpp"




void    Channel::update_topic_mode(Client client_socket, std::string mode) // TODO reread the options if that works as needed && change the replay numbers
{
    int droit = search_client_in_channel(client_socket.get_nickname());
    if (droit != 1 && droit != 2)
    {
        std::string erro(":IRC.srv.ma 442 TOPIC :You do not have permission to update the mode +-t !!\r\n");
        if (droit != 3)
        {
            std::string erro(":IRC.srv.ma 442 TOPIC :You do not belong to this channel !!\r\n");
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
        std::string msg(":" + client_socket.get_nickname() + "!~h@localhost MODE " + name + " +t " + "\r\n");
        send(client_socket.get_socket() , msg.c_str(), msg.length() , 0);
        send_message(msg, client_socket.get_socket());
    }
    else if ("-t")
    {
        if (!topic_changers_flag)
            return ;
        std::string msg(":" + client_socket.get_nickname() + "!~h@localhost MODE " + name + " -t " + "\r\n");
        topic_changers_flag = 0;
        send(client_socket.get_socket() , msg.c_str(), msg.length() , 0);
        send_message(msg, client_socket.get_socket());
    }
}

void    Server::mode_topic(int client_socket, std::string channel_name, std::string mode)
{
    std::vector<Channel> &Channels_copy = get_channels();
    // Channel *target_channel;
    for (std::vector<Channel>::iterator p = Channels_copy.begin(); p != Channels_copy.end();p++)
    {
        // std::cout << channel_name + " ?= "+ p->get_name() << std::endl; // debugging purpose
        if (channel_name == p->get_name())
        {
            p->update_topic_mode(clients[client_socket], mode);
            return ;
        }
    }
    std::string erro(":IRC.srv.ma 442 MODE : Channel not found !!\r\n");
    send(client_socket , erro.c_str(), erro.length() , 0);
    //TODO error no such channel
}

void    Channel::update_invite_mode(Client client_socket, std::string mode)
{
    int droit = search_client_in_channel(client_socket.get_nickname());
    if (droit != 1 && droit != 2)
    {
        std::string erro(":IRC.srv.ma 442 MODE :You do not have permission to update the mode +-i !!\r\n");
        if (droit != 3)
        {
            std::string erro(":IRC.srv.ma 442 MODE :You do not belong to this channel !!\r\n");
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
        std::string msg(":" + client_socket.get_nickname() + "!~h@localhost MODE " + name + " +i " + "\r\n");
        send(client_socket.get_socket() , msg.c_str(), msg.length() , 0);
        send_message(msg, client_socket.get_socket());
    }
    else if ("-i")
    {
        if (!invite_changers_flag)
            return ;
        std::string msg(":" + client_socket.get_nickname() + "!~h@localhost MODE " + name + " -i " + "\r\n");
        invite_changers_flag = 0;
        send(client_socket.get_socket() , msg.c_str(), msg.length() , 0);
        send_message(msg, client_socket.get_socket());
    }
}

void    callUNKNOWNMODE(int client_socket, std::string flg)
{
    std::string erro(":IRC.srv.ma 472 " + flg +" :is unknown mode char to me for\r\n");
    send(client_socket , erro.c_str(), erro.length() , 0);
}


void    Server::mode_invite(int client_socket, std::string channel_name, std::string mode)
{
    std::vector<Channel> &Channels_copy = get_channels();
    // Channel *target_channel;
    for (std::vector<Channel>::iterator p = Channels_copy.begin(); p != Channels_copy.end();p++)
    {
        // std::cout << channel_name + " ?= "+ p->get_name() << std::endl; // debugging purpose
        if (channel_name == p->get_name())
        {
            p->update_invite_mode(clients[client_socket], mode);
            return ;
        }
    }
    std::string erro(":IRC.srv.ma 442 MODE : Channel not found !!\r\n");
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
    if(mode == "+o")
    {
        for(std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
        {
            if(it->get_name() == channel_name)
            {
                for(std::vector<Client>::iterator it2 = it->get_users().begin(); it2 != it->get_users().end(); ++it2)
                {
                    if(it2->get_nickname() == arg)
                    {
                        it->add_operator(*it2);
                        std::string msg(":IRC.srv.ma 381 MODE : You are now channel operator\r\n");
                        send(it2->get_socket() , msg.c_str(), msg.length() , 0);
                        return ;
                    }
                }
                return ;
            }
        }
        std::string erro(":IRC.srv.ma 442 MODE : Channel not found !!\r\n");
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
                    if(it2->get_nickname() == arg)
                    {
                        it->remove_operator(*it2);
                        std::string msg(":IRC.srv.ma 381 MODE : You are no longer channel operator\r\n");
                        send(it2->get_socket() , msg.c_str(), msg.length() , 0);
                        return ;
                    }
                }
                return ;
            }
        }
        std::string erro(":IRC.srv.ma 442 MODE : Channel not found !!\r\n");
        send(client_socket , erro.c_str(), erro.length() , 0);
    }
}


void    mode_key(int client_socket, std::string channel_name, std::string mode, std::string arg)
{
    (void)client_socket;
    (void)channel_name;
    (void)mode;
    (void)arg;
    std::cout << "mode " + mode + " with parameter " + arg << std::endl;
}

void    mode_limit(int client_socket, std::string channel_name, std::string mode, std::string arg)
{
    (void)client_socket;
    (void)channel_name;
    (void)mode;
    (void)arg;
    std::cout << "mode " + mode + " with parameter " + arg << std::endl;
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
    // std::string mode = arg[0];
    // flags flg = init_flag(client_socket, mode,arg);
    // arg.erase(arg.begin());
    if (arg.empty())
    {
        call_ERR_NEEDMOREPARAMS(client_socket,"MODE");
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
                mode_invite(client_socket, channel_name, option_param);
            }
        }
        else if (mode[0] == 't'){
            option_param = option_sign + mode.at(0);
            if (option_param == "+t" || option_param == "-t")
            {
                mode_topic(client_socket, channel_name, option_param);
            }
        }
        else if (mode[0] == 'l'){
            option_param = option_sign + mode.at(0);
            if (option_param == "+l" || option_param == "-l") // CHECK IF THERE IS A PARAMETER FOR THE COMMAND
            {
                if (arg.size() < 1)
                    call_ERR_NEEDMOREPARAMS(client_socket,"MODE"); // update it for the right message
                // else if () // check if the numbers are degite
                //     call_ERR_NEEDMOREPARAMS(client_socket,"MODE"); // update it for the right message
                else
                {
                    mode_limit(client_socket, channel_name, option_param, arg[0]);
                    arg.erase(arg.begin());
                }
            }
        }
        else if (mode[0] == 'k'){
            option_param = option_sign + mode.at(0);
            if (option_param == "+k" || option_param == "-k") // CHECK IF THERE IS A PARAMETER FOR THE COMMAND
            {
                if (arg.size() < 1)
                    call_ERR_NEEDMOREPARAMS(client_socket,"MODE"); // update it for the right message
                else
                {
                    mode_key(client_socket, channel_name, option_param,arg[0]);
                    arg.erase(arg.begin());
                }
            }
        }
        else if (mode[0] == 'o'){
            option_param = option_sign + mode.at(0);
            if (option_param == "+o" || option_param == "-o") // CHECK IF USER EXIST AND THERE IS A PARAMETER FOR THE COMMAND
            {
                if (arg.size() < 1)
                    call_ERR_NEEDMOREPARAMS(client_socket,"MODE"); // update it for the right message
                else if (check_if_user_exist(arg[0]) == 0)
                    call_ERR_NEEDMOREPARAMS(client_socket,"MODE"); // update it for the right message
                else
                {
                    mode_operator(client_socket, channel_name, option_param, arg[0], this);
                    arg.erase(arg.begin());
                }
            }
        }
        else
            callUNKNOWNMODE(client_socket, mode);
        mode.erase(mode.begin());
        i++;
    }
}
// void Server::mode_flag(int client_socket, std::string buffer)
// {
//     std::string channel_name = buffer.substr(0,buffer.find(' '));
//     buffer.erase(0,buffer.find(' '));
//     std::string mode = buffer.substr(1,buffer.find(' ')+3);
//     buffer.erase(0,buffer.find(' ')+2);

//     // if (buffer.empty())
//     // {
//     //     call_ERR_NEEDMOREPARAMS(client_socket);
//     //     return;
//     // }

//     std::cout << mode << std::endl;
//     if (mode == "-i" || mode == "+i")
//     {
        // mode_invite(client_socket, channel_name, mode);
//     }
//     else if (mode == "-t" || mode == "+t")
//     {
//         mode_topic(client_socket, channel_name, mode);
//     }
//     else if (mode == "-k" || mode == "+k")
//     {

//     }
//     else if (mode == "-o" || mode == "+o")
//     {

//     }
//     else if (mode == "-l" || mode == "+l")
//     {

//     }
// }

// i t k ol
//   Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>]
//                [<ban mask>]



    // i - invite-only channel flag;
    // t - topic settable by channel operator only flag; => scayho
    // k - set a channel key (password).
    // o - give/take channel operator privileges;
    // l - set the user limit to channel;

// +ilkjt