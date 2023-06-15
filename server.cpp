#include "server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Tools.hpp"



// ++++++++++++++++++++++++++++++++DEBUGIN FUNCTIONS SESSION+++++++++++++++++++++++++++++++++++++

Server::Server(int port, std::string password)
{
    this->srv_port = port;
    this->srv_password = password;
}

Server::~Server(){}

void Server::init_server()
{
    pollfd pfd;
    this->srv_socket = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(this->srv_socket, F_SETFL, O_NONBLOCK);

    if (this->srv_socket == -1)
        throw std::runtime_error("Error: could not create socket, retrying...");

    int reuseaddr = 1;
    if (setsockopt(this->srv_socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)) == -1)
        throw std::runtime_error("Error: could not set socket options, retrying...");

    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(this->srv_port);
    srv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->srv_socket, reinterpret_cast<struct sockaddr*>(&srv_addr), sizeof(srv_addr)) == -1)
        throw std::runtime_error("Error: could not bind socket, retrying...");

    if (listen(this->srv_socket, SOMAXCONN) == -1)
        throw std::runtime_error("Error: could not listen on socket, retrying...");
    
    pfd.fd = this->srv_socket;
    pfd.events = POLLIN;
    pfd.revents = 0;
    this->pollfds.push_back(pfd);
}

void Server::accept_client()
{
    pollfd pfd;

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int client_socket = accept(this->srv_socket, (struct sockaddr*)&client_addr, &client_addr_size);
    if(client_socket == -1)
    {
        std::cerr << "Error: could not accept client." << std::endl; return;
    }
    std::cout << "Client connected." << std::endl;
    Client client(client_socket, client_addr);
    this->clients.insert(std::pair<int, Client>(client_socket, client));

    pfd.fd = client_socket;
    pfd.events = POLLIN;
    pfd.revents = 0;

    this->pollfds.push_back(pfd);
}

std::string casa_rm(std::string p)
{
    if (p[p.length() - 2] == '\r' && p[p.length() - 1] == '\n')
        return (p.substr(0, p.length() - 2));
    if (p[p.length() - 1] == '\n')
        return (p.substr(0, p.length() - 1));
    return (p);
}

std::string Server::client_request(int client_socket)
{
    char buffer[1024];
    int bytes_read = recv(client_socket, buffer, 1024, 0);
    buffer[bytes_read] = '\0';
    if (bytes_read == -1)
    {
        std::cerr << "Error: could not read from client." << std::endl;
        close(client_socket);
        return std::string();
    }
    else if (bytes_read == 0)
    {
        std::cout << "Client disconnected." << std::endl;
        close(client_socket);
        return std::string();
    }
    return (casa_rm(std::string(buffer)));//TODO rename casa_rm
}



std::ostream & operator << (std::ostream & o, Client const & rhs)
{
    o << "nickname :" << rhs.get_nickname();
    o << " username :" << rhs.get_username();
    o << " realname :" << rhs.get_realname() << std::endl;
    return (o);
}

std::ostream & operator << (std::ostream & o, Channel const & rhs)
{
    o << "nickname :" << rhs.get_name() << std::endl;
    // o << " username :" << rhs.get_username();
    // o << " realname :" << rhs.get_realname() << std::endl;
    return (o);
}

void    pp_ch(std::vector<Channel> &tmp)
{
    unsigned int i = 0;
    for (std::vector<Channel>::iterator o = tmp.begin(); o != tmp.end(); o++)
    {
        i++;
        // std::cout << "Channel number : " << i << " " << *o;
    }
}

void    pp_pp(std::map<int, Client> &tmp)
{
    unsigned int i = 0;
    for (std::map<int, Client>::iterator o = tmp.begin(); o != tmp.end(); o++)
    {
        i++;
        // std::cout << "Client number : " << i << " " << o->second ;
    }
}

void Server::msg(int client_socket, std::string buffer)
{
    Client client_caller = clients[client_socket];
    std::cout <<"LIMECHAT SENT:" << buffer << "|" << std::endl;
    std::string channel_name = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, channel_name.length() + 1);
    if (buffer[0] == ':')
        buffer.erase(0, 1);
    std::vector<std::string> target_names = split_multiple_targets(channel_name);
    for (unsigned int i = 0; i < target_names.size(); i++)
    {
        pp_ch(channels);
        channel_name = target_names[i];
        for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
        {
            if(it->get_name() == channel_name)
            {
                it->send_message(client_caller.get_nickname() + " :" + buffer + "\n", client_socket);
            }
        }
        pp_pp(clients);
        for(std::map<int, Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
        {
            if(it->second.get_nickname() == channel_name)
            {
                std::string msg = ":" + client_caller.get_nickname() + " PRIVMSG " + channel_name + " :" + buffer + "\r\n";
                send(it->second.get_socket(), msg.c_str(), msg.length(), 0);
            }
        }
    }
    return;
}

