#include "Channel.hpp"
#include "Client.hpp"


Channel::Channel(std::string name, std::string topic)
{
    std::cout << "im here nigga" << std::endl;
    this->name = name;
    this->topic = topic;
}

Channel::~Channel()
{
    std::cout << "i did this shit" << std::endl;
}
