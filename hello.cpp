
#include <iostream>
#include <set>
#include "board.h"



std::string fen_in1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
std::string fen_in2 = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2";
std::string fen_in3 = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
std::string fen_in4 = "k7/8/8/8/8/8/8/7K b KQkq - 20 31";

int test_fen_conversion(Board board, std::string fen_in) {
	board.fromFen(fen_in);
	std::string fen_out = board.toFen();
	if (fen_in == fen_out) {
		return 0;
	} else {
		std::cout << "Fen fail!\n";
		std::cout << "fen_in: " << fen_in << "\n";
		std::cout << "fen_out: " << fen_out << "\n";
		return 1;
	}
}

int test_fen() {
	int status = 0;
	Board board;
	status += test_fen_conversion(board, fen_in1);
	status += test_fen_conversion(board, fen_in2);
	status += test_fen_conversion(board, fen_in3);
	status += test_fen_conversion(board, fen_in4);
	return status;
}

int test_knight_moves(){
	Board board;
	board.fromFen(fen_in1);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1;
	s1.insert("a3");
	s1.insert("c3");
	s1.insert("f3");
	s1.insert("h3");

	for (tmove move : moves) {
		tsquare dest = std::get<1>(move);
		if (s1.count(board.getSquare(dest))) {
			std::cout << "Knight success!\n";
			continue;
		} else {
			// bad square
			std::cout << "Knight fail!\n";
			return 1;
		}
	}
	return 0;
}

int test_king_pos(){
	Board board;
	board.fromFen(fen_in1);
	tsquare king_pos = board.findKing();
	if ("e1" == board.getSquare(king_pos)){
		return 0;
	} else {
		std::cout << "King fail!\n";
		return 1;
	}
}

int main() {
	int pass = 0;
	std::cout << "Hello World\n";
	//board.printBoard();
	Board board;
	board.fromFen(fen_in1);
	std::vector<tmove> moves = board.getMoves();
	pass += test_fen();
	pass += test_knight_moves();
	pass += test_king_pos();
	return pass;
}
