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
#include <ifaddrs.h>
#include <sys/types.h>
#include <netdb.h>

//TODO create whatever classes we need to make the server work;

class Client;
class Channel;


class Server
{
    private:
        //private members
        int srv_socket;
        int srv_port;
        struct sockaddr_in srv_addr;
        std::string srv_password;
        std::vector<pollfd> pollfds;
        std::map<int ,Client> clients;
        std::vector<Channel> channels;


        //private methods

        void auth_client(int client_socket);

        void pass_cmd(int client_socket, std::string buffer);
        void join_cmd(int client_socket, std::string buffer); 
        void nick_cmd(int client_socket, std::string buffer); 
        void user_cmd(int client_socket, std::string buffer); 
        void kick_cmd(int client_socket, std::string buffer);
        void part_cmd(int client_socket,std::string buffer);
        void kill_cmd(int client_socket, std::string buffer);
        void mode_flag(int client_socket, std::string buffer);
        void list_cmd(int client_socket,std::string buffer);
        void invite_user(std::string user);
        void invite_cmd(int client_socket, std::string buffer);
        void msg(int client_socket, std::string buffer);

        void init_server();
        void poll_handler();
        void accept_client();
        std::string client_request(int client_socket);
        void handle_input(int client_socket);
        void    get_channel_topic(std::string channel_name, int client_socket);
        void    set_channel_topic(int client_socket, std::string channel_name, std::string buffer);
    public:

        Server(int port, std::string password);
        ~Server();
        void run();

        //getters
        int get_srv_socket() const;
        int get_srv_port() const;
        std::string get_srv_password() const;
        std::string get_srv_ip() const;
        std::vector<pollfd> get_pollfds() const;
        std::map<int ,Client> &get_clients();
        std::vector<Channel> &get_channels();

        //setters
        void set_srv_socket(int srv_socket);
        void set_srv_port(int srv_port);
        void set_pollfds(std::vector<pollfd> pollfds);
        void set_clients(std::map<int ,Client> clients);
        void set_channels(std::vector<Channel> channels);
        void topic_cmd(int client_socket, std::string buffer);


        std::string get_client_nick_by_socket(int client_socket);
        void call_ERR_NEEDMOREPARAMS(int client_socket);
        int check_if_on_channel(int client_socket, std::string channel_name);
        void    call_ERR_NOTONCHANNEL(int client_socket);
        void    call_ERR_CHANOPRIVSNEEDED(int client_socket, std::string channel_name);
        void    mode_topic(int client_socket, std::string buffer, std::string mode);
        void    unset_channel_topic(std::string channel_name, int client_socket);
};

void    pp_ch(std::vector<Channel> &tmp);
void    pp_pp(std::map<int, Client> &tmp);