void Server::kick_cmd(int client_socket, std::string buffer)
{
    std::string ch = buffer.substr(0,buffer.find(" "));
    buffer.erase(0,ch.length()+1);
    std::string user = buffer.substr(0,buffer.find(" "));
    buffer.erase(0,user.length()+1);
    std::string reason = buffer.substr(0);
    for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
    {
        if(it->get_name() == ch)
        {
            if(it->get_admin().get_nickname() == this->clients[client_socket].get_nickname())
            {
                it->kick_user(user);
                return;
            }
            else
            {
                return;
            }
        }
    }
}

void Channel::invite_user(std::string user)
{
    for(std::vector<Client>::iterator it = this->users.begin(); it != this->users.end(); ++it)
    {
        if(it->get_nickname() == user)
        {
            return;
        }
    }
}

void Server::invite_cmd(int client_socket, std::string buffer){
    std::string user = buffer.substr(0,buffer.find(" "));
    buffer.erase(0,user.length()+1);
    std::string ch = buffer.substr(0,buffer.find(" "));

    for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
    {
        if(it->get_name() == ch)
        {
            if(it->get_admin().get_nickname() == this->clients[client_socket].get_nickname())
            {
                it->invite_user(user);
                return;
            }
            else
            {
                return;
            }
        }
    }
}

void Server::part_cmd(int client_socket,std::string buffer){
    std::string ch = buffer.substr(0,buffer.find(" "));
    for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
    {
        if(it->get_name() == ch)
        {
            it->remove_user(this->clients[client_socket]);
            return;
        }
    }
}

int Channel::search_client_in_channel(int   client_socket)
{
    if (admin.get_socket() == client_socket)
        return (1);
    for(std::vector<Client>::iterator ch = users.begin(); ch != users.end(); ch++)
    {
        if (ch->get_socket() == client_socket)
            return (3);
    }
    for(std::vector<Client>::iterator ch = operators.begin(); ch != operators.end(); ch++)
    {
        if (ch->get_socket() == client_socket)
            return (2);
    }
    return (0);
}

int Channel::search_client_in_channel(std::string client_name)
{
    if (admin.get_nickname() == client_name)
        return (1);
    for(std::vector<Client>::iterator ch = users.begin(); ch != users.end(); ch++)
    {
        if (ch->get_nickname() == client_name)
            return (3);
    }
    for(std::vector<Client>::iterator ch = operators.begin(); ch != operators.end(); ch++)
    {
        if (ch->get_nickname() == client_name)
            return (2);
    }
    return (0);
}

void    Server::get_channel_topic(std::string channel_name, int client_socket)
{
    for(std::vector<Channel>::iterator ch = channels.begin(); ch != channels.end(); ch++)
    {
            int client_existens = ch->search_client_in_channel(client_socket);
                std::cout << client_existens << "ddd\n";
        if (ch->get_name() == channel_name) //TOPIC #test 
        {//:<user> TOPIC <channel> :<topic>\r\n
            if (client_existens)
            {//:dan!d@Clk-830D7DDC TOPIC #v3 :
                // std::cout << "ddd\n";
                // std::string msg = " TOPIC #" + ch->get_topic() + "\r\n";
                std::string message_sender = clients[client_socket].get_nickname();
                std::string msg = ":" + message_sender + "!" + message_sender[0] + "@localhost TOPIC " + channel_name + "\r\n";
                send(client_socket, (msg).c_str(), msg.length(), 0);
            }
            return;
        }
    }
    send(client_socket, "no channel\r\n", 12, 0);

    //error channel not found
}

void    Server::set_channel_topic(int client_socket, std::string channel_name, std::string buffer)
{
    for(std::vector<Channel>::iterator ch = channels.begin(); ch != channels.end(); ch++)
    {
        std::cout << "channel name in the queue :" + ch->get_name() << std::endl;
        if (ch->get_name() == channel_name)
        {
            int client_existens = ch->search_client_in_channel(client_socket);
            if (client_existens > 0 && client_existens < 3)
            {//:dan!d@Clk-830D7DDC TOPIC #v3 :This is a cool channel!!
                ch->set_topic(buffer);
                std::string user_nam = clients[client_socket].get_nickname();
                std::string msg = ":" + user_nam+"!"+user_nam[0]+"@localhost TOPIC " + channel_name + " :" +buffer + "\r\n";
                std::cout << msg << std::endl;
                send(client_socket, msg.c_str(), msg.length(), 0);
            }
            return ;
        }
    }
    //error channel not found
}

