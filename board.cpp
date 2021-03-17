#include <iostream>
#include "board.h"
#include <bitset>
#include <string>
#include <ctype.h>

void Board::clearBoard() {
	// puts some pieces on a board
	to_play = white;
	white_kingside = 0;
	white_queenside = 0;
	black_kingside = 0;
	black_queenside = 0;
	halfmove = 0;
	fullmove = 0;
	en_passent = "";

	// Board with bit showing pieces
	// x = rows, y = columns
	for (int i = 0; i < 8; i++){
		for (int j = 0; j<8; j++){
			board[i][j] = 0b0;
		}
	}
}

void Board::printBoard() {
	for (int i = 0; i < 8; i++){
		for (int j = 0; j<8; j++){
			// std::cout << "Hello Me\n";
			unsigned char piece = board[i][j];
			if (piece){
				std::cout << std::bitset<8>(board[i][j]) << " \n";
				std::cout << getPiece(board[i][j]) << " \n";
			}
		}
	}
}

std::string Board::getPiece(char piece) {
	std::string color;
	if (piece & white) {
		color = "white";
	} else if (piece & black) {
		color = "black";
	} else {
		// Code is broken
		// we should raise
		color = "failure 76";
	}
	// Strip color
	piece = piece & 0b111;
	//piece = piece & ~white;
	//piece = piece & ~black;
	std::string piece_name;
	if (piece == pawn) {
		piece_name = "pawn";
	} else if (piece == bishop) {
		piece_name = "bishop";
	} else if (piece == knight) {
		piece_name = "knight";
	} else if (piece == rook) {
		piece_name = "rook";
	} else if (piece == queen) {
		piece_name = "queen";
	} else if (piece == king) {
		piece_name = "king";
	} else {
		// Code is broken
		// we should raise
		color = "failure 77";
	}

	return color + " " + piece_name;
}


std::string Board::numToChar(int j) {
	char character = 'a' + j;
	std::string letter = "";
	letter += character;
	return letter;
}

std::string Board::getNameOfSquare(tsquare square) {
	std::string square_string = "";
	square_string += numToChar(std::get<1>(square));
	square_string += std::to_string(8 - std::get<0>(square));
	return square_string;
}

tsquare Board::getSquareOfName(std::string name) {
	int j = name[0] - 'a';
	int i = 8 - (name[1] - '0');
	return tsquare(i,j);
}


bool Board::sameDiagonal(tsquare sq_a, tsquare sq_b) {
	int ai = std::get<0>(sq_a);
	int aj = std::get<1>(sq_a);

	int bi = std::get<0>(sq_b);
	int bj = std::get<1>(sq_b);

	if (ai - bi == aj - bj) {
		return true;
	} else if (ai - bi == - (aj - bj)) {
		return true;
	}
	return false;
}

bool Board::sameRowOrColumn(tsquare sq_a, tsquare sq_b) {
	int ai = std::get<0>(sq_a);
	int aj = std::get<1>(sq_a);

	int bi = std::get<0>(sq_b);
	int bj = std::get<1>(sq_b);

	if ((ai == bi) or (aj == bj)) {
		return true;
	}
	return false;
}


std::vector<tmove> Board::stripByPin(
	tsquare square,
	const std::vector<tmove> &moves,
	tsquare pinner
) {
	// Use king_pos and otherwise legal moves to strip 
	// pinned moves
	// Moves within the pin are legal
	std::vector<tmove> new_moves = {};

	bool diag = sameDiagonal(square, king_pos);
	bool row = sameRowOrColumn(square, king_pos);

	for (tmove move : moves) {
		tsquare dest = std::get<1>(move);
		if (diag) {
			if (sameDiagonal(dest, king_pos) and
				sameDiagonal(dest, pinner)
			) {
				new_moves.push_back(move);
			}
		} else if (row) {
			if (sameRowOrColumn(dest, king_pos) and
				sameRowOrColumn(dest, pinner)
			) {
				new_moves.push_back(move);
			}
		}
	}
	return new_moves;
}


bool Board::isAbsolutePinned(tsquare square){
	tsquare pinner = getAbsolutePinner(square);
	if (pinner == tsquare(-1, -1)) {
		return false;
	} else {
		return true;
	}

}

