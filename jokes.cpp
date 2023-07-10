#include "jokes.hpp"

joke::joke()
{
    jokes_list.push_back("What’s the best thing about Switzerland?");
    jokes_answers.push_back("I don’t know, but the flag is a big plus.");
    jokes_list.push_back("");
    jokes_answers.push_back("");
    jokes_list.push_back("Want to hear a construction joke?");
    jokes_answers.push_back("Oh never mind, I’m still working on that one.");
    jokes_list.push_back("Why did the nurse need a red pen at work?");
    jokes_answers.push_back("In case she needed to draw blood.");
    jokes_list.push_back("Why aren’t koalas actual bears?");
    jokes_answers.push_back("They don’t meet the koalafications.");
    jokes_list.push_back("How many times can you subtract 10 from 100?");
    jokes_answers.push_back("Once. The next time you would be subtracting 10 from 90.");
    jokes_list.push_back("What breed of dog can jump higher than buildings?");
    jokes_answers.push_back("Any dog, because buildings can’t jump.");
    jokes_list.push_back("Where does Batman go to the bathroom?");
    jokes_answers.push_back("The batroom.");
    jokes_list.push_back("Aji Gol Liya");
    jokes_answers.push_back("La face Wela Lkoliya");
    jokes_list.push_back("What is an astronaut’s favorite part on a computer?");
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
    jokes_list.push_back("A man tells his doctor, “Doc, help me. I’m addicted to Twitter!”");
    jokes_answers.push_back("The doctor replies, “Sorry, I don’t follow you …”");
    jokes_list.push_back("Hear about the new restaurant called Karma?");
    jokes_answers.push_back("There’s no menu: You get what you deserve.");
}


int main(int ac, char **av)
{
    joke joker();

}