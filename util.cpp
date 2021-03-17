#include "util.h"

#include <iostream>

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

