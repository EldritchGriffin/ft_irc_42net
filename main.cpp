#include "Server.hpp"

int main(int ac, char** av)
{
    if(ac < 2)
    {
        std::cout << "Usage: " << av[0] << " <port>" << std::endl;
        return 1;
    }
    //TODO: create a server here;

    Server server(atoi(av[1]));

    server.run();
}