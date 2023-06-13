#include "Tools.hpp"
#include "Client.hpp"

// std::vector<std::string> split(std::string str)
// {
//     std::vector<std::string> result;
//     std::string tmp = "";
//     for (int i = 0; i < str.length(); i++)
//     {
//         if (str[i] == ' ')
//         {
//             result.push_back(tmp);
//             tmp = "";
//         }
//         else
//             tmp += str[i];
//     }
//     result.push_back(tmp);
//     return result;
// }

// void    pp_vector(std::vector<Client> &tmp)
// {
//     unsigned int i = 0;
//     for (std::vector<Client>::iterator o = tmp.begin(); o != tmp.end(); o++)
//     {
//         std::cout << "Client number : " << i << *o ;
//     }
// }

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