tsquare Board::getAbsolutePinner(tsquare square){
	// Is square pinned to king_pos?
	//
	//
	bool diag = sameDiagonal(square, king_pos);
	bool row = sameRowOrColumn(square, king_pos);
	tsquare pinner = tsquare(-1,-1);
	
	if (!diag and !row) {
		// false
		return tsquare(-1,-1);
	}

	int opponent;
	if (to_play == white) {
		opponent = black;
	} else {
		opponent = white;
	}

	int ai = std::get<0>(square);
	int aj = std::get<1>(square);

	int ki = std::get<0>(king_pos);
	int kj = std::get<1>(king_pos);

	// Extract directional info
	int rise = (0 < (ai - ki)) - ((ai - ki) < 0);
	int run = (0 < (aj - kj)) - ((aj - kj) < 0);

	// 1. Check for attackers on the far side
	int i = ai;
	int j = aj;
	bool possible_pin = false;
	// On diagonals both will hit one side
	// For rows we can follow a side of the board
	while (
		(i != 0 and i != 7 and j != 0 and j != 7) or 
		(i != 0 and i != 7 and run == 0) or
		(j != 0 and j != 7 and rise == 0)
	) {
		i += rise;
		j += run;
		tpiece piece = board[i][j];
		if (piece) {
			if (to_play == (to_play & piece)){
				// our piece
				// false
				return tsquare(-1,-1);
			} else {
				// enemy piece
				if (row) {
					if (piece == (rook | opponent)) {
						possible_pin = true;
						pinner = tsquare(i,j);
						break;
					} else if (piece == (queen | opponent)) {
						possible_pin = true;
						pinner = tsquare(i,j);
						break;
					}
				} else if (diag) {
					if (piece == (bishop | opponent)) {
						possible_pin = true;
						pinner = tsquare(i,j);
						break;
					} else if (piece == (queen | opponent)) {
						possible_pin = true;
						pinner = tsquare(i,j);
						break;
					}
				}
				// Otherwise we have a piece that can't pin
				return tsquare(-1,-1);
			}
		}
	}
	if (!possible_pin) {
		// We reached the edge witout seeing any new pieces
		return tsquare(-1,-1);
	}

	// 2. Check for pieces on the close side
	while ((ai != ki) or (aj != kj)) {
		ai -= rise;
		aj -= run;

		tpiece piece = board[ai][aj];
		if (piece) {
			if (piece != (king | to_play)) {
				return tsquare(-1,-1);
			}
		}
	}

	// We reached the king and are pinned
	return pinner;
}

bool Board::isEpDoublePinned(tmove move) {
	// K _ P p _ _ r

	tsquare source = std::get<0>(move);
	int i = std::get<0>(source);
	int j = std::get<1>(source);
	tpiece piece = board[i][j];

	tsquare dest = std::get<1>(move);

	int opponent;
	if (to_play == white) {
		opponent = black;
	} else {
		opponent = white;
	}

	if (
		piece == (pawn | to_play) and
		dest == getSquareOfName(en_passent)
	) {
		int dj = std::get<1>(dest);

		// Check king position
		int ki = std::get<0>(king_pos);
		int kj = std::get<1>(king_pos);
		if (ki != i) {
			return false;
		}

		// Find King
		if (j > kj) {
			// Search left
			int aj = std::min(j, dj);
			while (aj != 0){
				aj--;
				tpiece piece = board[i][aj];

				if (piece) {
					if (piece != (king | to_play)) {
						return false;
					}
				}
			}
		} else {
			// Search right
			int aj = std::max(j, dj);
			while (aj != 7){
				aj++;
				tpiece piece = board[i][aj];

				if (piece) {
					if (piece != (king | to_play)) {
						return false;
					}
				}
			}
		}

		// Find Pinner
		if (j < kj) {
			// Search left
			int aj = std::min(j, dj);
			while (aj != 0){
				aj--;
				tpiece piece = board[i][aj];

				if (piece) {
					if (
						piece == (rook | opponent) or
						piece == (queen | opponent)
					) {
						return true;
					} else {
						return false;
					}

				}
			}
		} else {
			// Search right
			int aj = std::max(j, dj);
			while (aj != 7){
				aj++;
				tpiece piece = board[i][aj];

				if (piece) {
					if (
						piece == (rook | opponent) or
						piece == (bishop | opponent)
					) {
						return true;
					} else {
						return false;
					}
				}
			}
		}
		// No pinner
		return false;

	} else {
		return false;
	}

}

std::set<tsquare> Board::checkStops(tsquare k_pos, tsquare attacker) {
	std::set<tsquare> squares = {};
	squares.insert(attacker);

	int i = std::get<0>(attacker);
	int j = std::get<1>(attacker);
	tpiece piece = board[i][j];

	int opponent;
	if (to_play == white) {
		opponent = black;
	} else {
		opponent = white;
	}

	if ((knight | opponent) == piece){
		return squares;
	}

	if ((pawn | opponent) == piece){
		// En passent could also be legal
		return squares;
	}

	if ((king | opponent) == piece){
		// This isn't really a real case
		return squares;
	}

	// Left with Bishop, Rook, Queen
	int ki = std::get<0>(k_pos);
	int kj = std::get<1>(k_pos);

	// Extract directional info
	int rise = (0 < (i - ki)) - ((i - ki) < 0);
	int run = (0 < (j - kj)) - ((j - kj) < 0);

	// we got the same square twice as input!
	if ((i == ki) and (j == kj)) {
		std::cout << getNameOfSquare(k_pos) << " " << ki << kj << " e \n";
		std::cout << getNameOfSquare(attacker) << " " << i << j << " o \n";
		throw 97;
	}

	while ((i != ki) or (j != kj)) {
		squares.insert(tsquare(i,j));
		i -= rise;
		j -= run;
	}
	return squares;
}

tsquare Board::findKing() {
	for (int i = 0; i < 8; i++){
		for (int j = 0; j<8; j++){
			unsigned char piece = board[i][j];
			if (piece){
				int pass;
				pass = (piece == (king | to_play));

				if (pass) {
					king_pos = tsquare(i,j);
					return king_pos;
				}

				// We have a piece.  It may be able to move.
			}
		}
	}
	// No King
	return std::tuple<int, int>(-1,-1);
}



