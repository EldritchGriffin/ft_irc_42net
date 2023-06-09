#pragma once

#include "Server.hpp"

enum clientState
{
    GUEST,
    USER,
    ADMIN
};


//TODO client will need further additional member variables;

class Client
{
    private:
        int socket;
        std::string name;
        std::string ip;
        int port;
        int grade;
    public:
        Client(int socket, std::string name, std::string ip, int port);
        ~Client();        
};