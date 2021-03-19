#ifndef Util_H
#define Util_H

#include "board.h"
#include <string>
#include <vector>

void printTmove(tmove move, Board &board);

tmove sortkthMove(
	std::list<scored_move> &scored_moves,
	int k
);

#endif