std::vector<tsquare> Board::getPieces() {
	std::vector<tsquare> locations = {};
	for (int i = 0; i < 8; i++){
		for (int j = 0; j<8; j++){
			unsigned char piece = board[i][j];
			if (piece){
				if (to_play == (piece & to_play)) {
					if (piece == (king | to_play)) {
						king_pos = tsquare(i,j);
					}
					// We have a piece of the right color
					locations.push_back(tsquare(i,j));
				}
			}
		}
	}
	return locations;
}

std::vector<tmove> Board::stripIllegal(const std::vector<tmove> &moves) {
	std::vector<tmove> legal_moves = {};
	for (tmove move : moves) {
		tsquare square = std::get<1>(move);
		int i = std::get<0>(square);
		int j = std::get<1>(square);
		if (i < 0 or i > 7 or j < 0 or j > 7) {
			continue;
		}
		unsigned char destination = board[i][j];
		if (destination) {
			if (to_play == (to_play & destination)) {
				continue;
			}
		}
		legal_moves.push_back(move);
	}

	return legal_moves;
}


std::vector<tmove> Board::getKnightMoves(tsquare square) {
	std::vector<tmove> moves = {};
	int i = std::get<0>(square);
	int j = std::get<1>(square);
	std::vector<int> hop = {-1,1};
	std::vector<int> jump = {-2,2};
	for (int a : hop) {
		for (int b : jump) {
			moves.push_back(tmove(square, tsquare(i+a, j+b), 0));
			moves.push_back(tmove(square, tsquare(i+b, j+a), 0));
		}
	}
	return stripIllegal(moves);
}


std::vector<tmove> Board::getBishopMoves(tsquare square) {
	std::vector<tmove> moves = {};
	int i = std::get<0>(square);
	int j = std::get<1>(square);

	auto extend = [&](int ibound, int jbound, int iinc, int jinc) {
		int i2 = i;
		int j2 = j;
		while (i2 != ibound and j2 != jbound) {
			i2 += iinc;
			j2 += jinc;
			tpiece destination = board[i2][j2];
			if (destination) {
				if (to_play == (to_play & destination)) {
					// Blocked by our piece
					break;
				} else {
					// Opponents piece
					moves.push_back(tmove(square, tsquare(i2, j2), 0));
					break;
				}
			} else {
				moves.push_back(tmove(square, tsquare(i2, j2), 0));
			}
		}
	};

	extend(7, 7, 1, 1);
	extend(7, 0, 1, -1);
	extend(0, 7, -1, 1);
	extend(0, 0, -1, -1);

	return moves;
}

std::vector<tmove> Board::getRookMoves(tsquare square) {
	std::vector<tmove> moves = {};
	int i = std::get<0>(square);
	int j = std::get<1>(square);

	auto extend = [&](int ibound, int jbound, int iinc, int jinc) {
		int i2 = i;
		int j2 = j;
		while (i2 != ibound and j2 != jbound) {
			i2 += iinc;
			j2 += jinc;
			tpiece destination = board[i2][j2];
			if (destination) {
				if (to_play == (to_play & destination)) {
					// Blocked by our piece
					break;
				} else {
					// Opponents piece
					moves.push_back(tmove(square, tsquare(i2, j2), 0));
					break;
				}
			} else {
				moves.push_back(tmove(square, tsquare(i2, j2), 0));
			}
		}
	};

	extend(7, -1, 1, 0);
	extend(0, -1, -1, 0);
	extend(-1, 7, 0, 1);
	extend(-1, 0, 0, -1);

	return moves;
}

std::vector<tmove> Board::getQueenMoves(tsquare square) {
	std::vector<tmove> moves;
	std::vector<tmove> new_moves;
	moves = getBishopMoves(square);
	new_moves = getRookMoves(square);
	moves.insert(moves.end(), new_moves.begin(), new_moves.end());
	return moves;
}

