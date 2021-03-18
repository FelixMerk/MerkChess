#include <random>
#include "hash.h"

// TODO: We have a fixed seed for deterministic debugging behavior
uint32_t seed_val = 12345;
std::uniform_int_distribution<unsigned long int> uint_dist(0, ULONG_MAX);
std::mt19937 rng;


void ZobristHash::initTable(){
	rng.seed(seed_val);
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 10; j++) {
			for (int k = 0; k < 13; k++) {
				zobrist[i][j][k] = uint_dist(rng);
			}
		}
	}
}


unsigned long int ZobristHash::zHash(
	unsigned char (&board)[8][8],
	bool whiteToPlay,
	int enPassentFile,
	bool w_castle_k,
	bool w_castle_q,
	bool b_castle_k,
	bool b_castle_q
) {
	// Add and remove pieces this same way
	// hash ^= zobrist[i][j][piece - 9]
	unsigned long hash = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j]) {
				// 1001 White Pawn = 9
				hash ^= zobrist[i][j][board[i][j] - 9];
			}
		}
	}
	if (!whiteToPlay) {
		hash ^= zobrist[0][8][1];
	}
	hash ^= zobrist[enPassentFile][8][0];
	if (w_castle_k)
		hash ^= zobrist[0][9][0];
	if (w_castle_q)
		hash ^= zobrist[1][9][0];
	if (b_castle_k)
		hash ^= zobrist[2][9][0];
	if (b_castle_q)
		hash ^= zobrist[3][9][0];
	return hash;
}
