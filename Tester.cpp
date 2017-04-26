#include <iostream>
#include <chrono>
#include "Sudoku.h"

using namespace std;
typedef chrono::steady_clock Time;
typedef chrono::duration<float> fsec;

#define NUM_TESTS 100

int main()
{
	int board[9][9] = {
			{0,2,0, 5,0,9, 0,8,0},
			{5,0,9, 0,2,0, 6,0,4},
			{0,3,0, 6,0,4, 0,9,0},

			{1,4,3, 8,6,5, 7,2,9},
			{9,5,8, 2,4,7, 3,6,1},
			{7,6,2, 3,9,1, 4,5,8},

			{3,7,1, 9,5,6, 8,4,2},
			{4,9,6, 1,8,2, 5,7,3},
			{2,8,5, 4,7,3, 9,1,6}};

	Sudoku s(board);

	/*
	 * Section for timing multithreaded solution
	 */
	auto start = Time::now();
	s.setMultithread(true);
	for (int i = 0; i < NUM_TESTS; i++) {
		s.newBoard(); // comment out to use board as defined above
		s.check();
		// Print statements, if desired:
		//cout << "Entered board:\n";
		//s.printBoard();
		//cout << "\nSolution:\n";
		//s.printSolution();
		//cout << endl;
	}
	auto end = Time::now();
	fsec fs = end - start;
	cout << "With multithreading, it takes " << fs.count() <<
			" seconds to generate " << NUM_TESTS << " boards and solve them.\n";

	/*
	 * Section for timing non-multithreaded solution
	 */
	start = Time::now();
	s.setMultithread(false);
	for (int i = 0; i < NUM_TESTS; i++) {
		s.newBoard(); // comment out to use board as defined above
		s.check();
		// Print statements, if desired:
		//cout << "Entered board:\n";
		//s.printBoard();
		//cout << "\nSolution:\n";
		//s.printSolution();
		//cout << endl;
	}
	end = Time::now();
	fs = end - start;
	cout << "Without multithreading, it takes " << fs.count() <<
			" seconds to generate " << NUM_TESTS << " boards and solve them.\n\n";


	cout << "Multithreading is slower than just solving the Sudoku directly.\n" <<
			"The cost of setting up so many threads is too high for this simple problem.\n";
	return 0;
}