check_info Board::inCheck(tsquare square) {
	// Returns number of checks (to account for double check)
	int check_count = 0;
	tsquare checker;
	int opponent;
	if (to_play == white) {
		opponent = black;
	} else {
		opponent = white;
	}

	// Pieces can't take their friends, so we stick a temp king in square
	int i2 = std::get<0>(square);
	int j2 = std::get<1>(square);
	tpiece checkee = board[i2][j2];
	board[i2][j2] = king | to_play;
	
	// Is this square under attack?
	
	// NOTE: We need to ignore pins for this
	// Check knight moves
	std::vector<tmove> moves;
	moves = getKnightMoves(square);
	for (tmove move : moves) {
		tsquare possible_knight = std::get<1>(move);
		int i = std::get<0>(possible_knight);
		int j = std::get<1>(possible_knight);
		if ((knight | opponent) == board[i][j]) {
			checker = possible_knight;
			check_count++;
		}
	}

	// check Bishop, Pawn, Queen p1, King p1, 
	moves = getBishopMoves(square);
	for (tmove move : moves) {
		tsquare possible_piece = std::get<1>(move);

		int i = std::get<0>(possible_piece);
		int j = std::get<1>(possible_piece);
		if ((bishop | opponent) == board[i][j]) {
			checker = possible_piece;
			check_count++;
		} else if ((queen | opponent) == board[i][j]) {
			checker = possible_piece;
			check_count++;
		} else if ((king | opponent) == board[i][j]) {
			if (std::abs(i-i2) < 2 and std::abs(j-j2) < 2) {
				checker = possible_piece;
				check_count++;
			}
		} else if ((pawn | opponent) == board[i][j]) {
			// TODO: simplify
			std::vector<tmove> pawn_moves;
			int tmp = to_play;
			to_play = opponent;
			pawn_moves = getPawnMoves(possible_piece);
			to_play = tmp;
			for (tmove pawn_move : pawn_moves) {
				tsquare pawn_dest = std::get<1>(pawn_move);
				if (pawn_dest == square) {
					checker = possible_piece;
					check_count++;
					// Don't overcount 4x promotion captures
					break;
				}
			}
		}

	}

	// check Rook, Queen p2, King p2 
	moves = getRookMoves(square);
	for (tmove move : moves) {
		tsquare possible_piece = std::get<1>(move);
		int i = std::get<0>(possible_piece);
		int j = std::get<1>(possible_piece);
		if ((rook | opponent) == board[i][j]) {
			checker = possible_piece;
			check_count++;
		} else if ((queen | opponent) == board[i][j]) {
			checker = possible_piece;
			check_count++;
		} else if ((king | opponent) == board[i][j]) {
			if (std::abs(i-i2) < 2 and std::abs(j-j2) < 2) {
				checker = possible_piece;
				check_count++;
			}
		}

	}

	check_info c = {check_count, checker};
	board[i2][j2] = checkee;
	return c;
}

std::vector<tmove> Board::getPawnMoves(tsquare square) {
	std::vector<tmove> moves = {};
	int i = std::get<0>(square);
	int j = std::get<1>(square);

	auto sub_pawn = [&](int iinc, int jinc, int start, int promo) {
		tpiece blocker = board[i+iinc][j+jinc];
		if (jinc != 0) {
			// Invert blocker for opponent's pieces
			if (blocker){
				if (to_play != (blocker & to_play)) {
					blocker = 0;
				} else {
					blocker = 1;
				}
			} else {
				blocker = 1;
			}

			std::string sq_name = getNameOfSquare(tsquare(i+iinc, j+jinc));
			// Take en_passent
			// en_passent square is always empty
			if (sq_name == en_passent) {
				blocker = 0;
			}
		}

		if (!blocker) {
			if ((i == start) and (jinc == 0)) {
				// Starting square
				moves.push_back(tmove(square, tsquare(i+iinc, j), 0));
				if (!board[i+2*iinc][j]) {
					// Move 2 and allow en passent
					moves.push_back(
						tmove(square, tsquare(i+2*iinc, j), 0)
					);
				}
			} else if (i == promo) {
				// Promotion square
				moves.push_back(
					tmove(square, tsquare(i+iinc, j+jinc),
						to_play | queen));
				moves.push_back(
					tmove(square, tsquare(i+iinc, j+jinc),
						to_play | bishop));
				moves.push_back(
					tmove(square, tsquare(i+iinc, j+jinc),
						to_play | knight));
				moves.push_back(
					tmove(square, tsquare(i+iinc, j+jinc),
						to_play | rook));
			} else {
				moves.push_back(tmove(square, tsquare(i+iinc, j+jinc), 0));
			}
		}
	};

	if (to_play == white) {
		sub_pawn(-1, 0, 6, 1);
		if (j!=0) {
			sub_pawn(-1, -1, -1, 1);
		}
		if (j!=7) {
			sub_pawn(-1, 1, -1, 1);
		}
	} else {
		sub_pawn(1, 0, 1, 6);
		if (j!=0) {
			sub_pawn(1, -1, -1, 6);
		}
		if (j!=7) {
			sub_pawn(1, 1, -1, 6);
		}
	}

	return moves;

}

std::vector<tmove> Board::getKingMoves(tsquare square) {
	std::vector<tmove> moves = {};
	int i = std::get<0>(square);
	int j = std::get<1>(square);

	// Hide the King in case king blocks checks?
	tpiece piece = board[i][j];
	board[i][j] = 0b0;

	std::vector<int> hop = {-1,0,1};
	for (int a : hop) {
		for (int b : hop) {
			if (a != 0 or b != 0) {
				if (inCheck(tsquare(i+a,j+b)).n == 0) {
					moves.push_back(tmove(
						square, 
						tsquare(i+a, j+b), 
						0
					));
				}
			}
		}
	}
	moves = stripIllegal(moves);

	board[i][j] = piece;


	// Castle rules
	if (to_play == white) {
		if (white_kingside) {
			tpiece f1 = board[7][5];
			tpiece g1 = board[7][6];
			int checks = 
				inCheck(tsquare(7,5)).n + 
				inCheck(tsquare(7,6)).n + 
				inCheck(square).n;
			if (f1 == 0 and g1 == 0 and checks == 0) {
				// kingside castle
				moves.push_back(tmove(square, tsquare(i, j+2), 0));
			}
		}
		if (white_queenside) {
			tpiece b1 = board[7][1];
			tpiece c1 = board[7][2];
			tpiece d1 = board[7][3];
			int checks = 
				inCheck(tsquare(7,2)).n + 
				inCheck(tsquare(7,3)).n + 
				inCheck(square).n;
			if (b1 == 0 and c1 == 0 and d1 == 0 and checks == 0) {
				// queenside castle
				moves.push_back(tmove(square, tsquare(i, j-2), 0));
			}
		}
	} else {
		if (black_kingside) {
			tpiece f8 = board[0][5];
			tpiece g8 = board[0][6];
			int checks = 
				inCheck(tsquare(0,5)).n + 
				inCheck(tsquare(0,6)).n + 
				inCheck(square).n;
			if (f8 == 0 and g8 == 0 and checks == 0) {
				// kingside castle
				moves.push_back(tmove(square, tsquare(i, j+2), 0));
			}
		}
		if (black_queenside) {
			tpiece b8 = board[0][1];
			tpiece c8 = board[0][2];
			tpiece d8 = board[0][3];
			int checks = 
				inCheck(tsquare(0,2)).n + 
				inCheck(tsquare(0,3)).n + 
				inCheck(square).n;
			if (b8 == 0 and c8 == 0 and d8 == 0 and checks == 0) {
				// queenside castle
				moves.push_back(tmove(square, tsquare(i, j-2), 0));
			}
		}
	}

	return moves;
}

