CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

all: main

main: main.o Number.o Vector.o
	$(CXX) $(CXXFLAGS) -o main main.o Number.o Vector.o

main.o: main.cpp Number.h Vector.h
	$(CXX) $(CXXFLAGS) -c -o main.o main.cpp

Number.o: Number.cpp Number.h
	$(CXX) $(CXXFLAGS) -c -o Number.o Number.cpp

Vector.o: Vector.cpp Vector.h Number.h
	$(CXX) $(CXXFLAGS) -c -o Vector.o Vector.cpp

run: main
	./main

clean:
	rm -f *.o main

.PHONY: all run clean
