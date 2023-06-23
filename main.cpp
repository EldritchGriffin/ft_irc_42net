#include "server.hpp"

int main(int ac, char** av)
{
    if(ac < 3)
    {
        std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
        return 1;
    }

    Server server(atoi(av[1]), av[2]);

    while(true)
    {
        try
        {
            server.run();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        sleep(10);
    }
}