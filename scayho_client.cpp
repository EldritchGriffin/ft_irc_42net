#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Client.hpp"
#include <exception>
#include <unistd.h>

int main(int ac, char **av)
{
    int srv_socket;
    int srv_port = atoi(av[1]);
    std::string client_buffer("test");
    srv_socket = socket(AF_INET, SOCK_STREAM, 0);
     struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(srv_port);
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, "127.0.0.1", &srv_addr.sin_addr); // convert the 2nd paramter from txt into a binary form compatible with the 1st parameter
    connect(srv_socket, (struct sockaddr*)&srv_addr,sizeof(srv_addr)); // connect the socket to the stream
    while (client_buffer != "EXIT")
    {
        std::getline(std::cin, client_buffer);
        send(srv_socket, client_buffer.c_str(), client_buffer.length(), 0); // sent the message into the port using the socket
        size_t message_buffer = 2024;
        char client_name[message_buffer];
        int soso = recv(srv_socket, client_name, message_buffer, 0);
        client_name[soso] = '\0';
        std::cout << "server said : " << client_name << " ;" << std::endl;
    }
}