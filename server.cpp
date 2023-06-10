#include "server.hpp"

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
    {
        std::cerr << "Error: could not create socket." << std::endl;
        exit(1);
    }

    int reuseaddr = 1;
    if (setsockopt(this->srv_socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)) == -1)
    {
        std::cerr << "Error: could not set socket option SO_REUSEADDR." << std::endl;
        exit(1);
    }

    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(this->srv_port);
    srv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->srv_socket, reinterpret_cast<struct sockaddr*>(&srv_addr), sizeof(srv_addr)) == -1)
    {
        std::cerr << "Error: could not bind socket." << std::endl;
        exit(1);
    }

    if (listen(this->srv_socket, SOMAXCONN) == -1)
    {
        std::cerr << "Error: could not listen on socket." << std::endl;
        exit(1);
    }
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
        std::cerr << "Error: could not accept client." << std::endl;
        exit(1);
    }
    std::cout << "Client connected." << std::endl;
    Client client(client_socket, client_addr);
    this->clients.push_back(client);

    pfd.fd = client_socket;
    pfd.events = POLLIN;
    pfd.revents = 0;

    this->pollfds.push_back(pfd);
}

void Server::client_request(int client_socket)
{
    char buffer[1024];
    int bytes_read = recv(client_socket, buffer, 1024, 0);
    if (bytes_read == -1)
    {
        std::cerr << "Error: could not read data from client." << std::endl;
        exit(1);
    }
    else if (bytes_read == 0)
    {
        std::cout << "Client disconnected." << std::endl;
        close(client_socket);
    }
    else
    {
        std::cout << "Client sent: " << buffer;
        // send(client_socket, buffer, bytes_read, 0);
    }
    memset(buffer, 0, 1024);
}

void Server::run()
{
    int poll_flag = 0;
    this->init_server();
    std::cout << "Server is listening on port " << this->srv_port << std::endl;

    while (true)
    {
        poll_flag = poll(this->pollfds.data(), this->pollfds.size(), 0);

        if(poll_flag > 0)
        {
            for (size_t i = 0; i < this->pollfds.size(); ++i)
            {
                if (this->pollfds[i].revents & POLLIN)
                {
                    if (this->pollfds[i].fd == this->srv_socket)  // Server socket has incoming connection
                    {
                        accept_client();
                    }
                    else  // Client socket has incoming data
                    {
                        client_request(this->pollfds[i].fd);
                        // std::cout << "Client " << i << " has data." << std::endl;
                    }
                }
            }
        }
    }
}
