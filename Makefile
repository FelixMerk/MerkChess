
CXX=g++


all: cli test_moves test_eval

cli: cli.o board.o util.o hash.o
	g++ -o cli cli.o board.o util.o hash.o -O3 --std=c++11

cli.o: cli.cpp
	g++ -c cli.cpp --std=c++11

board.o: board/board.cpp board/board.h
	g++ -c board/board.cpp --std=c++11

test_moves: test_moves.o board.o util.o hash.o
	g++ -o test_moves test_moves.o board.o util.o hash.o --std=c++11

test_moves.o: board/test_moves.cpp
	g++ -c board/test_moves.cpp --std=c++11

test_eval: test_eval.o board.o util.o hash.o
	g++ -o test_eval test_eval.o board.o util.o hash.o --std=c++11 -O3
	
test_eval.o: board/test_eval.cpp
	g++ -c board/test_eval.cpp --std=c++11

util.o: board/util.cpp board/util.h
	g++ -c board/util.cpp --std=c++11

hash.o: hash/hash.cpp hash/hash.h
	g++ -c hash/hash.cpp --std=c++11

test_util: test_util.o util.o board.o hash.o
	g++ -o test_util test_util.o util.o board.o hash.o --std=c++11

test_util.o: board/test_util.cpp
	g++ -c board/test_util.cpp --std=c++11

clean:
	rm -f cli cli.o board.o
	rm -f test_eval test_eval.o
	rm -f test_moves test_moves.o
	rm -f util.o
	rm -f hash.o
