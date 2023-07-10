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

void Bot::run()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        std::cerr << "Failed to create socket. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(atoi(this->_port.c_str()));  // Server port
    if (inet_pton(AF_INET, this->_host.c_str(), &sockaddr.sin_addr) <= 0)  // Server ip
    {
        std::cerr << "Failed to setup server address. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    if (connect(sock, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
    {
        std::cerr << "Failed to connect to the server. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        char buffer[1024];
        int bytes_read = recv(sock, buffer, 1024, 0);
        std::cout << buffer << std::endl;
    }
}


int main(int ac, char **av)
{
    if (ac != 3)
        return (1);
    Bot bot(av[1], av[2], av[3]);
    bot.run();
    return (0);
}