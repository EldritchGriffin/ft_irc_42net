#include "Server.hpp"

Server::Server(int port, std::string password)
{
    this->srv_port = port;
    this->srv_password = password;
    }

Server::~Server(){}

void Server::init_server()
{
    this->srv_socket = socket(AF_INET, SOCK_STREAM, 0);
    //FIXME: learn why setting the socket to non-blocking causes the server to not work properly (i.e. not accept clients);
    //fcntl(this->srv_socket, F_SETFL, O_NONBLOCK);
    if(this->srv_socket == -1)
    {
        std::cerr << "Error: could not create socket." << std::endl;
        exit(1);
    }
    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(this->srv_port);
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(this->srv_socket, (struct sockaddr*)&srv_addr, sizeof(srv_addr)) == -1)
    {
        std::cerr << "Error: could not bind socket." << std::endl;
        exit(1);
    }
    if(listen(this->srv_socket, 10) == -1)
    {
        std::cerr << "Error: could not listen on socket." << std::endl;
        exit(1);
    }
}

void Server::accept_client()
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int client_socket = accept(this->srv_socket, (struct sockaddr*)&client_addr, &client_addr_size);
    if(client_socket == -1)
    {
        std::cerr << "Error: could not accept client." << std::endl;
        exit(1);
    }
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::cout << "Client connected: " << client_ip << std::endl;
    char client_name[1024];
    int read_size = recv(client_socket, client_name, 1024, 0);
    if(read_size == -1)
    {
        std::cerr << "Error: could not read from socket." << std::endl;
        exit(1);
    }
    client_name[read_size] = '\0';
    std::cout << "Client name: " << client_name << std::endl;
    Client client(client_socket, client_name, client_ip, ntohs(client_addr.sin_port));
    this->clients.push_back(client);
}


void Server::run()
{
    this->init_server();
    std::cout << "Server is listening on port " << this->srv_port << std::endl;
    accept_client();
}