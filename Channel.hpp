#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "server.hpp"

class Channel{
    private:
        std::string name;
        std::string topic;
        Client      admin;
        std::vector<Client> users;
        std::vector<Client> operators;
    public:
        Channel();
        Channel(std::string name, std::string topic);
        ~Channel();
};


