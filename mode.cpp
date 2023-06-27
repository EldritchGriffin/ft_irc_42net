#include "server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

void Server::mode_flag(int client_socket, std::string buffer)
{
    std::string channel_name = buffer.substr(0,buffer.find(' '));
    buffer.erase(0,buffer.find(' '));
    std::string mode = buffer.substr(0,buffer.find(' ')+2);
    buffer.erase(0,buffer.find(' ')+2);

    if (buffer.empty())
    {
        call_ERR_NEEDMOREPARAMS(client_socket);
        return;
    }
    if (mode == "-i" || mode == "+i")
    {

    }
    else if (mode == "-t" || mode == "+t")
    {

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