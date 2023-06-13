#include "server.hpp"
#include "Channel.hpp"
#include "Client.hpp"



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
    return (casa_rm(std::string(buffer)));
    // return std::string(buffer);
}

void Server::join_cmd(int client_socket, std::string buffer)
{
    if(this->clients[client_socket].get_pass_state() == NOPASS)
    {
        send(client_socket, "ERR PASS\r\n", 10, 0);
        return;
    }
    if(buffer.empty())
    {
        send(client_socket, "ERR JOIN\r\n", 10, 0);
        return;
    }
    std::string channel_name = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, channel_name.length());
    if(buffer.empty())
    {
        send(client_socket, "ERR JOIN\r\n", 10, 0);
        return;
    }
    std::string channel_password = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, channel_password.length());
    if(buffer.empty())
    {
        send(client_socket, "ERR JOIN\r\n", 10, 0);
        return;
    }

    for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
    {
        if(it->get_name() == channel_name)
        {
            if(it->get_password() == channel_password)
            {
                it->add_user(this->clients[client_socket]);
                send(client_socket, "JOIN OK\r\n", 9, 0);
                return;
            }
        }
        else
        {
            send(client_socket, "wrong pass\n" ,11, 0);
            return ;
        }
    }
    Channel channel(channel_name, channel_password);
    channel.set_admin(this->clients[client_socket]);
    this->channels.push_back(channel);
    send(client_socket, "JOIN OKK\r\n", 9, 0);
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


void Server::msg(int client_socket, std::string buffer)
{
    std::string channel_name = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, channel_name.length() + 1);
    buffer.erase(0, buffer.find(":") + 1);
    std::vector<std::string> target_names;
    if (channel_name.find(',', 0) != std::string::npos)
    {
        while (channel_name.find(',', 0) != std::string::npos)
        {
            std::string tmp = channel_name.substr(0, channel_name.find(',', 0));
            channel_name.erase(0, channel_name.find(',', 0) + 1);
            target_names.push_back(tmp);
        }
        std::string tmp = channel_name.substr(0);
        // channel_name.erase(0, channel_name.find(' ', 0));
        target_names.push_back(tmp);
    }
    else
        target_names.push_back(channel_name);
    for (unsigned int i = 0; i < target_names.size(); i++)
    {
        pp_ch(channels);
        channel_name = target_names[i];
        Client client_caller = clients[client_socket];
        for(std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
        {
            if(it->get_name() == channel_name)
            {
                it->send_message(buffer);
            }
        }
        
        pp_pp(clients);
        for(std::map<int, Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
        {
            if(it->second.get_nickname() == channel_name)
            {
                send(it->second.get_socket(), (client_caller.get_nickname() + " :" + buffer + "\n").c_str(), buffer.length() + client_caller.get_nickname().length() + 3, 0);
            }
        }
    }
    return;
}




void Server::handle_input(int client_socket)
{
    std::string buffer = client_request(client_socket);
    if(buffer.empty())
        return;
    std::cout << "command cougth :" << buffer  << std::endl;
    // std::cout << "last character : " << (int)buffer[buffer.length() - 1] << std::endl;
    // std::cout << "last of last character : " << (int)buffer[buffer.length() - 2] << std::endl;
    std::string cmd = buffer.substr(0, buffer.find(" "));
    buffer.erase(0, cmd.length() + 1);    
    if(cmd == "JOIN")
        join_cmd(client_socket, buffer);
    if(cmd == "PASS")
        pass_cmd(client_socket, buffer);
    if(cmd == "NICK")
        nick_cmd(client_socket, buffer);
    if(cmd == "USER")
        user_cmd(client_socket, buffer);
    // std::cout << buffer << " | " << cmd << std::endl;
    // std::cout << this->clients[client_socket].get_pass_state() << std::endl;
    if(cmd == "MSG" || cmd == "PRIVMSG") // 1st send by nc || 2nd limechat add PRV to MSG
    {
        std::cout << "command MSG detected " << std::endl;
        
        msg(client_socket, buffer);
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

//TODO modify the messages sent to the users when executing authentification commands;
//TODO find a way to mark a user as authentificated after entering all auth commands;
//TODO remove the testing msg command;
//TODO polish and finish the functions you started (talking to myself here (griffin) );
//TODO this one is for drari, find something to work on, anything i started, ill finish, (dont modify code only add your own stuff, to avoid conflict);
// GGWP GUYS IRC DONE IN A LITTLE BIT;