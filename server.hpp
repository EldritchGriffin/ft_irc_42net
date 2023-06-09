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


//TODO create whatever classes we need to make the server work;

class Client;

class Server
{
    private:
        int srv_socket;
        int srv_port;
        std::vector<Client> clients;
    public:
        Server(int port);
        ~Server();
        void run();
};