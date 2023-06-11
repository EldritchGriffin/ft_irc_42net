#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

enum clientState
{
    GUEST,
    USER,
    OPERATOR,
    ADMIN
};


//TODO client will need further additional member variables;

class Channel;
class Client
{
    private:
        int socket;
        sockaddr_in addr;
        int grade;
    public:
        Client();
        Client(int socket, sockaddr_in client_addr);
        int get_grade() const;
        void set_grade(int grade);
        ~Client();        
};