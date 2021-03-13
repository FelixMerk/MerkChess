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
	//board[0][0] = pawn | white;
	//board[2][5] = king | black;
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


std::vector<tmove> Board::stripByPin(tsquare square, std::vector<tmove> moves) {
	// Use king_pos and otherwise legal moves to strip 
	// pinned moves
	// Moves within the pin are legal
	std::vector<tmove> new_moves = {};

	bool diag = sameDiagonal(square, king_pos);
	bool row = sameRowOrColumn(square, king_pos);

	for (tmove move : moves) {
		tsquare dest = std::get<1>(move);
		if (diag) {
			if (sameDiagonal(dest, king_pos)) {
				new_moves.push_back(move);
			}
		} else if (row) {
			if (sameRowOrColumn(dest, king_pos)) {
				new_moves.push_back(move);
			}
		}
	}
	return new_moves;
}


bool Board::isAbsolutePinned(tsquare square){
	// Is square pinned to king_pos?
	//
	//
	bool diag = sameDiagonal(square, king_pos);
	bool row = sameRowOrColumn(square, king_pos);
	
	if (!diag and !row) {
		return false;
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
	// On diagonals both will hit the side at once
	// For rows we can follow a side of the board
	while ((i != 0 and i != 7) or (j != 0 and j != 7)) {
		i += rise;
		j += run;
		tpiece piece = board[i][j];
		if (piece) {
			if (to_play == (to_play & piece)){
				// our piece
				return false;
			} else {
				// enemy piece
				if (row) {
					if (rook == (piece & rook)) {
						possible_pin = true;
						break;
					} else if (queen == (piece & queen)) {
						possible_pin = true;
						break;
					}
				} else if (diag) {
					if (bishop == (piece & bishop)) {
						possible_pin = true;
						break;
					} else if (queen == (piece & queen)) {
						possible_pin = true;
						break;
					}
				}
				// Otherwise we have a piece that can't pin
				return false;
			}
		}
	}
	if (!possible_pin) {
		// We reached the edge witout seeing any new pieces
		return false;
	}

	// 2. Check for pieces on the close side
	while ((ai != ki) or (aj != kj)) {
		ai -= rise;
		aj -= run;

		tpiece piece = board[i][j];
		if (piece) {
			if (piece != (king | to_play)) {
				return false;
			}
		}
	}

	// We reached the king and are pinned
	return true;
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

	if ((piece | opponent) == knight){
		return squares;
	}

	if ((piece | opponent) == pawn){
		// En passent could also be legal
		return squares;
	}

	if ((piece | opponent) == king){
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
	if ((i == ki) and (j = kj)) {
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

std::vector<tmove> Board::stripIllegal(std::vector<tmove> moves) {
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
						white | queen));
				moves.push_back(
					tmove(square, tsquare(i+iinc, j+jinc),
						white | bishop));
				moves.push_back(
					tmove(square, tsquare(i+iinc, j+jinc),
						white | knight));
				moves.push_back(
					tmove(square, tsquare(i+iinc, j+jinc),
						white | rook));
			} else {
				moves.push_back(tmove(square, tsquare(i+iinc, j+jinc), 0));
			}
		}
	};

	if (to_play == white) {
		sub_pawn(-1, 0, 6, 1);
		if (j!=0) {
			sub_pawn(-1, -1, 6, -1);
		}
		if (j!=7) {
			sub_pawn(-1, 1, 6, -1);
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
		} else if (((piece ^ to_play) != king) and  isAbsolutePinned(square)) {
			// Handle Absolute Pins
			new_moves = stripByPin(square, new_moves);
		}
		moves.insert(moves.end(), new_moves.begin(), new_moves.end());
	}
	return moves;
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
