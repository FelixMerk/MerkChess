
#include <iostream>
#include <set>
#include "board.h"



std::string fen_in1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";


tmove getUsersMove(Board& board) {
	std::string umove;
	std::cout << "Enter your move (like e2e4): ";
	std::getline(std::cin, umove);
	std::cout << "Your move, '" << umove << "'." << std::endl;

	tmove move;
	move = tmove(
		board.getSquareOfName(umove.substr(0,2)),
		board.getSquareOfName(umove.substr(2,2)),
		0
	);
	return move;
}

void move(Board& board){
	std::vector<tmove> moves = board.getMoves();
	if (moves.size() > 0) {
		tmove move = moves.front();
		tsquare source = std::get<0>(move);
		tsquare dest = std::get<1>(move);
		tpiece promo = std::get<2>(move);

		std::cout << board.getNameOfSquare(source) << " -> ";
		std::cout << board.getNameOfSquare(dest) << " \n";
		if (promo != 0) {
			std::cout << board.getPiece(promo) << " \n";
		}

		board.makeMove(move);
	}

}

int main() {
	int pass = 0;
	std::cout << "Hello Player\n";

	Board board;
	board.fromFen(fen_in1);

	int i = 0;
	while (i < 20){
		move(board);

		tmove user_move = getUsersMove(board);
		board.makeMove(user_move);

		if (i == 5 or i == 10) {
			std::cout << "Game state:\n";
			std::cout << board.toFen() << "\n";
		}

		i++;
	}

	std::cout << "Final game state:\n";
	std::cout << board.toFen() << "\n";

	return 0;
}