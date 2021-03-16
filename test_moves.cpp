
#include <iostream>
#include <set>
#include "board.h"



std::string fen_in1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
std::string fen_in2 = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2";
std::string fen_in3 = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
std::string fen_in4 = "k7/8/8/8/8/8/8/7K b Qkq - 20 31";

// White bishops and knight with restrictions
std::string fen_in5 = "8/5p2/7p/p3P3/2B2B2/1N6/3P3P/8 w - - 0 1";
std::string fen_in6 = "8/8/3p4/8/8/b2R4/7R/3K4 w - - 0 1";

// Pawn things
std::string fen_in7 = "8/3P4/2p5/1pP1pp2/p4P1p/2p1p1P1/PP2P3/8 w - b6 0 1";

// King things
std::string fen_in8 = "8/8/8/8/5k2/3n4/2PK4/8 w - - 0 1";

bool test_fen_conversion(Board board, std::string fen_in) {
	board.fromFen(fen_in);
	std::string fen_out = board.toFen();
	if (fen_in == fen_out) {
		return true;
	} else {
		std::cout << "Fen fail!\n";
		std::cout << "fen_in: " << fen_in << "\n";
		std::cout << "fen_out: " << fen_out << "\n";
		return false;
	}
}

bool test_fen() {
	int status = 0;
	Board board;
	status += test_fen_conversion(board, fen_in1);
	status += test_fen_conversion(board, fen_in2);
	status += test_fen_conversion(board, fen_in3);
	status += test_fen_conversion(board, fen_in4);
	status += test_fen_conversion(board, fen_in7);
	return status;
}

bool squares_in_moves(
	std::set<std::string> s1,
	std::vector<tmove> moves,
	Board board
) {
	for (std::string key : s1) {
		bool pass = false;
		for (tmove move : moves) {
			tsquare dest = std::get<1>(move);
			if (board.getNameOfSquare(dest) == key) {
				pass = true;
				break;
			}
		}
		if (!pass) {
			std::cout << key << " is missing?\n";
			return false;
		}
	}
	return true;
}

bool moves_in_squares(
	std::vector<tmove> moves,
	Board board,
	std::set<std::string> s1
) {
	for (tmove move : moves) {
		tsquare source = std::get<0>(move);
		tsquare dest = std::get<1>(move);
		tpiece mpiece = board.board[std::get<0>(source)][std::get<1>(source)];

		if (s1.count(board.getNameOfSquare(dest))) {
			continue;
		} else {
			// bad square
			std::cout << board.getPiece(mpiece) << " trying ";
			std::cout << board.getNameOfSquare(source) << "->";
			std::cout << board.getNameOfSquare(dest) << " is wrong!\n";
			return false;
		}
	}
	return true;
}

bool piece_moves_in_squares(
	std::vector<tmove> moves, tpiece piece, Board board, std::set<std::string> s1
) {
	for (tmove move : moves) {
		tsquare source = std::get<0>(move);

		tpiece mpiece = board.board[std::get<0>(source)][std::get<1>(source)];
		if (mpiece == piece) {
			tsquare dest = std::get<1>(move);
			if (s1.count(board.getNameOfSquare(dest))) {
				continue;
			} else {
				// bad square
				std::cout << board.getPiece(piece) << " fail!\n";
				std::cout << board.getNameOfSquare(dest) << " is included?\n";
				return false;
			}
		}
	}
	return true;

}

bool test_knight_moves(){
	bool pass = true;

	Board board;
	board.fromFen(fen_in1);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1 = {"a3", "c3", "f3", "h3"};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::knight,
		board,
		s1
	);

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	if (!pass) {
		std::cout << "Knight fail!\n";
	}

	return pass;
}

bool test_bishop_moves(){
	bool pass = true;

	Board board;
	board.fromFen(fen_in5);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1 = {"a6", "b5", "d5", "d3", "e6", "e3", "e2", "f7",
		"f1", "g5", "g3", "h6"};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::bishop,
		board,
		s1
	);

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	if (!pass) {
		std::cout << "Bishop fail!\n";
	}

	return pass;
}

