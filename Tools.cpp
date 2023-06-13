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