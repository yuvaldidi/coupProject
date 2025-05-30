# yuvalgaddidi@gmail.com

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
LDFLAGS = 
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

SRCS = Game.cpp Player.cpp Bank.cpp
OBJS = $(SRCS:.cpp=.o)

all: Main

Main: main.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

test: test.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o test test.cpp $(OBJS)

valgrind: test
	valgrind ./test

gui: main_gui.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o gui_app main_gui.cpp $(OBJS) $(LIBS)

clean:
	rm -f *.o Main test gui_app
