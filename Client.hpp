#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

enum States
{
    AUTHENTICATED,
    NOT_AUTHENTICATED,

    PASS,
    NOPASS,

    NICK,
    NONICK,

    USER,
    NOUSER
};


//TODO client will need further additional member variables;

class Channel;
class Client
{
    private:
        std::string nickname;
        std::string username;
        std::string hostname;
        std::string servername;
        std::string realname;
        std::string buffer;
        
        int socket;
        sockaddr_in addr;

        int pass_state;
        int nick_state;
        int user_state;
        int grade;

    public:
        Client();
        Client(int socket, sockaddr_in client_addr);

        void set_nickname(std::string nickname);
        void set_username(std::string username);
        void set_realname(std::string realname);
        void set_grade(int grade);
        void set_pass_state(int state);
        void set_nick_state(int state);
        void set_user_state(int state);
        void set_hostname(std::string hostname);
        void set_servername(std::string servername);

        int get_socket() const;

        std::string get_nickname() const;
        std::string get_username() const;
        std::string get_realname() const;
        std::string get_hostname() const;
        std::string get_servername() const;

        int get_pass_state() const;
        int get_grade() const;
        int get_nick_state() const;
        int get_user_state() const;


        ~Client();        
};