std::vector<tmove> Board::getMoves() {
	std::vector<tmove> moves = {};
	std::vector<tsquare> locations = getPieces();

	int opponent;
	if (to_play == white) {
		opponent = black;
	} else {
		opponent = white;
	}
	
	check_info c = inCheck(king_pos);
	int check_count = c.n;
	std::set<tsquare> check_stops = {};
	if (check_count > 1) {
		// Double check.
		moves = getKingMoves(king_pos);
		return moves;
	} else if (check_count == 1) {
		check_stops = checkStops(king_pos, c.checker);
	}

	for (tsquare square : locations) {
		std::vector<tmove> new_moves;
		int i = std::get<0>(square);
		int j = std::get<1>(square);
		tpiece piece = board[i][j];
		switch(piece ^ to_play) {
			case pawn:
				new_moves = getPawnMoves(square);
				break;
			case bishop: 
				new_moves = getBishopMoves(square);
				break;
			case knight: 
				new_moves = getKnightMoves(square);
				break;
			case rook:
				new_moves = getRookMoves(square);
				break;
			case queen:
				new_moves = getQueenMoves(square);
				break;
			case king: 
				new_moves = getKingMoves(square);
				break;
			default: break;
		}

		if (((piece ^ to_play) != king) and  check_count) {
			// Handle checks
			std::vector<tmove> check_stopping_moves;
			tsquare en_passent_option = tsquare(-1, -1);

			if ((en_passent != "") and ((piece ^ to_play) == pawn)){
				// if en_passent and checker = pawn
				// Then we know that the pawn gave check
				// Since there aren't discovered pawn checks
				int ci = std::get<0>(c.checker);
				int cj = std::get<1>(c.checker);
				tpiece c_piece = board[ci][cj];
				if (c_piece == (pawn | opponent)) {
					en_passent_option =  getSquareOfName(en_passent);
				}
			}


			for (tmove move : new_moves) {
				tsquare dest = std::get<1>(move);
				if (
					check_stops.count(dest) or 
					(dest == en_passent_option)
				) {
					check_stopping_moves.push_back(move);
				}
			}

			new_moves = check_stopping_moves;
		}

		if ((piece ^ to_play) != king) {
			// Handle Absolute Pins
			tsquare potential_pinner = getAbsolutePinner(square);
			if (potential_pinner != tsquare(-1, -1)){
				new_moves = stripByPin(
					square, 
					new_moves,
					potential_pinner
				);
			}
		}

		// Handle absolute pins through two pieces
		// ie absolute en passent pins
		if (((piece ^ to_play) == pawn) and  (en_passent != "")) {
			std::vector<tmove> safe_moves;
			for (tmove move : new_moves) {
				tsquare dest = std::get<1>(move);
				if (
					dest == getSquareOfName(en_passent)
				) {
					if (!isEpDoublePinned(move)) {
						safe_moves.push_back(move);
					}
				} else {
					safe_moves.push_back(move);
				}
			}
			new_moves = safe_moves;
		}

		moves.insert(moves.end(), new_moves.begin(), new_moves.end());
	}
	return moves;
}

std::vector<tmove> Board::orderMoves(
	const std::vector<tmove> &moves,
	const std::vector<tmove> &old_pv /*= {}*/
) {
	std::vector<tmove> captures = {};
	std::vector<tmove> pins = {};
	std::vector<tmove> boring_moves = {};

	tmove prev_choice = tmove(-1,-1);
	if (!old_pv.empty()) {
		prev_choice = old_pv.back();
	}


	for (tmove move : moves) {
		tsquare source = std::get<0>(move);
		tsquare dest = std::get<1>(move);
		int di = std::get<0>(dest);
		int dj = std::get<1>(dest);
		tpiece dpiece = board[di][dj];

		if (move == prev_choice) {
			// Previous Principal Variation
			captures.insert(captures.begin(), move);
		} else if (dpiece != 0) {
			// Captures
			captures.push_back(move);
		} else if (
			sameDiagonal(king_pos, dest) or
			sameRowOrColumn(king_pos, dest)
		) {
			// Pins and Checks
			pins.push_back(move);
		} else {
			boring_moves.push_back(move);
		}
	}

	captures.insert(captures.end(), pins.begin(), pins.end());
	captures.insert(captures.end(), boring_moves.begin(), boring_moves.end());
	return captures;
}

