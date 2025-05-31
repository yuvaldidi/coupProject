# yuvalgaddidi@gmail.com
# Makefile for the Coup project
# Builds and runs demo, tests, GUI, valgrind, and cleaning

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
LDFLAGS_SFML = -lsfml-graphics -lsfml-window -lsfml-system

SRC = Bank.cpp Game.cpp Player.cpp
OBJ = $(SRC:.cpp=.o)

HEADERS = Bank.hpp Game.hpp Player.hpp Role.hpp Deck.hpp Judge.hpp Baron.hpp General.hpp Governor.hpp Merchant.hpp Spy.hpp

.PHONY: all Main test gui valgrind clean

all: Main

Main: main.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) -o Main main.cpp $(OBJ)
	./Main

test: test.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) -o test test.cpp $(OBJ)
	./test

gui: main_gui.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) -o gui_app main_gui.cpp $(OBJ) $(LDFLAGS_SFML)
	./gui_app

valgrind: Main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./Main

clean:
	rm -f *.o Main test gui_app