bool test_rook_moves(){
	bool pass = true;

	Board board;
	board.fromFen(fen_in6);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1;
	s1 = {"a3", "a2", "b3", "b2", "c3", "c2", "d6", "d5", "d4", "d2", 
		"e3", "e2", "f3", "f2", "g3", "g2",
		"h8", "h7", "h6", "h5", "h4", "h3", "h1"
	};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::rook,
		board,
		s1
	);

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	if (!pass) {
		std::cout << "Rook fail!\n";
	}

	return pass;
}


bool test_pawn_moves(){
	bool pass = true;

	Board board;
	board.fromFen(fen_in7);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1;
	s1 = {"a3", "b6", "b4", "b3", "c3", "d8", "e5", "g4", "h4"};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::pawn,
		board,
		s1
	);

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	if (!pass) {
		std::cout << "Pawn fail!\n";
	}

	return pass;
}

bool test_king_pos(){
	Board board;
	board.fromFen(fen_in1);
	tsquare king_pos = board.findKing();
	if ("e1" == board.getNameOfSquare(king_pos)){
		return true;
	} else {
		std::cout << "King fail!\n";
		return false;
	}
}

bool test_name_get(){
	Board board;
	tsquare square = board.getSquareOfName("b2");
	std::string name = board.getNameOfSquare(square);
	if (name != "b2") {
		std::cout << "Name conversion failure\n";
		return false;
	} else {
		return true;
	}
}


bool test_check(){
	bool pass = true;
	Board board;
	std::string fen_checks = "8/r5K1/3p4/2K5/4K3/2k1K1b1/1K4n1/4K3 w - - 0 1";
	board.fromFen(fen_checks);

	int checks;
	checks = board.inCheck(board.getSquareOfName("b2")).n;
	pass = pass and (checks == 1);

	checks = board.inCheck(board.getSquareOfName("c5")).n;
	pass = pass and (checks == 1);

	checks = board.inCheck(board.getSquareOfName("e4")).n;
	pass = pass and (checks == 0);

	checks = board.inCheck(board.getSquareOfName("e3")).n;
	pass = pass and (checks == 1);

	checks = board.inCheck(board.getSquareOfName("e1")).n;
	pass = pass and (checks == 2);
		
	checks = board.inCheck(board.getSquareOfName("g7")).n;
	pass = pass and (checks == 1);

	// Non king spot behavior
	std::string fen_check_1 = "8/8/3p4/2pP4/3K4/8/8/8 w - c6 0 1";
	board.fromFen(fen_check_1);

	checks = board.inCheck(board.getSquareOfName("c5")).n;
	pass = pass and (checks == 1);

	// Pawn with promotion
	std::string fen_check_2 = 
		"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q2/PPPBBPpP/R4K1R w kq - 0 2";
	board.fromFen(fen_check_2);
	checks = board.inCheck(board.getSquareOfName("f1")).n;
	pass = pass and (checks == 1);


	if (!pass) {
		std::cout << "Check fail!\n";
	}

	return pass;
}

bool test_king_moves(){
	bool pass = true;
	Board board;
	board.fromFen(fen_in8);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1;
	s1 = {"c3", "d3", "d1", "e2"};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::king,
		board,
		s1
	);

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	// King would block its own check
	std::string fen_k1 = "8/2p5/K2p4/1P5r/5Rk1/8/4P1P1/8 b - - 0 1";
	board.fromFen(fen_k1);
	moves = board.getMoves();

	std::set<std::string> s2;
	s2 = {"g5", "f4", "g3"};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::king,
		board,
		s2
	);

	pass = pass and squares_in_moves(
		s2,
		moves,
		board
	);

	if (!pass) {
		std::cout << "King fail!\n";
	}

	return pass;
}


