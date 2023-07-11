#include "Tools.hpp"
#include "server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

std::vector<std::string> split_multiple_targets(std::string channel_name)
{
    std::vector<std::string> target_names;
    if (channel_name.find(',', 0) != std::string::npos)
    {
        while (channel_name.find(',', 0) != std::string::npos)
        {
            std::string tmp = channel_name.substr(0, channel_name.find(',', 0));
            channel_name.erase(0, channel_name.find(',', 0) + 1);
            target_names.push_back(tmp);
        }
        std::string tmp = channel_name.substr(0);
        // channel_name.erase(0, channel_name.find(' ', 0));
        target_names.push_back(tmp);
    }
    else
        target_names.push_back(channel_name);
    return (target_names);
}

int Server::check_if_channel_exist(std::string n)
{
    for(std::vector<Channel>::iterator i = channels.begin() ; channels.end() != i ; i++)
    {
        if (i->get_name() == n)
            return (1);
    }
    return (0);
}