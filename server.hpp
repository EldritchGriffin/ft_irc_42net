#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Client.hpp"
#include <exception>


//TODO create whatever classes we need to make the server work;

class Client;

class Server
{
    private:
        int srv_socket;
        int srv_port;
        std::string srv_password;
        std::vector<Client> clients;
    public:
        Server(int port, std::string password);
        ~Server();
        void init_server();
        void accept_client();
        void run();
};