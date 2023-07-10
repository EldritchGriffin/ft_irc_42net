#include "Bot.hpp"

Bot::Bot(std::string const &host, std::string const &port)
{
    this->_host = host;
    this->_port = port;
}

Bot::~Bot()
{
}

void Bot::run()
{
    int fd = connect();
    std::cout << "Bot is running" << std::endl;
    std::cout << "Host: " << this->_host << std::endl;
    std::cout << "Port: " << this->_port << std::endl;

    send(fd, this->_port, "NICK bot\r\n");
    send(fd, "USER bot 0 * :realname\r\n");
    send(fd, "JOIN #test\r\n");
    send(fd, "PRIVMSG #test :Hello World\r\n");
    send(fd, "QUIT\r\n");
}


int main(int ac, char **av)
{
    if (ac != 3)
        return (1);
    Bot bot(av[1], av[2]);
    bot.run();
    return (0);
}