bool test_castle_moves(){
	bool pass = true;

	std::string fen_c1 = "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1";
	std::string fen_c2 = "r3k2r/8/8/8/8/8/8/R3K2R w Qkq - 0 1";
	std::string fen_c3 = "r3k2r/8/8/8/8/8/8/R3K2R w kq - 0 1";

	Board board;

	// 1
	board.fromFen(fen_c1);
	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1 = {"c1", "g1"};

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	// 2
	board.fromFen(fen_c2);
	moves = board.getMoves();

	std::set<std::string> s2 = {"c1",
		"d2", "d1", "e2", "f2", "f1"
	};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::king,
		board,
		s2
	);

	pass = pass and squares_in_moves(
		s2,
		moves,
		board
	);

	// 3

	board.fromFen(fen_c3);
	moves = board.getMoves();

	std::set<std::string> s3 = {
		"d2", "d1", "e2", "f2", "f1"
	};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::king,
		board,
		s3
	);

	pass = pass and squares_in_moves(
		s3,
		moves,
		board
	);

	// 4
	// Check blocks kingside, b1 in check (allowed) queenside
	std::string fen_c4 = "8/8/1b6/8/4b3/8/8/R3K2R w KQ - 0 1";
	board.fromFen(fen_c4);
	moves = board.getMoves();

	std::set<std::string> s4 = {
		"c1", "d2", "d1", "e2", "f1"
	};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::king,
		board,
		s4
	);

	pass = pass and squares_in_moves(
		s4,
		moves,
		board
	);

	// 5
	// King is in check
	std::string fen_c5 = "8/8/8/8/8/4r3/8/R3K2R w KQ - 0 1";
	board.fromFen(fen_c5);
	moves = board.getMoves();

	std::set<std::string> s5 = {
		"d2", "d1", "f2", "f1"
	};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::king,
		board,
		s5
	);

	pass = pass and squares_in_moves(
		s5,
		moves,
		board
	);

	if (!pass) {
		std::cout << "Castle fail!\n";
	}

	return pass;
}

bool test_sameDiagonal() {
	bool pass = true;

	Board board;

	tsquare sq_a = board.getSquareOfName("b1");
	tsquare sq_b = board.getSquareOfName("f5");
	pass = pass and (board.sameDiagonal(sq_a, sq_b) == true);

	sq_a = board.getSquareOfName("c8");
	sq_b = board.getSquareOfName("g4");
	pass = pass and (board.sameDiagonal(sq_a, sq_b) == true);

	sq_a = board.getSquareOfName("g2");
	sq_b = board.getSquareOfName("e4");
	pass = pass and (board.sameDiagonal(sq_a, sq_b) == true);

	sq_a = board.getSquareOfName("h8");
	sq_b = board.getSquareOfName("d1");
	pass = pass and (board.sameDiagonal(sq_a, sq_b) == false);

	sq_a = board.getSquareOfName("a7");
	sq_b = board.getSquareOfName("h7");
	pass = pass and (board.sameDiagonal(sq_a, sq_b) == false);

	if (!pass) {
		std::cout << "sameDiagonal broken\n";
	}
	return pass;
}

bool test_sameRow() {
	bool pass = true;

	Board board;

	tsquare sq_a = board.getSquareOfName("a7");
	tsquare sq_b = board.getSquareOfName("h7");
	pass = pass and (board.sameRowOrColumn(sq_a, sq_b) == true);

	sq_a = board.getSquareOfName("e3");
	sq_b = board.getSquareOfName("e2");
	pass = pass and (board.sameRowOrColumn(sq_a, sq_b) == true);

	sq_a = board.getSquareOfName("c8");
	sq_b = board.getSquareOfName("g4");
	pass = pass and (board.sameRowOrColumn(sq_a, sq_b) == false);
	
	if (!pass) {
		std::cout << "sameRow broken\n";
	}
	return pass;
}

bool test_pin() {
	bool pass = true;
	Board board;

	std::string fen_pins = "8/8/3r4/7b/q2R4/1P3N2/2B5/3KN1Nr w - - 0 1";
	board.fromFen(fen_pins);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1;
	s1 = {"b4", "b1",
	       	"d6", "d5", "d3", "d2", 
		"e2", "g2", "h3",
		"c1", "e2"};

	pass = pass and piece_moves_in_squares(
		moves,
		Board::white | Board::king,
		board,
		s1
	);

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	if (!pass) {
		std::cout << "Pin broken\n";
	}

	return pass;

}

bool test_ep_double_pin() {
	bool pass = true;
	Board board;

	std::string fen_d_pins = "8/2p5/3p4/KP5r/1R2Pp1k/8/6P1/8 b - e3 0 1";
	board.fromFen(fen_d_pins);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1;
	s1 = {"f3", "d5", "c6", "c5"}; // Not e3

	pass = pass and piece_moves_in_squares(
		moves,
		Board::black | Board::pawn,
		board,
		s1
	);

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	tmove move = tmove(
		tsquare(board.getSquareOfName("f4")), 
		tsquare(board.getSquareOfName("e3")), 
		0
	);
	bool pin = board.isEpDoublePinned(move);
	pass = pass and pin;

	if (!pass) {
		std::cout << "EP double pin broken\n";
	}

	return pass;

}

