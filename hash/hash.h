#ifndef Hash_H
#define Hash_H


class ZobristHash
{
	private:
		// Piece numers range from
		// 01001 9
		// 10110 22
		// TODO: slight speed by making white = 00000
	public:
		unsigned long int zobrist[8][10][13];
		void initTable();
		// 32 bits for now ?
		// long long 64 bits would be less collisions
		unsigned long int zHash(
			unsigned char (&board)[8][8],
			bool whiteToPlay,
			int enPassentFile,
			bool w_castle_k,
			bool w_castle_q,
			bool b_castle_k,
			bool b_castle_q
		);
};

#endif