void Server::topic_cmd(int client_socket, std::string buffer)
{// if givven the 3rd arguments , the topic changes and broadcast it to the users , if there was no 3rd argument the corrent topic is returned
    std::cout << "command is :" + buffer << std::endl;
    std::string channel_name = buffer.substr(0, buffer.find(' '));
    buffer.erase(0, buffer.find(' '));
    std::cout <<  "channel name is :" + channel_name + " command is :" + buffer << std::endl;
    if (buffer.empty())
    {//TOPIC #test 
        std::cout << "empty" << std::endl;
        get_channel_topic(channel_name, client_socket);
    }
    else
    {
        std::cout << "not empty" << std::endl;
        set_channel_topic(client_socket, channel_name, buffer);
    }
}

void Server::handle_input(int client_socket)
{   
    std::string buffer = this->client_request(client_socket);
    if(buffer.empty())
    {
        return;
    }
    std::cout << "|" << buffer << "|" << std::endl;
    std::string command = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, command.length() + 1);
    if(command == "PART")
    {
        this->part_cmd(client_socket, buffer);
    }
    if(command == "PASS")
    {
        this->pass_cmd(client_socket, buffer);
    }
    else if(command == "INVITE")
    {
        this->invite_cmd(client_socket, buffer);
    }
    else if(command == "NICK")
    {
        this->nick_cmd(client_socket, buffer);
    }
    else if(command == "TOPIC")
    {
        this->topic_cmd(client_socket, buffer);
    }
    else if(command == "USER")
    {
        this->user_cmd(client_socket, buffer);
    }
    else if (command == "KICK")
    {
        this->kick_cmd(client_socket, buffer);
    }
    else if(command == "JOIN")
    {
        this->join_cmd(client_socket, buffer);
        pp_ch(channels);
    }
    else if(command == "MSG" || command == "PRIVMSG")
    {
        this->msg(client_socket, buffer);
    }
    else
    {
    }
}

void Server::poll_handler()
{
    for (size_t i = 0; i < this->pollfds.size(); i++)
    {
        if (this->pollfds[i].revents & POLLIN)
        {
            if (this->pollfds[i].fd == this->srv_socket)
            {
                this->accept_client();
            }
            else
            {
                handle_input(this->pollfds[i].fd);
            }
        }
    }
}

void Server::run()
{
    int poll_flag = 0;
    this->init_server();
    std::cout << "Server is listening on port " << this->srv_port << std::endl;

    while (true)
    {
        poll_flag = poll(this->pollfds.data(), this->pollfds.size(), 0);
        if(poll_flag == -1)
        {
            std::cerr << "Error: could not poll." << std::endl;
            continue;
        }
        if(poll_flag > 0)
        {
            this->poll_handler();
        }
    }
}

//setters

void Server::set_srv_socket(int srv_socket)
{
    this->srv_socket = srv_socket;
}

void Server::set_srv_port(int srv_port)
{
    this->srv_port = srv_port;
}

void Server::set_pollfds(std::vector<struct pollfd> pollfds)
{
    this->pollfds = pollfds;
}

void Server::set_clients(std::map<int, Client> clients)
{
    this->clients = clients;
}

void Server::set_channels(std::vector<Channel> channels)
{
    this->channels = channels;
}

//Getters
int Server::get_srv_socket() const
{
    return (this->srv_socket);
}

int Server::get_srv_port() const
{
    return (this->srv_port);
}

std::vector<struct pollfd> Server::get_pollfds() const
{
    return (this->pollfds);
}

std::map<int, Client> Server::get_clients() const
{
    return (this->clients);
}

std::vector<Channel> Server::get_channels() const
{
    return (this->channels);
}


//TODO modify the messages sent to the users when executing authentification commands;
//TODO find a way to mark a user as authentificated after entering all auth commands;
//TODO remove the testing msg command;
//TODO polish and finish the functions you started (talking to myself here (griffin) );
//TODO this one is for drari, find something to work on, anything i started, ill finish, (dont modify code only add your own stuff, to avoid conflict);
// GGWP GUYS IRC DONE IN A LITTLE BIT;