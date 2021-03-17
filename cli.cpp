
#include <iostream>
#include <set>
#include "board.h"
#include "util.h"


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
	std::vector<tmove> moves = board.getMoves();
	if (std::find(moves.begin(), moves.end(), move) == moves.end()) {
		// move not in moves
		std::cout << "Computer thinks your move is not legal\n";
		move = getUsersMove(board);
	}


	return move;
}

void moveFirst(Board& board){
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

void moveMinimax(Board& board){
	tmove move = board.minimax(3).move;

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

std::vector<tmove> moveAlphaBeta(Board& board, std::vector<tmove> old_pv){
	int alpha = -1000000;
	int beta = 1000000;
	// Pop our previous move and opponents move
	if (!old_pv.empty()){
		old_pv.pop_back();
		if (!old_pv.empty()){
			old_pv.pop_back();
		}
	}
	auto info = board.alphabeta(6, alpha, beta, old_pv);
	tmove move = info.move;
	std::vector<tmove> pv = info.pv;
	int val = info.val;

	tsquare source = std::get<0>(move);
	tsquare dest = std::get<1>(move);
	tpiece promo = std::get<2>(move);

	std::cout << "Primary Variation: " << val << "\n";
	std::cout << "    Expect print errors deeper as board get out of sync:\n";
	for (tmove pv_move : pv) {
		std::cout << "    ";
		printTmove(pv_move, board);
	}

	std::cout << board.getNameOfSquare(source) << " -> ";
	std::cout << board.getNameOfSquare(dest) << " \n";
	if (promo != 0) {
		std::cout << board.getPiece(promo) << " \n";
	}

	board.makeMove(move);
	return pv;
}

int main() {
	int pass = 0;
	std::cout << "Hello Player\n";

	std::string fen_in1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	// fen_in1 = "rq5r/p3kp2/4p2p/1p4pQ/4N3/4R3/Pn3PPP/3R2K1 w - - 0 29";


	Board board;
	board.fromFen(fen_in1);
	std::vector<tmove> pv = {};

	int i = 0;
	while (i < 40){
		tmove user_move = getUsersMove(board);
		board.makeMove(user_move);

		//moveFirst(board);
		//moveMinimax(board);
		pv = moveAlphaBeta(board, pv);


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
