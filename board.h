#ifndef Board_H
#define Board_H

#include <string>
#include <tuple>
#include <vector>

typedef std::tuple<int, int> tsquare;
typedef unsigned char tpiece;
// piece in move for promotions
typedef std::tuple<tsquare, tsquare, tpiece> tmove;

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

		std::vector<tsquare> getPieces();
		std::vector<tmove> getMoves();
		tsquare findKing();

		std::string numToChar(int j);
		std::string getSquare(tsquare square);

		std::vector<tmove> stripIllegal(std::vector<tmove> moves);
		std::vector<tmove> getKnightMoves(tsquare square);
		std::vector<tmove> getBishopMoves(tsquare square);
		std::vector<tmove> getRookMoves(tsquare square);
		std::vector<tmove> getQueenMoves(tsquare square);

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