bool test_move_counts() {
	bool pass = true;

	Board board;

	board.fromFen(fen_in1);

	std::vector<tmove> moves = board.getMoves();

	pass = pass and (moves.size() == 20);

	if (!pass) {
		std::cout << "move count broken\n";
	}

	return pass;
}

bool test_check_blocking() {
	bool pass = true;
	Board board;

	std::string fen_check_1 = "R3r3/8/8/3N4/8/8/r6B/4K3 w - - 0 1";
	board.fromFen(fen_check_1);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1;
	s1 = {"d1", "f1",
	       	"e8", "e7", "e5", "e3"
	};

	pass = pass and moves_in_squares(
		moves,
		board,
		s1
	);

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	if (!pass) {
		std::cout << "Check block broken\n";
	}

	return pass;

}

bool test_en_passent_checking_pawn() {
	bool pass = true;
	Board board;

	std::string fen_check_1 = "8/8/3p4/2pP4/3K4/8/8/8 w - c6 0 1";
	board.fromFen(fen_check_1);

	std::vector<tmove> moves = board.getMoves();

	std::set<std::string> s1;
	s1 = {"c4", "e4", "c3", "d3", "e3",
	       	"c6"
	};

	pass = pass and moves_in_squares(
		moves,
		board,
		s1
	);

	pass = pass and squares_in_moves(
		s1,
		moves,
		board
	);

	if (!pass) {
		std::cout << "En passent check broken\n";
	}

	return pass;

}

bool test_make_moves() {
	bool pass = true;
	Board board;

	board.fromFen(fen_in1);
	std::vector<tmove> moves = board.getMoves();

	// Move 1
	tsquare source = board.getSquareOfName("e2");
	tsquare target = board.getSquareOfName("e4");
	complete_move_info info1 = board.makeMove(tmove(source, target, 0));

	std::string fen_out1 = board.toFen();
	pass = pass and (
		fen_out1 == 
		"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
	);

	// Move 2
	source = board.getSquareOfName("e7");
	target = board.getSquareOfName("e5");
	complete_move_info info2 = board.makeMove(tmove(source, target, 0));

	std::string fen_out2 = board.toFen();
	pass = pass and (
		fen_out2 == 
		"rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2"
	);

	// Unmake moves
	board.undoMove(info2);
	std::string fen_out3 = board.toFen();
	pass = pass and (
		fen_out3 == 
		fen_out1
	);
	board.undoMove(info1);
	std::string fen_out4 = board.toFen();
	pass = pass and (
		fen_out4 == 
		fen_in1
	);
	

	// Test castle
	std::string fen_pre_castle = 
		"r1bqkbnr/ppp2ppp/2np4/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq e6 0 2";
	board.fromFen(fen_pre_castle);

	source = board.getSquareOfName("e1");
	target = board.getSquareOfName("g1");
	complete_move_info info3 = board.makeMove(tmove(source, target, 0));

	std::string fen_out5 = board.toFen();
	pass = pass and (
		fen_out5 == 
		"r1bqkbnr/ppp2ppp/2np4/4p3/2B1P3/5N2/PPPP1PPP/RNBQ1RK1 b kq - 0 2"
	);

	board.undoMove(info3);
	std::string fen_out6 = board.toFen();
	pass = pass and (
		fen_out6 ==
		fen_pre_castle
	);

	// Test en passent
	std::string fen_pre_ep = 
		"1n4k1/8/8/3pP3/8/8/8/3K1B2 w - d6 0 1";
	board.fromFen(fen_pre_ep);

	source = board.getSquareOfName("e5");
	target = board.getSquareOfName("d6");
	complete_move_info info4 = board.makeMove(tmove(source, target, 0));

	std::string fen_out7 = board.toFen();
	pass = pass and (
		fen_out7 == 
		"1n4k1/8/3P4/8/8/8/8/3K1B2 b - - 0 1"
	);

	board.undoMove(info4);
	std::string fen_out8 = board.toFen();
	pass = pass and (
		fen_out8 ==
		fen_pre_ep
	);

	if (!pass) {
		std::cout << "Make move broken\n";
	}

	return pass;

}

