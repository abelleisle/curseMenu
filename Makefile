LIBS = -lpthread -lncurses
INC = -Iinclude
FLAGS = -g -m64 -std=c++17
WARN = -Wall -Wextra -Werror -pedantic-errors

all:
	@g++ $(FLAGS) $(INC) $(WARN) -o main *.cpp $(LIBS) 