int pieceToVal(tpiece piece) {
	int p = 1000;
	switch(piece & 0b111) {
		case Board::pawn:
			return p;
		case Board::bishop: 
			return 3*p;
		case Board::knight: 
			return 3*p;
		case Board::rook:
			return 5*p;
		case Board::queen:
			return 9*p;
		case Board::king: 
			// Leaving your king in check is illegal anyway
			return 9000*p;
		default:
			return 0;
	}
}

int pieceLoctionToVal(tpiece piece, int i, int j, int gamestate) {
	// location is normalized (i = 7 - i for white)
	// Gamestate 0 = opening, 1 = middlegame, 2 = endgame
	int p = 1000;
	int bonus = 9;
	switch(piece & 0b111) {
		case Board::pawn:
			// Good if further along
			bonus += i*i;
			if (i == 6) {
				bonus += p;
			}
			// Central pawns are better
			bonus += 10;
			if (j < 3) {
				bonus -= 20*(3-j);
			}
			if (j > 4) {
				bonus -= 20*(j-4);
			}
			break;
		case Board::bishop: 
			if (j == 0 or j == 7) {
				bonus -= 100;
			}
			if (i == 0) {
				bonus -= 100;
			}
			break;
		case Board::knight: 
			// Good if centralized
			if (j == 0 or j == 7) {
				bonus -= 300;
			}
			if (i == 0 or i == 7) {
				bonus -= 300;
			}
			if (j == 3 or j == 4) {
				bonus += 100;
			}
			if (i == 3 or i == 4) {
				bonus -= 100;
			}
			break;
		case Board::rook:
			if (i == 6) {
				bonus += 900;
			}
			break;
		case Board::queen:
			// Avoid early moves
			if (gamestate == 0) {
				if (i != 0 or j != 3) {
					bonus -= 100;
				}
			}
		case Board::king: 
			if (gamestate != 2) {
				if (i == 0) {
					bonus += 200;
				}
				if (i > 1) {
					bonus -= 200;
				}
			} else {
				// Centralize in endgame
				if (i < 3) {
					bonus -= 50*(3-i);
				}
				if (j < 3) {
					bonus -= 25*(3-j);
				}
				if (j > 4) {
					bonus -= 25*(j-4);
				}
			}
			break;
		default:
			break;
	}
	return bonus;
}

int Board::evaluate() {
	// Returns integer value of position for to_play
	int me = to_play;
	int opponent;

	if (to_play == white) {
		opponent = black;
	} else {
		opponent = white;
	}
	std::vector<tsquare> my_locations = getPieces();
	to_play = opponent;
	std::vector<tsquare> op_locations = getPieces();
	to_play = me;

	int my_val = 0;
	int op_val = 0;

	int gamestate = 0; //opening
	if (fullmove > 8) {
		// midgame
		gamestate = 1;
	}
	if (my_locations.size() < 6 or op_locations.size() < 6){
		// endgame
		gamestate = 2;
	}


	for (tsquare square : my_locations) {
		int i = std::get<0>(square);
		int j = std::get<1>(square);

		tpiece piece = pieceToVal(board[i][j]);
		my_val += pieceToVal(board[i][j]);

		int norm_i;
		if (to_play == white) {
			norm_i = 7-i;
		} else {
			norm_i = i;
		}
		my_val += pieceLoctionToVal(
			board[i][j],
			norm_i,
			j,
			gamestate
		);
	}

	for (tsquare square : op_locations) {
		int i = std::get<0>(square);
		int j = std::get<1>(square);
		op_val += pieceToVal(board[i][j]);

		int norm_i;
		if (opponent == white) {
			norm_i = 7-i;
		} else {
			norm_i = i;
		}
		op_val += pieceLoctionToVal(
			board[i][j],
			norm_i,
			j,
			gamestate
		);
	}

	return (my_val - op_val);
}

minimax_val Board::minimax(int depth) {
	int value = -9000000;
	tmove best_move;

	if (depth == 0) {
		// Should qiuescent search on checks ...
		value = evaluate();
	} else {
		std::vector<tmove> moves = getMoves();

		if (moves.size() == 0) {
			// checkmate or stalemate
			if (inCheck(king_pos).n > 0) {
				return {best_move, -900000};
			}
			else {
				return {best_move, 0};
			}
		}
			
		for (tmove move : moves) {
			// Make
			complete_move_info info = makeMove(move);

			// Check eval
			int branch_val = -1 * minimax(depth - 1).val;
			if (branch_val > value) {
				best_move = move;
				value = branch_val;
			}

			// Unmake
			undoMove(info);
		}
	}

	return {best_move, value};
}

