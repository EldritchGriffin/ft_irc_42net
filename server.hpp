#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <exception>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>

//TODO create whatever classes we need to make the server work;

class Client;
class Channel;

class Server
{
    private:
        //private members
        int srv_socket;
        int srv_port;
        std::string srv_password;
        std::vector<pollfd> pollfds;
        std::map<int ,Client> clients;
        std::vector<Channel> channels;

        //private methods

        void join_cmd(int client_socket, std::string buffer); // remove later;

        void init_server();
        void poll_handler();
        void accept_client();
        std::string client_request(int client_socket);
        void authentificate_client(int client_socket);
        void handle_input(int client_socket);
    public:
        Server(int port, std::string password);
        ~Server();
        void run();
};