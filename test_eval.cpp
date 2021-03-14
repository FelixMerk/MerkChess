
#include <iostream>
#include <set>
#include "board.h"

std::string fen_in1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
std::string fen_in2 = "4k2r/8/8/8/8/8/8/2B1K3 w - - 0 1";


bool do_test(
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
	pass = pass and do_test(fen_in1, 0);
	pass = pass and do_test(fen_in2, -2000);
	return pass;
}

int main () {
	test_positions();
	return 0;
}
