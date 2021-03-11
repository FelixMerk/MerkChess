#include <iostream>
#include "board.h"
#include <bitset>
#include <string>
#include <ctype.h>

void Board::clearBoard() {
	// puts some pieces on a board
	to_play = 0;
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


tsquare Board::findKing() {
	for (int i = 0; i < 8; i++){
		for (int j = 0; j<8; j++){
			unsigned char piece = board[i][j];
			if (piece){
				int pass;
				if (to_play == 0) {
					pass = (piece == (king | white));
				} else {
					pass = (piece == (king | black));
				}

				if (pass) {
					return std::tuple<int, int>(i,j);
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
				// We have a piece.  It may be able to move.
				locations.push_back(tsquare(i,j));
			}
		}
	}
	return locations;
}


std::vector<tmove> Board::getMoves() {
	std::vector<tmove> moves = {};
	std::vector<tsquare> locations = getPieces();
	moves.push_back(tmove(tsquare(0,0), tsquare(0,1), 0));
	return v;
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
		if (i != 7) {
			if (empty_count) {
				fen = fen + std::to_string(empty_count);
				empty_count = 0;
			}
			fen = fen + "/";
		}
	}
	if (to_play == 0) {
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
	std::string collecting_halfmoves = "";
	// Sets Board to fen
	int i = 0;
	int j = 0;
	int piece_end = 0;
	for (char &c: fen){
		if (piece_end) {
		switch(c) {
			case 'w':
				to_play = 0;
				break;
			case 'b':
				to_play = 1;
				break;
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
