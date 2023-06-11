#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "Client.hpp"

class Client;
class Server;

class Channel{
    private:
        std::string topic;
        Client      admin;
        std::vector<Client> operators;
    public:
        std::string name;//make private later;
        std::vector<Client> users;
        Channel();
        Channel(std::string name, std::string topic);
        ~Channel();
};


