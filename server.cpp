#include "server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Tools.hpp"
#include "numeric_replies.hpp"


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
        std::cout << "Channel number : " << i << " " << *o;
    }
}

void    pp_pp(std::map<int, Client> &tmp)
{
    unsigned int i = 0;
    for (std::map<int, Client>::iterator o = tmp.begin(); o != tmp.end(); o++)
    {
        i++;
        std::cout << "Client number : " << i << " " << o->second ;
    }
}


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

    this->srv_addr = srv_addr;

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

void Server::msg(int client_socket, std::string buffer)
{
    Client client_caller = clients[client_socket];
    if(client_caller.get_grade() != AUTHENTICATED)
    {
        std::string msg = ":" + this->get_srv_ip() + " 451 :You have not registered\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
    std::string channel_name = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, channel_name.length() + 1);
    if (buffer[0] == ':')
        buffer.erase(0, 1);
    std::vector<std::string> target_names = split_multiple_targets(channel_name);
    for (unsigned int i = 0; i < target_names.size(); i++)
    {
        channel_name = target_names[i];
        for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
        {
            if(it->get_name() == channel_name)
            {
                std::string msg = ":" + client_caller.get_nickname() + " PRIVMSG " + channel_name + " :" + buffer + "\r\n";
                std::cout << "Num users : " << it->get_users().size() << std::endl;
                it->send_message(msg, client_socket);
            }
        }
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
    Client client_caller = clients[client_socket];
    std::string ch = buffer.substr(0,buffer.find(" "));
    buffer.erase(0,ch.length()+1);
    std::string user = buffer.substr(0,buffer.find(" "));
    buffer.erase(0,user.length()+1);
    std::string reason = buffer.substr(0);
    if (ch.empty() && user.empty() && reason.empty())
    {
        std::string msg = "461 ERR_NEEDMOREPARAMS <" + buffer +">:Not enough parameters\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
        return;
    }
     for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
    {
        if(it->get_name() == ch)
        {
            if(it->get_admin().get_nickname() == client_caller.get_nickname())
            {
                for(std::vector<Client>::iterator it2 = it->get_users().begin(); it2 != it->get_users().end(); ++it2)
                {
                    if(it2->get_nickname() == user)
                    {
                        std::string msg = ":" + this->get_srv_ip() + " KICK " + ch + " " + user + " :Kicked by " + client_caller.get_nickname() + "\r\n";
                        send(it2->get_socket(), msg.c_str(), msg.length(), 0);
                        return;
                    }
                }
                std::string msg = ":" + this->get_srv_ip() + " 441 " + client_caller.get_nickname() + " " + user + " :They aren't on that channel\r\n";
                send(client_socket, msg.c_str(), msg.length(), 0);
                return;
            }
            // std::string msg = ":" + this->get_srv_ip() + " 482 " + client_caller.get_nickname() + " :You're not channel operator\r\n";
            std::string msg ="482 ERR_CHANOPRIVSNEEDED <" + client_caller.get_nickname() + ">:You're not channel operator\r\n";
            send(client_socket, msg.c_str(), msg.length(), 0);
            return;
        }
    }
    // std::string msg = ":" + this->get_srv_ip() + " 403 " + client_caller.get_nickname() + " " + ch + " :No such channel\r\n";
    std::string msg ="403 ERR_NOSUCHCHANNEL <" + ch + "> :No such channel\r\n";
    send(client_socket, msg.c_str(), msg.length(), 0);
}

// void Channel::invite_user(std::string user)
// {
//     for(std::vector<Client>::iterator it = this->users.begin(); it != this->users.end(); ++it)
//     {
//         if(it->get_nickname() == user)
//         {
//             return;
//         }
//     }
// }
// void Channel::invite_user(Client user)
// {
//     for(std::vector<Client>::iterator it = this->users.begin(); it != this->users.end(); ++it)
//     {
//         if(it->get_nickname() == user)
//         {
//             return;
//         }
//     }
// }

void Server::invite_cmd(int client_socket, std::string buffer){
    std::string user = buffer.substr(0,buffer.find(" "));
    buffer.erase(0,user.length()+1);
    std::string ch = buffer.substr(0,buffer.find(" "));
    (void)client_socket;
    // for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
    // {
    //     if(it->get_name() == ch)
    //     {
    //         if(it->get_admin().get_nickname() == this->clients[client_socket].get_nickname())
    //         {
    //             it->add_invited_user()
    //             return;
    //         }
    //         else
    //         {
    //             return;
    //         }
    //     }
    // }
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

// void Server::list(int client_socket, std::string buffer) {
//     // buffer.erase(0, buffer.find(" ") + 1);
//     std::string ch = buffer.substr(0, buffer.find(" "));
//     std::cout << "*** " << buffer << std::endl;
//     for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
//         if (it->get_name() == ch) {
//             std::string msg = "LIST " + it->get_name() + " ";
//             std::string msg1 = it->list_cmd(msg);
//             msg1 += "\r\n";
//             send(client_socket, msg1.c_str(), msg1.length(), 0);
//             return;
//         }
//     }
//     send(client_socket, "ERR LIST\r\n", 10, 0);
// }

// void Server::list(int client_socket,std::string buffer)
// {
//     buffer.erase(0,buffer.find(" ")+1);
//     std::string ch = buffer.substr(0,buffer.find(" "));
//     for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
//     {
//         if(it->get_name() == ch)
//         {
//             std::string msg = "LIST " + it->get_name() + " " + it->get_admin().get_nickname() + " " + std::to_string(it->get_name().size()) + "\n";
//             send(client_socket, msg.c_str(), msg.length(), 0);
//             return;
//         }
//     }
//     send(client_socket, "ERR LIST\r\n", 10, 0);
// }

void Server::kill_cmd(int client_socket, std::string buffer)
{
    std::string user = buffer.substr(0,buffer.find(" "));
    buffer.erase(0,user.length()+1);
    std::string reason = buffer.substr(0);
    for(std::map<int,Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
    {
        if(it->second.get_nickname() == user)
        {
            send(it->first, ("KILL " + reason + "\n").c_str(), 16, 0);
            close(it->first);
            clients.erase(it);
            send(client_socket, "KILL OK\n", 8, 0);
            return;
        }
    }
    send(client_socket, "ERR KILL\n", 9, 0);
}

void Server::handle_input(int client_socket)
{
    std::string buffer = this->client_request(client_socket);
    if(buffer.empty())
    {
        return;
    }
    std::cout << "|" + buffer + "|" << std::endl;
    std::string command = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, command.length() + 1);
    if(command == "MODE")
    // MODE # +t +l 100 -o & 
    //MODE # +tl 100
    {   // MODE & +++++---+++tm  +t -m  +t-m
        //+lio 100 heheh
        this->mode_flag(client_socket, buffer);
    }
    // if(command == "LIST")
    // {
    //     this->list(client_socket, buffer);
    // }
    else if (command == "KILL")
    {
        this->kill_cmd(client_socket, buffer);
    }
    else if(command == "PART")
    {
        this->part_cmd(client_socket, buffer);
    }
    else if(command == "PASS") // griffin
    {
        this->pass_cmd(client_socket, buffer);
    }
    else if(command == "INVITE")
    {
        this->invite_cmd(client_socket, buffer);
    }
    else if(command == "NICK") // griffin
    {
        this->nick_cmd(client_socket, buffer);
    }
    else if(command == "TOPIC") // scayho
    {
        this->topic_cmd(client_socket, buffer);
    }
    else if(command == "USER") // griffin
    {
        this->user_cmd(client_socket, buffer);
    }
    else if (command == "KICK")
    {
        this->kick_cmd(client_socket, buffer);
    }
    else if(command == "JOIN") // griffin
    {
        this->join_cmd(client_socket, buffer);
    }
    else if(command == "MSG" || command == "PRIVMSG") // scayho
    {
        this->msg(client_socket, buffer);
    }
    else
    {
        // std::string msg = std::string(ERR_UNKNOWNCOMMAND) + " " + command + " :Unknown command\r\n";
        std::string msg = "421 ERR_UNKNOWNCOMMAND <" + command +"> :Unknown command\r\n";
        send(client_socket, msg.c_str(), msg.length(), 0);
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

std::map<int, Client> &Server::get_clients()
{
    return (this->clients);
}

std::vector<Channel> &Server::get_channels()
{
    return (this->channels);
}

std::string Server::get_srv_ip() const
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];
    std::string ip;

    if (getifaddrs(&ifaddr) == -1)
    {
        std::cerr << "Error: could not get server ip." << std::endl;
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET)
        {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0)
            {
                std::cerr << "Error: could not get server ip." << std::endl;
                exit(EXIT_FAILURE);
            }
            ip = host;
        }
    }
    freeifaddrs(ifaddr);
    return (ip);
}


//TODO modify the messages sent to the users when executing authentification commands;
//TODO find a way to mark a user as authentificated after entering all auth commands;
//TODO remove the testing msg command;
//TODO polish and finish the functions you started (talking to myself here (griffin) );
//TODO this one is for drari, find something to work on, anything i started, ill finish, (dont modify code only add your own stuff, to avoid conflict);
// GGWP GUYS IRC DONE IN A LITTLE BIT;