minimax_val Board::alphabeta(
	int depth, int alpha, int beta,
	std::vector<tmove> old_pv /*= {}*/

) {
	int value = -9000000;
	tmove best_move;
	std::vector<tmove> best_line;

	if (depth == 0) {
		// Should qiuescent search on checks ...
		value = evaluate();
	} else {
		std::vector<tmove> moves = getMoves();

		if (moves.size() == 0) {
			// checkmate or stalemate
			if (inCheck(king_pos).n > 0) {
				//std::cout << "Checkmate found\n";
				//std::cout << value << "\n";
				return {best_move, -900000};
			}
			else {
				return {best_move, 0};
			}
		}

		moves = orderMoves(moves, old_pv);
		if (old_pv.size() > 0) {
			old_pv.pop_back();
		}
			
		for (tmove move : moves) {
			// Make
			complete_move_info info = makeMove(move);

			// Check eval
			auto ab_search = alphabeta(
				depth - 1,
				-beta,
				-alpha,
				old_pv
			);

			int branch_val = -1 * ab_search.val;

			// Unmake
			undoMove(info);

			if (branch_val > value) {
				/*
				if (depth == 5) {
					std::cout << "New game in town \n";
					std::cout << value << "\n";
					std::cout << branch_val << "\n";
					std::cout << "We now pick\n";
					tsquare source = std::get<0>(move);
					int i = std::get<0>(source);
					int j = std::get<1>(source);
					tsquare dest = std::get<1>(move);
					tpiece piece = board[i][j];
					std::cout << getPiece(piece) << "\n";
					std::cout << getNameOfSquare(source) << " -> ";
					std::cout << getNameOfSquare(dest) << "\n";
				}*/
				best_move = move;
				value = branch_val;
				best_line = ab_search.pv;
			}

			alpha = std::max(value, alpha);
			if (alpha >= beta) {
				break;
			}
		}
	}

	best_line.push_back(best_move);
	return {best_move, value, best_line};
}

complete_move_info Board::makeMove(tmove move) {

	tsquare source = std::get<0>(move);
	int si = std::get<0>(source);
	int sj = std::get<1>(source);
	tpiece spiece = board[si][sj];
	tsquare dest = std::get<1>(move);
	int di = std::get<0>(dest);
	int dj = std::get<1>(dest);
	tpiece dpiece = board[di][dj];
	tpiece promotion = std::get<2>(move);

	int opponent;
	if (to_play == white) {
		opponent = black;
	} else {
		opponent = white;
	}

	complete_move_info info = {
		move,
		dpiece,
		en_passent,
		white_kingside,
		white_queenside,
		black_kingside,
		black_queenside
	};

	// Move piece
	board[di][dj] = spiece;

	// Promotion
	if (promotion != 0) {
		board[di][dj] = promotion;
	}
	board[si][sj] = 0b0;


	// En Passent

	if ((spiece == (pawn | to_play)) and
		(en_passent != "") and
		(getSquareOfName(en_passent) == dest)
	){
		// Took en_passent
		if (to_play == white) {
			board[di+1][dj] = 0b0;
		}
		if (to_play == black) {
			board[di-1][dj] = 0b0;
		}
	}
	en_passent = "";
	if ((spiece == (pawn | to_play)) and
		(std::abs(si-di) == 2)
	) {
		en_passent = getNameOfSquare(tsquare((si+di)/2,sj));
	}

	// Castles
	
	if (
		spiece == (king | to_play) and
		std::abs(sj-dj) == 2
	) {
		int rj = 0;
		if (sj < dj) {
			rj = 7;
		}

		board[si][rj] = 0b0;
		board[si][(sj+dj)/2] = rook | to_play;
	}

	// King moves
	if (spiece == (king | to_play)) {
		if (to_play == white){
			white_kingside = 0;
			white_queenside = 0;
		} else if (to_play == black) {
			black_kingside = 0;
			black_queenside = 0;
		}
	}

	// Rook moves
	// Note we have to check both i and j in case of the other rook
	// or promo getting to a8/h8
	if (spiece == (rook | to_play)) {
		if (to_play == white){
			if (sj == 0 and si == 7) {
				white_queenside = 0;
			} else if (sj == 7 and si == 7) {
				white_kingside = 0;
			}
		} else if (to_play == black) {
			if (sj == 0 and si == 0) {
				black_queenside = 0;
			} else if (sj == 7 and si == 0) {
				black_kingside = 0;
			}
		}
	}

	// We capture a rook
	if (dpiece == (rook | opponent)) {
		if (opponent == white){
			if (dj == 0 and di == 7) {
				white_queenside = 0;
			} else if (dj == 7 and di == 7) {
				white_kingside = 0;
			}
		} else if (opponent == black) {
			if (dj == 0 and di == 0) {
				black_queenside = 0;
			} else if (dj == 7 and di == 0) {
				black_kingside = 0;
			}
		}
	}

	// Switch colors
	if (to_play == white) { to_play = black; }
	else { to_play = white; fullmove++; }

	return info;
}

