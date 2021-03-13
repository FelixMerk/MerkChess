
#include <iostream>
#include <set>
#include "board.h"



std::string fen_in1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
std::string fen_in2 = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2";
std::string fen_in3 = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
std::string fen_in4 = "k7/8/8/8/8/8/8/7K b Qkq - 20 31";

// White bishops and knight with restrictions
std::string fen_in5 = "8/5p2/7p/p3P3/2B2B2/1N6/3P3P/8 w - - 0 1";
std::string fen_in6 = "8/8/3p4/8/8/b2R4/7R/3K4 w - - 0 1";

// Pawn things
std::string fen_in7 = "8/3P4/2p5/1pP1pp2/p4P1p/2p1p1P1/PP2P3/8 w - b6 0 1";

// King things
std::string fen_in8 = "8/8/8/8/5k2/3n4/2PK4/8 w - - 0 1";

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
	status += test_fen_conversion(board, fen_in7);
	return status;
}

bool squares_in_moves(
	std::set<std::string> s1,
	std::vector<tmove> moves,
	Board board
) {
	for (std::string key : s1) {
		bool pass = false;
		for (tmove move : moves) {
			tsquare dest = std::get<1>(move);
			if (board.getNameOfSquare(dest) == key) {
				pass = true;
				break;
			}
		}
		if (!pass) {
			std::cout << key << " is missing?\n";
			return false;
		}
	}
	return true;
}

bool piece_moves_in_squares(
	std::vector<tmove> moves, tpiece piece, Board board, std::set<std::string> s1
) {
	for (tmove move : moves) {
		tsquare source = std::get<0>(move);

		tpiece mpiece = board.board[std::get<0>(source)][std::get<1>(source)];
		if (mpiece == piece) {
			tsquare dest = std::get<1>(move);
			if (s1.count(board.getNameOfSquare(dest))) {
				continue;
			} else {
				// bad square
				std::cout << board.getPiece(piece) << " fail!\n";
				std::cout << board.getNameOfSquare(dest) << " is included?\n";
				return false;
			}
		}
	}
	return true;

}

bool test_knight_moves(){
	bool pass = true;

	Board board;
	board.fromFen(fen_in1);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1 = {"a3", "c3", "f3", "h3"};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::knight,
		board,
		s1
	);

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	return pass;
}

bool test_bishop_moves(){
	bool pass = true;

	Board board;
	board.fromFen(fen_in5);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1 = {"a6", "b5", "d5", "d3", "e6", "e3", "e2", "f7",
		"f1", "g5", "g3", "h6"};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::bishop,
		board,
		s1
	);

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	return pass;
}

bool test_rook_moves(){
	bool pass = true;

	Board board;
	board.fromFen(fen_in6);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1;
	s1 = {"a3", "a2", "b3", "b2", "c3", "c2", "d6", "d5", "d4", "d2", 
		"e3", "e2", "f3", "f2", "g3", "g2",
		"h8", "h7", "h6", "h5", "h4", "h3", "h1"
	};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::rook,
		board,
		s1
	);

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	return pass;
}


bool test_pawn_moves(){
	bool pass = true;

	Board board;
	board.fromFen(fen_in7);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1;
	s1 = {"a3", "b6", "b4", "b3", "c3", "d8", "e5", "g4", "h4"};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::pawn,
		board,
		s1
	);

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	return pass;
}

bool test_king_pos(){
	Board board;
	board.fromFen(fen_in1);
	tsquare king_pos = board.findKing();
	if ("e1" == board.getNameOfSquare(king_pos)){
		return true;
	} else {
		std::cout << "King fail!\n";
		return false;
	}
}

bool test_king_moves(){
	bool pass = true;
	Board board;
	board.fromFen(fen_in8);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1;
	s1 = {"c3", "d3", "d1", "e2", 
		"c1", "e3", "e1"
	};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::king,
		board,
		s1
	);

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	return pass;
}


bool test_castle_moves(){
	bool pass = true;

	std::string fen_c1 = "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1";
	std::string fen_c2 = "r3k2r/8/8/8/8/8/8/R3K2R w Qkq - 0 1";
	std::string fen_c3 = "r3k2r/8/8/8/8/8/8/R3K2R w kq - 0 1";

	Board board;

	// 1
	board.fromFen(fen_c1);
	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1 = {"c1", "g1"};

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	// 2
	board.fromFen(fen_c2);
	moves = board.getMoves();

	std::set<std::string> s2 = {"c1",
		"d2", "d1", "e2", "f2", "f1"
	};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::king,
		board,
		s2
	);

	pass = pass and squares_in_moves(
		s2,
		moves,
		board
	);

	return pass;
}

int main() {
	int pass = 0;
	std::cout << "Hello World\n";
	//board.printBoard();
	Board board;
	board.fromFen(fen_in1);
	//std::vector<tmove> moves = board.getMoves();

	pass += test_fen();
	pass += test_knight_moves();
	pass += test_bishop_moves();
	pass += test_king_pos();
	pass += test_rook_moves();
	pass += test_pawn_moves();
	pass += test_king_moves();
	pass += test_castle_moves();
	return pass;
}
