#ifndef Board_H
#define Board_H

#include <string>
#include <tuple>
#include <vector>
#include <set>
#include <algorithm>

typedef std::tuple<int, int> tsquare;
typedef unsigned char tpiece;
// piece in move for promotions
typedef std::tuple<tsquare, tsquare, tpiece> tmove;

typedef struct check_info
{
	int n;
	tsquare checker;
} check_info;

typedef struct complete_move_info
{
	tmove move;
	tpiece piece;
	std::string ep;
	int K;
	int Q;
	int k;
	int q;
} complete_move_info;

typedef struct minimax_val
{
	tmove move;
	int val;
	std::vector<tmove> pv;
} minimax_val;


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
		tsquare king_pos;

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
		std::string getNameOfSquare(tsquare square);
		tsquare getSquareOfName(std::string name);

		std::vector<tmove> stripIllegal(const std::vector<tmove> &moves);
		std::vector<tmove> getKnightMoves(tsquare square);
		std::vector<tmove> getBishopMoves(tsquare square);
		std::vector<tmove> getRookMoves(tsquare square);
		std::vector<tmove> getQueenMoves(tsquare square);
		std::vector<tmove> getPawnMoves(tsquare square);
		std::vector<tmove> getKingMoves(tsquare square);

		check_info inCheck(tsquare square);

		bool sameDiagonal(tsquare sq_a, tsquare sq_b);
		bool sameRowOrColumn(tsquare sq_a, tsquare sq_b);
		std::vector<tmove> stripByPin(
			tsquare square,
			const std::vector<tmove> &moves,
			tsquare pinner
		);
		bool isAbsolutePinned(tsquare square);
		tsquare getAbsolutePinner(tsquare square);
		bool isEpDoublePinned(tmove move);
		std::set<tsquare> checkStops(tsquare k_pos, tsquare attacker);

		complete_move_info makeMove(tmove move);
		void undoMove(complete_move_info info);
		std::vector<tmove> orderMoves(
			const std::vector<tmove> &moves,
			const std::vector<tmove> &old_pv = {}
		);

		int evaluate();
		minimax_val minimax(int depth);
		minimax_val alphabeta(
			int depth, int alpha, int beta,
			std::vector<tmove> old_pv = {}
		);

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
