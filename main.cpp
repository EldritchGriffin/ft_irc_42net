#include "Server.hpp"

int main(int ac, char** av)
{
    if(ac < 3)
    {
        std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
        return 1;
    }
    //TODO: create a server here;

    Server server(atoi(av[1]), av[2]);

    server.run();
}