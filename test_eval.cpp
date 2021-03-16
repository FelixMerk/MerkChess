
#include <iostream>
#include <set>
#include "board.h"

std::string fen_in1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
std::string fen_in2 = "4k2r/8/8/8/8/8/8/2B1K3 w - - 0 1";

void printTmove(
	tmove move,
	Board board
) {
	tsquare source = std::get<0>(move);
	int i = std::get<0>(source);
	int j = std::get<1>(source);
	tpiece piece = board.board[i][j];

	tsquare dest = std::get<1>(move);

	tpiece promo = std::get<2>(move);
	
	std::cout << board.getPiece(piece) << " moves: ";	
	std::cout << board.getNameOfSquare (source) << "->";
	std::cout << board.getNameOfSquare (dest) << "\n";
}

tmove makeTmove(
	std::string from,
	std::string to
) {
	Board board;
	board.fromFen(fen_in1);
	tmove move = tmove(
		tsquare(board.getSquareOfName(from)), 
		tsquare(board.getSquareOfName(to)), 
		0
	);
	return move;
}


bool do_eval(
	std::string fen,
	int expected_eval
) {
	Board board;
	board.fromFen(fen);

	int val = board.evaluate();

	if (expected_eval != val) {
		std::cout << "Evaluation failure!\n";
		std::cout << "Expected value: " << expected_eval << "\n";
		std::cout << "Actual value: " << val << "\n";
		std::cout << fen << "\n";
		return false;
	}
	return true;
}


bool test_positions() {
	bool pass = true;
	pass = pass and do_eval(fen_in1, 0);
	pass = pass and do_eval(fen_in2, -2000);

	std::string fen_mate = "2k3R1/5R2/8/8/8/4K3/8/8 b - - 0 1";
	// TODO: maybe detect mate?
	pass = pass and do_eval(fen_mate, -10000);
	return pass;
}

bool do_minimax(
	std::string fen,
	tmove expected_move
) {
	Board board;
	board.fromFen(fen);

	tmove move = board.minimax(4).move;

	if (expected_move != move) {
		std::cout << "Minimax failure!\n";
		std::cout << "Expected value:\n";
		printTmove(expected_move, board);
		std::cout << "Actual value:\n";
		printTmove(move, board);
		std::cout << fen << "\n";
		return false;
	}
	return true;
}

bool test_minimax() {
	bool pass = true;
	tmove expected_move = makeTmove("a2", "a3");
	pass = pass and do_minimax(fen_in1, expected_move);

	std::string fen_simple_capture = "3k4/8/8/8/8/8/8/3Kr3 w - - 0 1";
	pass = pass and do_minimax(fen_simple_capture, makeTmove("d1", "e1"));
	
	std::string fen_capture_choice = "1r1k4/b7/8/8/8/1B2N2p/6Q1/2K5 b - - 0 1";
	pass = pass and do_minimax(fen_capture_choice, makeTmove("h3", "g2"));

	// Handling mate
	
	std::string fen_unavoidable_mate = "2k5/5R2/6R1/8/8/4K3/8/8 b - - 0 1";
	pass = pass and do_minimax(fen_unavoidable_mate, makeTmove("c8", "b8"));

	std::string fen_losing = 
		"r1b2rk1/ppp2ppp/2n5/2bpp3/7q/7N/PPPPPnPP/RNBQKB1R w Q - 0 10";
	pass = pass and do_minimax(fen_losing, makeTmove("g2", "g3"));

	std::string fen_find_mate =
		"r1b2rk1/ppp2ppp/2n5/2bpp3/7q/7N/PPPPPnPP/RNBQKBR1 b Q - 1 10";
	pass = pass and do_minimax(fen_find_mate, makeTmove("f2", "d3"));

	return pass;
}

bool do_alphabeta(
	std::string fen,
	tmove expected_move
) {
	Board board;
	board.fromFen(fen);

	// auto stats = board.alphabeta(5, -1000000, 1000000);
	auto stats = board.alphabeta(7, -1000000, 1000000);
	tmove move = stats.move;
	int val = stats.val;
	std::vector<tmove> best_line = stats.pv;

	if (expected_move != move) {
		std::cout << "Expect print errors deeper as board get out of sync:\n";
		for (tmove pv_move : best_line) {
			printTmove(pv_move, board);
		}

		std::cout << "Alpha Beta failure!\n";
		std::cout << "Expected move:\n";
		printTmove(expected_move, board);
		std::cout << "Actual move:\n";
		printTmove(move, board);
		std::cout << "With value: " << val << "\n";
		std::cout << fen << "\n";
		return false;
	}
	return true;
}

bool test_alphabeta(){
	bool pass = true;
	std::string fen_a =
		"rnbqkb1r/ppp2ppp/5n2/P2pp3/8/8/1PPPPPPP/RNBQKBNR w KQkq - 1 4";

	/*
	tmove expected_move = makeTmove("b2", "b3");
	pass = pass and do_alphabeta(fen_a, expected_move);

	std::string fen_simple_capture = "3k4/8/8/8/8/8/8/3Kr3 w - - 0 1";
	pass = pass and do_alphabeta(fen_simple_capture, makeTmove("d1", "e1"));
	
	std::string fen_capture_choice = "1r1k4/b7/8/8/8/1B2N2p/6Q1/2K5 b - - 0 1";
	pass = pass and do_alphabeta(fen_capture_choice, makeTmove("a7", "e3"));
	*/

	// Handling mate
	
	/*
	std::string fen_unavoidable_mate = "2k5/5R2/6R1/8/8/4K3/8/8 b - - 0 1";
	pass = pass and do_alphabeta(fen_unavoidable_mate, makeTmove("c8", "b8"));
	*/

	std::string fen_losing = 
		"r1b2rk1/ppp2ppp/2n5/2bpp3/7q/7N/PPPPPnPP/RNBQKB1R w Q - 0 10";
	pass = pass and do_alphabeta(fen_losing, makeTmove("g2", "g3"));

	/*
	std::string fen_find_mate =
		"r1b2rk1/ppp2ppp/2n5/2bpp3/7q/7N/PPPPPnPP/RNBQKBR1 b Q - 1 10";
	pass = pass and do_alphabeta(fen_find_mate, makeTmove("f2", "d3"));
	*/

	return pass;
}

int main () {
	//test_positions();
	//test_minimax();
	test_alphabeta();
	return 0;
}
