SRC 	:= main.cpp server.cpp Client.cpp Channel.cpp Tools.cpp authenticateUser.cpp join.cpp topic.cpp quit.cpp  kick.cpp mode.cpp list.cpp part.cpp
CC 		:= c++
CFLAG	:= -Wall -Werror -Wextra -std=c++98 
NAME 	:= ircserv
HEADERS := server.hpp Client.hpp Channel.hpp Tools.hpp
OBJ		:= $(SRC:%.cpp=%.o)

all: $(NAME)

$(NAME) : $(OBJ)
	c++ $(CFLAG) $(SRC) -o $(NAME)

%.o: %.cpp $(HEADERS)
	c++ $(CFLAG) -c $< -o $@

clean:
	rm -rf $(OBJ)
	
fclean : clean
	rm -rf $(NAME)

j : all clean
	clear && ./ircserv 6667 ok

re : fclean all