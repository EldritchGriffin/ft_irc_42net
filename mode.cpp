#include "server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

void Server::mode_cmd(int client_socket, std::string buffer)
{
    Client client_caller = clients[client_socket];
    std::string channel_name = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, channel_name.length() + 1);
    std::string mode = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, mode.length() + 1);
    std::string user = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, user.length() + 1);
    //TODO we need to know how many paramet we need for this mode:by absela
}