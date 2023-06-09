#include "server.hpp"

Server::Server(int port, std::string password)
{
    this->srv_port = port;
    this->srv_password = password;
    }

Server::~Server(){}

void Server::init_server()
{
    this->srv_socket = socket(AF_INET, SOCK_STREAM, 0); // 1st : domain  ; 2nd type 
    //FIXME: learn why setting the socket to non-blocking causes the server to not work properly (i.e. not accept clients);
    //fcntl(this->srv_socket, F_SETFL, O_NONBLOCK);
    int s = 1;
    if (setsockopt(this->srv_socket, SOL_SOCKET, SO_REUSEADDR, &s, sizeof(int)) < 0) // when the server shuts down the port used remain reserved for aproximate 2 minutes , the bind fails whithin this time {line 30}
        exit(0);
    if(this->srv_socket == -1)
    {
        std::cerr << "Error: could not create socket." << std::endl;
        exit(1);
    }
    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET; // define the protocole used , in this case TCP/IP && UDP {the first part of bits in this struct}
    srv_addr.sin_port = htons(this->srv_port); // define which port the socket must listen to  {the second part of bits in this struct}
    srv_addr.sin_addr.s_addr = INADDR_ANY;  // {the 3rd part of bits in this struct contain 32 unsigned int which hold the IP number}
    if(bind(this->srv_socket, (struct sockaddr*)&srv_addr, sizeof(srv_addr)) == -1) // bind the socket created to the structor sockadd_in
    {
        std::cerr << "Error: could not bind socket." << std::endl;
        exit(1);
    }
    // SOMAXCONN 128
    if(listen(this->srv_socket, 10) == -1) // in success the socket is ready to recieve clients
    {
        std::cerr << "Error: could not listen on socket." << std::endl;
        exit(1);
    }
}

void Server::accept_client()
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int client_socket = accept(this->srv_socket, (struct sockaddr*)&client_addr, &client_addr_size); // accept the client by creating a socket that can read from the client
    if(client_socket == -1)
    {
        std::cerr << "Error: could not accept client." << std::endl;
        exit(1);
    }
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN); // convert the the number in the 2nd parameter to a txt buffered in the 3rd parameter with the length of the 4rd parameter
    std::cout << "Client connected: " << client_ip << std::endl;
    size_t message_buffer = 2024;
    char client_name[message_buffer];
    // int read_size = recv(client_socket, client_name, message_buffer, 0); // it fill the 2nd parameter with the length of the 3rd parameter from the socket specified in the 1st parameter ; the last parameter is the flags seems like open()
    // struct pollfd fds[2];
    // fds[0].fd = client_socket;
    // fds[0].events = POLLIN;
    // fds[1].fd = client_socket;
    // fds[1].events = POLLIN;
    int read_size = read(client_socket, client_name, message_buffer);
    if(read_size == -1)
    {
        std::cerr << "Error: could not read from socket." << std::endl;
        exit(1);
    }
    client_name[read_size] = '\0';
    std::cout << "Client name: " << client_name << std::endl;
    send(client_socket, "well recieved", strlen("well recieved"), 0);
    Client client(client_socket, client_name, client_ip, ntohs(client_addr.sin_port));
    this->clients.push_back(client);
}

// void Server::accept_client()
// {
//     struct sockaddr_in client_addr;
//     socklen_t client_addr_size = sizeof(client_addr);
//     int client_socket = accept(this->srv_socket, (struct sockaddr*)&client_addr, &client_addr_size); // accept the client by creating a socket that can read from the client
//     if(client_socket == -1)
//     {
//         std::cerr << "Error: could not accept client." << std::endl;
//         exit(1);
//     }
//     char client_ip[INET_ADDRSTRLEN];
//     inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN); // convert the the number in the 2nd parameter to a txt buffered in the 3rd parameter with the length of the 4rd parameter
//     std::cout << "Client connected: " << client_ip << std::endl;
//     size_t message_buffer = 2024;
//     char client_name[message_buffer];
//     // int read_size = recv(client_socket, client_name, message_buffer, 0); // it fill the 2nd parameter with the length of the 3rd parameter from the socket specified in the 1st parameter ; the last parameter is the flags seems like open()
//     int read_size = read(client_socket, client_name, message_buffer);
//     if(read_size == -1)
//     {
//         std::cerr << "Error: could not read from socket." << std::endl;
//         exit(1);
//     }
//     client_name[read_size] = '\0';
//     std::cout << "Client name: " << client_name << std::endl;
//     send(srv_socket, "hello", strlen("hello"), 0); // sent the message into the port using the socket
//     Client client(client_socket, client_name, client_ip, ntohs(client_addr.sin_port));
//     this->clients.push_back(client);
// }

void Server::run()
{
    this->init_server();
    std::cout << "Server is listening on port " << this->srv_port << std::endl;
    accept_client();
}