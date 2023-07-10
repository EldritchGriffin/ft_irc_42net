#include "Bot.hpp"

Bot::Bot(std::string const &host, std::string const &port, std::string const &pass)
{
    this->_host = host;
    this->_port = port;
    this->_pass = pass;
}

Bot::~Bot()
{
}

void auth_to_server(int sock, std::string const &pass)
{
    std::string msg = "PASS " + pass + "\n";
    send(sock, msg.c_str(), msg.size(), 0);
    usleep(100000);
    msg = "NICK ROBOT\n";
    send(sock, msg.c_str(), msg.size(), 0);
    usleep(100000);
    msg = "USER BOT 0 * ROBOT\n";
    send(sock, msg.c_str(), msg.size(), 0);
    usleep(100000);
    std::cout << "Connected to the server" << std::endl;
}

int connect_to_server(std::string const &host, std::string const &port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        std::cerr << "Failed to create socket. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(atoi(port.c_str()));
    if (inet_pton(AF_INET, host.c_str(), &sockaddr.sin_addr) <= 0)
    {
        std::cerr << "Failed to setup server address. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    if (connect(sock, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
    {
        std::cerr << "Failed to connect to the server. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    return (sock);
}

void Bot::run()
{
    int sock = connect_to_server(this->_host, this->_port);
    auth_to_server(sock, this->_pass);
    while(1)
    {
        char buffer[1024];
        int bytes_read = recv(sock, &buffer, 1024, 0);
        buffer[bytes_read] = '\0';
        if (bytes_read <= 0)
        {
            std::cerr << "Server disconnected" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::cout << buffer << std::endl;
    }
}


int main(int ac, char **av)
{
    if (ac != 4)
        return (1);
    Bot bot(av[1], av[2], av[3]);
    bot.run();
    return (0);
}