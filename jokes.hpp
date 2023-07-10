#ifndef JOKES_HPP
#define JOKES_HPP

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

class joke
{
    std::vector<std::string> jokes_list;
    std::vector<std::string> jokes_answers;
    joke();
    ~joke();
};

#endif