int perft(int depth, Board board) {
	if (depth == 0) {
		std::cout << board.toFen() << "\n";
		return 1;
	}
	std::vector<tmove> moves = board.getMoves();

	int count = 0;

	if (depth == 1) {
		count = moves.size();
		//std::cout << board.toFen() << " " << count << "\n";
		return count;
	}

	for (tmove move : moves) {
		complete_move_info info = board.makeMove(move);
		count += perft(depth - 1, board);
		board.undoMove(info);
	}

	return count;
}


bool test_perft() {
	bool pass = true;
	//starting pos
	Board board;
	int count;

	/*
	board.fromFen(fen_in1);

	count = perft(2, board);
	pass = pass and count == 400;
	std::cout << count << "\n";

	count = perft(3, board);
	pass = pass and count == 8902;
	std::cout << count << "\n";
	*/

	// passes but takes a few seconds 
	/*
	count = perft(4, board);
	pass = pass and count == 197281;
	std::cout << count << "\n";
	*/

	// This takes a while, but we pass 4865609
	/*
	count = perft(5, board);
	pass = pass and count == 4865609;
	std::cout << count << "\n";
	*/

	/*
	// Kiwipete
	std::string fen_perft2 = 
		"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
	board.fromFen(fen_perft2);

	count = perft(1, board);
	pass = pass and count == 48;
	std::cout << count << "\n";

	count = perft(2, board);
	pass = pass and count == 2039;
	std::cout << count << "\n";

	count = perft(3, board);
	pass = pass and count == 97862;
	std::cout << count << "\n";

	// Pos 3
	std::string fen_perft3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
	board.fromFen(fen_perft3);

	count = perft(1, board);
	pass = pass and count == 14;
	std::cout << count << "\n";

	count = perft(2, board);
	pass = pass and count == 191;
	std::cout << count << "\n";

	count = perft(3, board);
	pass = pass and count == 2812;
	std::cout << count << "\n";

	count = perft(4, board);
	pass = pass and count == 43238;
	std::cout << count << "\n";
	*/

	std::string fen_perft4 = 
		"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
	board.fromFen(fen_perft4);

	count = perft(1, board);
	pass = pass and count == 6;
	std::cout << count << "\n";

	count = perft(2, board);
	pass = pass and count == 264;
	std::cout << count << "\n";

	count = perft(3, board);
	pass = pass and count == 9467;
	std::cout << count << "\n";

	/* Works but takes a bit
	count = perft(4, board);
	pass = pass and count == 422333;
	std::cout << count << "\n";
	*/

	// Pos 5
	
	std::string fen_perft5 = 
		"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
	board.fromFen(fen_perft5);

	count = perft(1, board);
	pass = pass and count == 44;
	std::cout << count << "\n";

	count = perft(2, board);
	pass = pass and count == 1486;
	std::cout << count << "\n";

	count = perft(3, board);
	pass = pass and count == 62379;
	std::cout << count << "\n";

	/* Works but takes a bit
	count = perft(4, board);
	pass = pass and count == 2103487;
	std::cout << count << "\n";
	*/

	// Pos 6
	
	std::string fen_perft6 = 
		"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1";
	board.fromFen(fen_perft6);

	count = perft(1, board);
	pass = pass and count == 44;
	std::cout << count << "\n";

	count = perft(2, board);
	pass = pass and count == 2079;
	std::cout << count << "\n";

	count = perft(3, board);
	pass = pass and count == 89890;
	std::cout << count << "\n";

	/* Works but takes a bit
	count = perft(4, board);
	pass = pass and count == 3894594;
	std::cout << count << "\n";
	*/

	/*
	std::string fen_perft9 = 
		"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/P2P1RPP/r2Q2K1 w kq - 0 2";
		"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/P2P1RPP/Q5K1 w kq - 0 1";
		"1r2k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P1RPP/R2Q2K1 w - - 0 1";
		"8/2p5/3p4/1P5r/KR3pk1/6P1/4P3/8 b - - 0 1";

		"8/K1p5/3p4/1P4kr/1R3p2/8/4P1P1/8 b - - 0 1";
		"8/2p5/K2p4/1P5r/5Rk1/8/4P1P1/8 b";
		"r1bqkbnr/pppppppp/2n5/7Q/4P3/8/PPPP1PPP/RNB1KBNR b KQkq";
		
	//"r3k2r/p1ppqpb1/bn2pnp1/1N1PN3/1p2P3/5Q1p/PPPBBPPP/R3K2R b KQkq - 0 1";
	// "r3k2r/p1ppqpb1/bn2pnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1";
	*/
	/*
	board.fromFen(fen_perft9);
	board.getPieces();
	board.getMoves();
	board.makeMove(tmove(
		board.getSquareOfName("d1"), 
		board.getSquareOfName("a1"),
		0
	));
	std::vector<tmove> moves = board.getMoves();
	for (tmove move : moves) {
		tsquare source = std::get<0>(move);
		tsquare dest = std::get<1>(move);
		//if (source == board.getSquareOfName("f7")) {
			std::cout << board.getNameOfSquare(source) << " ";
			std::cout << board.getNameOfSquare(dest) << " \n";
		//}
	}
	*/
	//std::cout << board.isAbsolutePinned(board.getSquareOfName("f4")) << "\n";

	if (!pass) {
		std::cout << "Perft broken\n";
	}
	return true;
}

