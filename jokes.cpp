#include "jokes.hpp"
# include <sys/time.h>

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

void    joke::get_a_joke(int sock)
{
    unsigned long long ok = wakt();
    int joke_number = ok % jokes_list.size();

    // send(sock, jokes_list[joke_number].c_str(), jokes_list[joke_number].size(), 0);
    // send(sock, jokes_answers[joke_number].c_str(), jokes_answers[joke_number].size(), 0);
    std::cout << jokes_list[joke_number] << std::endl;
    std::cout << jokes_answers[joke_number] << std::endl;
}

zodiac::zodiac()
{
    zodiac_list.push_back("Aries");
    zodiac_list.push_back("Taurus");
    zodiac_list.push_back("Gemini");
    zodiac_list.push_back("Cancer");
    zodiac_list.push_back("Leo");
    zodiac_list.push_back("Virgo");
    zodiac_list.push_back("Libra");
    zodiac_list.push_back("Scorpio");
    zodiac_list.push_back("Sagittarius");
    zodiac_list.push_back("Capricorn");
    zodiac_list.push_back("Aquarius ");
    zodiac_list.push_back("Pisces");
}

void    zodiac::get_a_zodiac(int sock, int month, int day)
{
    if (month > 12 || month < 1 || day < 1 || day > 31)
        std::string err("wrong date");
    int number = (month * 31) + day;
    number %= 31;
    // send(sock, jokes_list[joke_number].c_str(), jokes_list[joke_number].size(), 0);
    // send(sock, jokes_answers[joke_number].c_str(), jokes_answers[joke_number].size(), 0);
    std::cout << zodiac_list[number] << std::endl;
}

int main(int ac, char **av)
{
    joke joker;
    joker.get_a_joke(3);
    zodiac zozo;
    zozo.get_a_zodiac(4, 8, 10);
    std::cout << ((8 * 31) + 10) % 31 << std::endl;
}