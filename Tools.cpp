#include "Tools.hpp"
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

void print_vector(std::vector<Client> v)
{
    for (std::vector<Client>::iterator it = v.begin(); it != v.end(); ++it)
    {
        std::cout << "----------- " << it->get_nickname() << std::endl;
    }
}