bool test_absolute_pin() {
	bool pass = true;
	//starting pos
	Board board;
	int count;

	std::string fen_test = "rnbqkbnr/pppppppp/8/8/5P2/8/PPPPP1PP/RNBQKBNR b KQkq - 0 1";

	board.fromFen(fen_test);
	board.getPieces();

	pass = pass and (board.isAbsolutePinned(board.getSquareOfName("d7")) == 0);

	std::string fen_test2 = "3r4/8/8/8/b5b1/1n6/2NRN1N1/3KN3 w - - 0 1";
	board.fromFen(fen_test2);
	board.getPieces();

	pass = pass and (board.isAbsolutePinned(board.getSquareOfName("c2")) == 0);
	pass = pass and (board.isAbsolutePinned(board.getSquareOfName("d2")) == 1);
	pass = pass and (board.isAbsolutePinned(board.getSquareOfName("e2")) == 1);
	pass = pass and (board.isAbsolutePinned(board.getSquareOfName("e1")) == 0);
	pass = pass and (board.isAbsolutePinned(board.getSquareOfName("g2")) == 0);

	std::string fen_test3 = "3q4/8/8/8/8/1pr2b2/2PPP3/3K4 w - - 0 1";
	board.fromFen(fen_test3);
	board.getPieces();

	pass = pass and (board.isAbsolutePinned(board.getSquareOfName("c2")) == 0);
	pass = pass and (board.isAbsolutePinned(board.getSquareOfName("d2")) == 1);
	pass = pass and (board.isAbsolutePinned(board.getSquareOfName("e2")) == 1);

	std::string fen_test4 = "r1bqkbnr/pppppppp/2n5/7Q/4P3/8/PPPP1PPP/RNB1KBNR b KQkq";
	board.fromFen(fen_test4);
	board.getPieces();

	pass = pass and (board.isAbsolutePinned(board.getSquareOfName("f7")) == 1);


	if (!pass) {
		std::cout << "Abs Pin broken\n";
	}
	return pass;
}

int main() {
	int pass = 0;
	//std::cout << "Hello World\n";
	//board.printBoard();
	Board board;
	board.fromFen(fen_in1);
	//std::vector<tmove> moves = board.getMoves();

	/*
	pass += test_fen();
	pass += test_knight_moves();
	pass += test_bishop_moves();
	pass += test_king_pos();
	pass += test_rook_moves();
	pass += test_pawn_moves();
	pass += test_king_moves();
	pass += test_castle_moves();
	pass += test_check();

	pass += test_name_get();
	pass += test_sameRow();
	pass += test_sameDiagonal();
	pass += test_pin();
	pass += test_check_blocking();
	pass += test_en_passent_checking_pawn();

	pass += test_make_moves();
	pass += test_move_counts();
	*/

	pass += test_perft();

	//pass += test_ep_double_pin();
	//pass += test_absolute_pin();

	return pass;
}
