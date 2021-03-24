#include <iostream>
#include <fstream>

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>

#include "board/board.h"


bool is_ready = false;
bool stop_thread = false;
std::mutex m;
std::condition_variable cv;

std::vector<std::string> moves = {};

std::string fen_in1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
std::string fen = fen_in1;


std::string getInput() {
	std::string command;
	std::getline(std::cin, command);

	return command;
}

void id(){
	std::cout << "id name Merk 0.1\n";
	std::cout << "id author Felix Merk\n";
	std::cout << "option name Threads type spin default 2 min 2 max 512\n";
}

void set_position(std::string input) {
	// sets fen and/or makes moves
	return;
}


// From stackoverflow 
// stackoverflow.com/questions/236129/how-do-i-iterate-over-the-words-of-a-string
template <typename Out>
void split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}
// End from stackoverflow


void run_engine2(std::string move) {
	std::cout << "info multipv 1 depth 2 seldepth 2 nodes 118 time 0 nps 0 hashfull 0 tbhits 0 score cp 5 pv d2d4 e7e5\n";
}


void run_engine() {
	// Wait for lock, then run engine
	bool quit = false;
	std::cout << "doing those\n";

	Board board;
	board.fromFen(fen_in1);

	while (!quit){
		std::unique_lock<std::mutex> lk(m);
		while (!is_ready) {
			cv.wait(lk);
		}
		std::cout << "doing that\n";

		board.fromFen(fen);
		for (std::string move : moves) {
			tmove board_move = board.getMoveOfName(move);
			board.makeMove(board_move);
		}
		

		auto info  = board.alphabeta(4, -1000000, 1000000, {});

		tmove move = info.move;
		tsquare source = std::get<0>(move);
		tsquare dest = std::get<1>(move);
		tpiece promo = std::get<2>(move);

		std::cout << "info multipv 1 depth 4 seldpeth 4 ";
		std::cout << "score cp " << info.val / 10;
		std::cout << " pv " << board.getNameOfSquare(source);
		std::cout << board.getNameOfSquare(dest) << "\n";

		std::cout << "bestmove " << board.getNameOfSquare(source);
		std::cout << board.getNameOfSquare(dest) << "\n";


		is_ready = false;

		// std::unique_lock<std::mutex> lk2(m);
		quit = stop_thread;
	}
	std::cout << "Stopping engine\n";
}


std::string concat_with_space(std::vector<std::string> v){
	// concats with space as delim
	// from stackoverflow
	std::stringstream ss;
	std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>(ss, " "));
	std::string result = ss.str();
	if (!result.empty()) {
	    result.resize(result.length() - 1); // trim trailing space
	}
	return result;
}


void repl() {
	std::string input;
	std::string command;
	bool quit = false;
	bool bob = true;


	while(!quit) {
		if (bob) {
			input = getInput();
			if (input == "go infinite") {
				bob = false;
				std::cout << "bobster\n";
			}
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}

		std::vector<std::string> words = split(input, ' ');

		if (words.size() == 0) {
			continue;
		}
		command = words.front();

		if (command == "uci"){
			id();
			std::cout << "uciok\n";
		} else if (command == "isready"){
			std::cout << "readyok\n";
		} else if (command == "ucinewgame"){
			fen = fen_in1;
			moves = {};
		} else if (command == "setoption"){
			//pass
		} else if (command == "position"){
			if (words.size() >= 2) {
				if (words.begin()[1] == "startpos") {
					fen = fen_in1;
				} else {
					// Get all the fen arguments
					std::vector<std::string> temp(words.begin() + 1, 
							words.begin() + 7);
							//words.end());
					fen = concat_with_space(temp);
				}
			}
			if ((words.size() >= 4) and (words.begin()[1] == "startpos")) {
				// given moves
				std::vector<std::string> amoves(words.begin() + 3, 
						words.end());
				moves = amoves;
			} else if (words.size() >= 8) {
				std::vector<std::string> amoves(words.begin() + 8, 
						words.end());
				moves = amoves;
			}
		} else if (command == "go"){
			// Start search
			//std::thread thread_object(run_engine, "e2e4");
			std::cout << "doing this\n";
			std::unique_lock<std::mutex> lk(m);
			std::cout << "got mutex\n";
			is_ready = true;
			cv.notify_all();
			//std::thread athread(run_engine, "e2e4");
			//std::cout << "bestmove d7d5 ponder f2f3\n";
			//std::cout << "\n";
		} else if (command == "stop"){
			// print bestmove
			std::cout << "bestmove e2e4 ponder e7e5\n";
		} else if (command == "quit"){
			quit = true;
		} else {
			quit = true;
			std::cout << "fail: unknown command: " << command << "\n";
		}
	}
}




int main() {
	// starts UCI thread following UCI protocol
	std::string command;

	std::cout << "Hello World!\n";
	std::thread athread(run_engine);
	
	repl();

	// std::unique_lock<std::mutex> lk(m);
	stop_thread = true;
	is_ready = true;
	cv.notify_one();
	athread.join();

	return 0;
}
