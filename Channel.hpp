#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "Client.hpp"

class Client;
class Server;

class Channel{
    private:
        std::string name;
        std::string topic;
        std::string password;
        Client      admin;
        int         topic_changers_flag;
        std::vector<Client> users;
        std::vector<Client> invited;
        std::vector<Client> operators;
    public:
        Channel();
        Channel(std::string name, std::string topic);
        ~Channel();

        void set_name(std::string name);
        void set_topic(std::string topic);
        void set_topic_flag(int g);
        void set_admin(Client admin);
        void set_password(std::string password);
        void set_users(std::vector<Client> users);

        std::string get_name() const;
        int get_topic_flag() const;
        std::string get_topic() const;
        std::string get_password() const;
        std::vector<Client> &get_users();
        Client get_admin() const;

        void add_user(Client user);
        void kick_user(std::string user);
        void invite_user(std::string user);
        std::string list_cmd(std::string msg);
        void add_operator(Client user);

        void remove_user(Client user);
        void remove_operator(Client user);

        void send_message(std::string message,int client_socket);


        int search_client_in_channel(int   client_socket);
        int search_client_in_channel(std::string client_name);

        void    update_topic_mode(Client client_socket, std::string mode);
        int check_if_user_exist_in_channel(std::string user);
        void add_invited_user(Client &user, std::string cmd, int client_socket);
};


