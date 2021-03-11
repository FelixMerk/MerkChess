
#include <iostream>
#include "board.h"

int test_fen_conversion(Board board, std::string fen_in) {
	board.fromFen(fen_in);
	std::string fen_out = board.toFen();
	if (fen_in == fen_out) {
		std::cout << "Fen succesfull!\n";
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
	std::string fen_in1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	std::string fen_in2 = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2";
	std::string fen_in3 = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
	std::string fen_in4 = "k7/8/8/8/8/8/8/7K b KQkq - 20 31";
	Board board;
	status += test_fen_conversion(board, fen_in1);
	status += test_fen_conversion(board, fen_in2);
	status += test_fen_conversion(board, fen_in3);
	status += test_fen_conversion(board, fen_in4);
	return status;
}

int main() {
	std::cout << "Hello World\n";
	//board.printBoard();
	Board board;
	std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	board.fromFen(fen);
	std::tuple<int, int> king_pos = board.findKing();
	std::cout << board.numToChar(std::get<1>(king_pos)) << 8 - std::get<0>(king_pos) << "\n";
	return test_fen();
}
