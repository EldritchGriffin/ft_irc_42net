#include "Bot.hpp"

long long	wakt(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

joke::joke()
{
    jokes_list.push_back("What's the best thing about Switzerland?");
    jokes_answers.push_back("I don't know, but the flag is a big plus.");
    jokes_list.push_back("Two artists had an art contest.");
    jokes_answers.push_back("It ended in a draw!");
    jokes_list.push_back("Want to hear a construction joke?");
    jokes_answers.push_back("Oh never mind, I'm still working on that one.");
    jokes_list.push_back("Why did the nurse need a red pen at work?");
    jokes_answers.push_back("In case she needed to draw blood.");
    jokes_list.push_back("Why aren't koalas actual bears?");
    jokes_answers.push_back("They don't meet the koalafications.");
    jokes_list.push_back("How many times can you subtract 10 from 100?");
    jokes_answers.push_back("Once. The next time you would be subtracting 10 from 90.");
    jokes_list.push_back("What breed of dog can jump higher than buildings?");
    jokes_answers.push_back("Any dog, because buildings can't jump.");
    jokes_list.push_back("Where does Batman go to the bathroom?");
    jokes_answers.push_back("The batroom.");
    jokes_list.push_back("Aji Gol Liya");
    jokes_answers.push_back("La face Wela Lkoliya");
    jokes_list.push_back("What is an astronaut's favorite part on a computer?");
    jokes_answers.push_back("The space bar.");
    jokes_list.push_back("What did the shark say when he ate the clownfish?");
    jokes_answers.push_back("This tastes a little funny.");
    jokes_list.push_back("What do you call a magic dog?");
    jokes_answers.push_back("A labracadabrador.");
    jokes_list.push_back("What did the 0 say to the 8?");
    jokes_answers.push_back("Nice belt!");
    jokes_list.push_back("What do you call a fake noodle?");
    jokes_answers.push_back("An impasta.");
    jokes_list.push_back("What did the left eye say to the right eye?");
    jokes_answers.push_back("Between you and me, something smells.");
    jokes_list.push_back("A man tells his doctor, “Doc, help me. I'm addicted to Twitter!”");
    jokes_answers.push_back("The doctor replies, “Sorry, I don't follow you …”");
    jokes_list.push_back("Hear about the new restaurant called Karma?");
    jokes_answers.push_back("There's no menu: You get what you deserve.");
}

void    joke::get_a_joke(int sock, std::string b)
{
    unsigned long long ok = wakt();
    int joke_number = ok % jokes_list.size();

    std::string msg = "PRIVMSG " + b.substr(0, b.find(' ')) + " :" + jokes_list[joke_number] + "\r\n";
    send(sock, msg.c_str(), msg.size(), 0);
    usleep(100);
    msg = "PRIVMSG " + b.substr(0, b.find(' ')) + " :" + jokes_answers[joke_number] + "\r\n";
    send(sock, msg.c_str(), msg.size(), 0);
}

zodiac::zodiac()
{
}

void    zodiac::get_a_zodiac(int sock, int month, int day, std::string name)
{
    std::string taha;
    if (month > 12 || month < 1 || day < 1 || day > 31)
        taha = "wrong date";
    else if ((month == 1 && day >19) || (month == 2 && day < 19))

        taha = "Aquarius" ;
    else if ((month == 2 && day  > 18) || (month == 3 && day < 21))

        taha = "Pisces" ;
    else if ((month == 3 && day  > 21) || (month == 4 && day < 20))

        taha = "Aries" ;
    else if ((month == 4 && day  > 19) || (month == 5 && day < 21))

        taha = "Taurus" ;
    else if ((month == 5 && day  > 20) || (month == 6 && day < 21))

        taha = "Gemini" ;
    else if ((month == 6 && day  > 20) || (month == 7 && day < 23))

        taha = "Cancer" ;
    else if ((month == 7 && day  > 22) || (month == 8 && day < 23))

        taha = "Leo" ;
    else if ((month == 8 && day  > 22) || (month == 9 && day < 23))

        taha = "Virgo" ;
    else if ((month == 9 && day  > 22) || (month == 10 && day < 23))

        taha = "Libra" ;
    else if ((month == 10 && day  > 22) || (month == 11 && day < 22))

        taha = "Scorpio" ;
    else if ((month == 11 && day  > 21) || (month == 12 && day < 21))

        taha = "Sagittarius" ;
    else
        taha = "Capricorn" ;
    std::cout << "|" + name + "|" << std::endl;
    std::string msg = "PRIVMSG " + name + " :" + taha + "\r\n";
    send(sock, msg.c_str(), msg.size(), 0);
}


Bot::Bot(std::string const &host, std::string const &port, std::string const &pass)
{
    this->_host = host;
    this->_port = port;
    this->_pass = pass;
}

Bot::~Bot()
{
}

void auth_to_server(int sock, std::string const &pass)
{
    std::string msg = "PASS " + pass + "\n";
    send(sock, msg.c_str(), msg.size(), 0);
    usleep(100000);
    msg = "NICK ROBOT\n";
    send(sock, msg.c_str(), msg.size(), 0);
    usleep(100000);
    msg = "USER BOT 0 * ROBOT\n";
    send(sock, msg.c_str(), msg.size(), 0);
    usleep(100000);
    std::cout << "Connected to the server" << std::endl;
}

int connect_to_server(std::string const &host, std::string const &port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        std::cerr << "Failed to create socket. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(atoi(port.c_str()));
    if (inet_pton(AF_INET, host.c_str(), &sockaddr.sin_addr) <= 0)
    {
        std::cerr << "Failed to setup server address. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    if (connect(sock, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
    {
        std::cerr << "Failed to connect to the server. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    return (sock);
}

int get_month(std::string &b)
{
    b.erase(0, b.find('-') + 1);
    int k = atoi(b.c_str());
    b.erase(0, b.find('-') + 1);
    return (k);
}
int get_day(std::string &b)
{
    int k = atoi(b.c_str());
    return (k);
}

void Bot::run()
{
    int sock = connect_to_server(this->_host, this->_port);
    auth_to_server(sock, this->_pass);
    while(1)
    {
        char buffer[1024];
        std::cout << "waiting to receive" << std::endl;
        int bytes_read = recv(sock, &buffer, 1024, 0);
        std::cout << "received" << std::endl;
        buffer[bytes_read] = '\0';
        if (bytes_read <= 0)
        {
            std::cerr << "Server disconnected" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::cout << buffer << std::endl;
        std::string b(buffer);
        if (b[0] == 'J' && b[1] == ' ')
        {
            joke f;
            b.erase(0, 2);
            f.get_a_joke(sock, b);
        }
        if (b[0] == 'Z' && b[1] == ' ')
        {
            b.erase(b.begin());
            b.erase(b.begin());
            zodiac f;
            std::string name = b.substr(0, b.find(' '));
            b.erase(0, b.find(' ') + 1);
            f.get_a_zodiac(sock, get_month(b), get_day(b), name);
        }
    }
}


int main(int ac, char **av)
{
    if (ac != 4)
        return (1);
    Bot bot(av[1], av[2], av[3]);
    bot.run();
    return (0);
}