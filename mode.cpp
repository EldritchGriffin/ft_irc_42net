#include "server.hpp"
#include "Channel.hpp"
#include "Client.hpp"



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
    std::string erro(":IRC.srv.ma 442 TOPIC : Channel not found !!\r\n");
    send(client_socket , erro.c_str(), erro.length() , 0);
    //TODO error no such channel
}


// handle ERR_NOSUCHCHANNEL in case false channel;
void Server::mode_flag(int client_socket, std::string buffer)
{
    std::string channel_name = buffer.substr(0,buffer.find(' '));
    buffer.erase(0,buffer.find(' '));
    std::string mode = buffer.substr(1,buffer.find(' ')+3);
    buffer.erase(0,buffer.find(' ')+2);

    // if (buffer.empty())
    // {
    //     call_ERR_NEEDMOREPARAMS(client_socket);
    //     return;
    // }

    std::cout << mode << std::endl;
    if (mode == "-i" || mode == "+i")
    {

    }
    else if (mode == "-t" || mode == "+t")
    {
        mode_topic(client_socket, channel_name, mode);
    }
    else if (mode == "-k" || mode == "+k")
    {

    }
    else if (mode == "-o" || mode == "+o")
    {

    }
    else if (mode == "-l" || mode == "+l")
    {

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