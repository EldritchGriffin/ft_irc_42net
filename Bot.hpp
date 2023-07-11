#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <exception>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <netdb.h>

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <exception>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/time.h>

class joke
{
    std::vector<std::string> jokes_list;
    std::vector<std::string> jokes_answers;
    public:
        joke();
        ~joke(){}
        void    get_a_joke(int sock, std::string b);
};

class zodiac
{
    public:
        zodiac();
        ~zodiac(){}
        void    get_a_zodiac(int sock, int month, int day, std::string name);
};

class Bot
{
    private:
        std::string _host;
        std::string _port;
        std::string _pass;
    public:
        Bot(std::string const &host, std::string const &port, std::string const &pass);
        ~Bot();

    void run();
};
