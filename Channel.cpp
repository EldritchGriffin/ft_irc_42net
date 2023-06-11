#include "Channel.hpp"

Channel::Channel(std::string name, std::string topic)
{
    this->name = name;
    this->topic = topic;
}

Channel::~Channel()
{
}
