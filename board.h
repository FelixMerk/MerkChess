#ifndef Board_H
#define Board_H

#include <string>
#include <tuple>
#include <vector>

class Board
{
	private:
		int to_play;
		int white_kingside;
		int white_queenside;
		int black_kingside;
		int black_queenside;
		int halfmove;
		int fullmove;
		std::string en_passent;
		/*
		unsigned char pawn;
		unsigned char bishop;
		unsigned char knight;
		unsigned char rook;
		unsigned char queen;
		unsigned char king;
		unsigned char white;
		unsigned char black;
		*/
	public:
		unsigned char board[8][8];

		void clearBoard();
		void printBoard();
		std::string getPiece(char piece);
		void fromFen(std::string fen);
		std::string toFen();
		std::string pieceToFen(char piece);
		std::vector<int> getMoves();
		std::tuple<int,int> findKing();
		std::string numToChar(int j);

		enum Pieces
		{
			pawn = 0b001,
			bishop = 0b010,
			knight = 0b011,
			rook = 0b100,
			queen = 0b101,
			king = 0b110,
			white = 0b01000,
			black = 0b10000
		};

};

#endif