void Board::undoMove(
	complete_move_info info
) {
	tmove move = info.move;
	tpiece piece = info.piece;
	std::string ep = info.ep;
	int K = info.K;
	int Q = info.Q;
	int k = info.k;
	int q = info.q;

	tsquare source = std::get<0>(move);
	int si = std::get<0>(source);
	int sj = std::get<1>(source);

	tsquare dest = std::get<1>(move);
	int di = std::get<0>(dest);
	int dj = std::get<1>(dest);

	tpiece dpiece = board[di][dj];

	tpiece promotion = std::get<2>(move);

	int opponent;
	if (to_play == white) {
		opponent = black;
	} else {
		opponent = white;
	}


	// Unmove piece
	board[si][sj] = dpiece;
	board[di][dj] = piece;


	// Castling rights
	white_kingside = K;
	white_queenside = Q;
	black_kingside = k;
	black_queenside = q;


	// Promotion
	if (promotion != 0) {
		board[si][sj] = pawn | opponent;
	}


	// (un) En Passent

	if ((dpiece == (pawn | opponent)) and
		(ep != "") and
		(getSquareOfName(ep) == dest)
	){
		// Took en_passent
		if (opponent == white) {
			board[di+1][dj] = pawn | to_play;
		}
		if (opponent == black) {
			board[di-1][dj] = pawn | to_play;
		}
	}
	en_passent = ep;

	// (un) Castles
	
	if (
		dpiece == (king | opponent) and
		std::abs(sj-dj) == 2
	) {
		int rj = 0;
		if (sj < dj) {
			rj = 7;
		}

		board[si][rj] = rook | opponent;
		board[si][(sj+dj)/2] = 0b0;
	}

	// Switch colors
	if (to_play == white) { to_play = black; fullmove--; }
	else { to_play = white; }
}
		
std::string Board::pieceToFen(char piece) {
	switch(piece) {
		case pawn | white: return "P";
		case pawn | black: return "p";
		case bishop | white: return "B";
		case bishop | black: return "b";
		case knight | white: return "N";
		case knight | black: return "n";
		case rook | white: return "R";
		case rook | black: return "r";
		case queen | white: return "Q";
		case queen | black: return "q";
		case king | white: return "K";
		case king | black: return "k";
		default: return "";
	}

}

std::string Board::toFen() {
	// Converts Board to fen
	std::string fen = "";
	for (int i = 0; i < 8; i++){
		int empty_count = 0;
		for (int j = 0; j<8; j++){
			std::string piece_string = pieceToFen(board[i][j]);
			if (piece_string != "") {
				if (empty_count) {
					fen = fen + std::to_string(empty_count);
					empty_count = 0;
				}
				fen = fen + piece_string;
			} else {
				empty_count++;
			}
		}

		if (empty_count) {
			fen = fen + std::to_string(empty_count);
			empty_count = 0;
		}

		if (i != 7) {
			fen = fen + "/";
		}
	}
	if (to_play == white) {
		fen = fen + " w";
	} else {
		fen = fen + " b";
	}
	if (white_kingside + white_queenside + black_kingside + black_queenside) {
		fen += " ";
		if (white_kingside) {
			fen += "K";
		}
		if (white_queenside) {
			fen += "Q";
		}
		if (black_kingside) {
			fen += "k";
		}
		if (black_queenside) {
			fen += "q";
		}

	} else {
		fen += " -";
	}
	if (en_passent != "") {
		fen += " ";
		fen += en_passent;
	} else {
		fen += " -";
	}

	fen += " " + std::to_string(halfmove);
	fen += " " + std::to_string(fullmove);
	return fen;
}

void Board::fromFen(std::string fen) {
	clearBoard();
	halfmove = -1;
	to_play = 0;
	std::string collecting_halfmoves = "";
	// Sets Board to fen
	int i = 0;
	int j = 0;
	int piece_end = 0;
	for (char &c: fen){
		if (piece_end) {
		switch(c) {
			case 'K':
				white_kingside = 1;
				break;
			case 'Q':
				white_queenside = 1;
				break;
			case 'k':
				black_kingside = 1;
				break;
			case 'q':
				black_queenside = 1;
				break;
			case '-':
			case ' ':
				if (collecting_halfmoves.length()) {
					halfmove = std::stoi(collecting_halfmoves);
				}
				collecting_halfmoves = "";
				break;
			case 'w':
				to_play = white;
				break;
			case 'b':
				if (to_play == 0) {
					to_play = black;
					break;
				}
				// Might be en passent, don't break
			default:
				if (isalpha(c)) {
					en_passent = c;
				} else if (en_passent.length() == 1) {
					// Waiting for second part of en passent
					en_passent += c;
				} else if (halfmove == -1) {
					collecting_halfmoves += c;
				} else {
					int val = c - '0';
					fullmove = 10 * fullmove + val;
				}
				break;
		}} else {
		switch(c) {
			case 'p':
				board[i][j] = pawn | black;
				j++;
				break;
			case 'P':
				board[i][j] = pawn | white;
				j++;
				break;
			case 'n':
				board[i][j] = knight | black;
				j++;
				break;
			case 'N':
				board[i][j] = knight | white;
				j++;
				break;
			case 'b':
				board[i][j] = bishop | black;
				j++;
				break;
			case 'B':
				board[i][j] = bishop | white;
				j++;
				break;
			case 'r':
				board[i][j] = rook | black;
				j++;
				break;
			case 'R':
				board[i][j] = rook | white;
				j++;
				break;
			case 'q':
				board[i][j] = queen | black;
				j++;
				break;
			case 'Q':
				board[i][j] = queen | white;
				j++;
				break;
			case 'k':
				board[i][j] = king | black;
				j++;
				break;
			case 'K':
				board[i][j] = king | white;
				j++;
				break;
			case '/':
				j = 0;
				i++;
				break;
			default:
				if (isdigit(c)) {
					j += c - '0';
				} else {
					piece_end = 1;
				}
				break;
		}
		}
	}

}
