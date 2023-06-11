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

enum passState
{
    PASS,
    NOPASS
};


//TODO client will need further additional member variables;

class Channel;
class Client
{
    private:
        std::string nickname;
        std::string username;
        std::string realname;
        
        int socket;
        sockaddr_in addr;
        int grade;
        int pass_state;
    public:
        Client();
        Client(int socket, sockaddr_in client_addr);

        void set_pass_state();
        void set_grade(int grade);
        void set_nickname(std::string nickname);
        void set_username(std::string username);
        void set_realname(std::string realname);

        int get_pass_state() const;
        int get_grade() const;
        int get_socket() const;
        std::string get_nickname() const;
        std::string get_username() const;
        std::string get_realname() const;

        ~Client();        
};