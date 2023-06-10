#pragma once

#include "server.hpp"

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
        sockaddr_in addr;
        int grade;
    public:
        Client(int socket, sockaddr_in client_addr);
        int get_grade() const;
        void set_grade(int grade);
        ~Client();        
};