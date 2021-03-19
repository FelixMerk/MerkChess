#include "util.h"

#include <iostream>


bool test_sort_kth() {
	bool pass = true;

	std::list<scored_move> scored_moves = {};
	int k = 0;

	tmove amove = tmove(tsquare(0,0), tsquare(1,1), 0);
	tmove bmove = tmove(tsquare(1,7), tsquare(2,2), 0);
	tmove cmove = tmove(tsquare(2,7), tsquare(2,2), 0);
	tmove dmove = tmove(tsquare(3,7), tsquare(2,2), 0);

	scored_moves.push_back({amove,9});
	scored_moves.push_back({cmove,4});
	scored_moves.push_back({dmove,2});
	scored_moves.push_back({bmove,5});

	sortkthMove(scored_moves, 0);
	std::list<scored_move>::iterator it = scored_moves.begin();
	pass = pass and (amove == it->move);

	sortkthMove(scored_moves, 1);
	it = scored_moves.begin();
	std::advance(it, 1);
	pass = pass and (bmove == it->move);

	sortkthMove(scored_moves, 2);
	it = scored_moves.begin();
	std::advance(it, 2);
	pass = pass and (cmove == it->move);

	sortkthMove(scored_moves, 3);
	it = scored_moves.begin();
	std::advance(it, 3);
	pass = pass and (dmove == it->move);

	if (!pass) {
		std::cout << "sortkthMove failed!!\n";
	}
	return pass;
}

int main() {
	// Tests some util functions
	int pass = 0;
	
	pass += test_sort_kth();
	
	return pass;
}
