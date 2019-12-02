#Chase Richards
#CMPSCI 4280 P4
#Filename: makefile

P2: main.cpp parser.cpp scanner.cpp semantics.cpp
	g++ -std=c++0x -Wall -g -o compile main.cpp parser.cpp scanner.cpp semantics.cpp

clean:
	rm compile keyboard.txt
