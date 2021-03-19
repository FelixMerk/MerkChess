#include "util.h"

#include <iostream>

void printTmove(
	tmove move,
	Board &board
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

tmove sortkthMove(
	std::list<scored_move> &scored_moves,
	int k
) {
	int score = -1;
	tmove move = tmove(-1, -1);

	std::list<scored_move>::iterator it = scored_moves.begin();
	std::list<scored_move>::iterator best_iter = scored_moves.end();
	// Start at kth element
	std::advance(it, k);
	auto target = it;
	for (; it != scored_moves.end();it++) {
    		auto s_move = it;
		if (s_move->score > score) {
			move = s_move->move;
			best_iter = it;
			score = s_move->score;
		}
	}
	scored_moves.splice(target, scored_moves, best_iter);
	return move;
}
