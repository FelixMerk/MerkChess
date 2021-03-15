
CXX=g++


all: cli test_moves test_eval

cli: cli.o board.o
	g++ -o cli cli.o board.o -O3 --std=c++11

cli.o: cli.cpp
	g++ -c cli.cpp --std=c++11

board.o: board.cpp board.h
	g++ -c board.cpp --std=c++11

test_moves: test_moves.o board.o
	g++ -o test_moves test_moves.o board.o --std=c++11

test_moves.o: test_moves.cpp
	g++ -c test_moves.cpp --std=c++11

test_eval: test_eval.o board.o
	g++ -o test_eval test_eval.o board.o -O3 --std=c++11
	
test_eval.o: test_eval.cpp
	g++ -c test_eval.cpp --std=c++11

clean:
	rm -f cli cli.o board.o
	rm -f test_eval test_eval.o
	rm -f test_moves test_moves.o
