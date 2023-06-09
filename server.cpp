#include "Server.hpp"

Server::Server(int port)
{
    this->srv_port = port;
}

Server::~Server()
{
}

void Server::run()
{
    std::cout << "Server is running on port " << this->srv_port << std::endl;

    //TODO: here is where we are going to create the socket and start listening for connections;
    this->srv_socket = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(this->srv_socket, F_SETFL, O_NONBLOCK);

    sockaddr_in sckt;
    sockaddr_in client;
    socklen_t   client_len;
    sckt.sin_family = AF_INET;
    sckt.sin_port = htons(this->srv_port);
    sckt.sin_addr.s_addr = inet_addr("0.0.0.0");


    bind(this->srv_socket, (sockaddr *)&sckt, sizeof(sockaddr_in));

    listen(this->srv_socket, 10);
    accept(this->srv_socket, (sockaddr *)&client, &client_len);
    
}