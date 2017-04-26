#include "Sudoku.h"
#include <thread>       // std::thread
#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include <iostream>     // std::cout

using namespace std;

/*
 * Default constructor.  Generates a random new board.
 */
Sudoku::Sudoku() {
	this->multithread = true;
	newBoard();
}
/*
 * Constructor with board initializer.
 */
Sudoku::Sudoku(int board[9][9]) {
	this->multithread = true;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			this->board[i][j] = board[i][j];
			this->solution[i][j] = board[i][j];
		}
	}
	solvable = solve();
}

/*
 * Constructor with board initializer and explicit multithread setting.
 */
Sudoku::Sudoku(int board[9][9], bool multithread) {
	this->multithread = multithread;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			this->board[i][j] = board[i][j];
			this->solution[i][j] = board[i][j];
		}
	}
	solvable = solve();
}

/*
 * Multithreading setter.
 */
void Sudoku::setMultithread(bool b) {multithread = b;}

/*
 * Generates a new random board.
 * Does not guarantee a unique solution (in fact almost never unique).
 */
void Sudoku::newBoard() {
	srand ( unsigned ( std::time(0) ) );
	vector<int> v;
	for (int i = 1; i < 10; i++) v.push_back(i);

	// Repeatedly shuffle a vector of [1..9]
	// Add the numbers to the board
	for (int r = 0; r < 9; r++) {
		random_shuffle(v.begin(), v.end());
		for (int c = 0; c < 9; c++) {
			board[r][c] = v[c];
			solution[r][c] = v[c];
		}
	}

	// Reusing v to enumerate the cells.
	for (int i = 10; i < 82; i++) v.push_back(i);
	random_shuffle(v.begin(), v.end());
	int r = v[0]/9, c = v[0]%9;
	board[r][c] = 0;
	solution[r][c] = 0;

	// Remove random elements until board is valid
	// Change while condition from !check() to !solve()
	// if you really want a unique solution.
	// It will take considerably longer though.
	int i = 1;
	while (!check()) {
		int r = v[i]/9, c = v[i]%9;
		board[r][c] = 0;
		solution[r][c] = 0;
		i++;
	}
	solvable = solve();

}

/*
 * Sets the new board
 */
void Sudoku::newBoard(int board[9][9]) {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			this->board[i][j] = board[i][j];
			this->solution[i][j] = board[i][j];
		}
	}
	solvable = solve();
}

/*
 * Prints board
 */
void Sudoku::printBoard() {
	cout << "\n|-----|-----|-----|" << endl;
	for(int i = 0; i < 9; i++)
	{
		for(int j = 0; j < 9; j++) {
			if (board[i][j]) cout << "|" << board[i][j];
			else cout << "| ";
		}
		cout << "|" << endl;

		if((i+1)%3 == 0)
			cout << "|-----|-----|-----|" << endl;
	}
}

/*
 * Prints solution
 */
void Sudoku::printSolution() {
	if (!solvable) {
		cout << "The board is not solvable.\n";
		return;
	}
	cout << "\n|-----|-----|-----|" << endl;
	for(int i = 0; i < 9; i++)
	{
		for(int j = 0; j < 9; j++) {
			if (solution[i][j]) cout << "|" << solution[i][j];
			else cout << "| ";
		}
		cout << "|" << endl;

		if((i+1)%3 == 0)
			cout << "|-----|-----|-----|" << endl;
	}
}

/*
 * Checks entire board.
 */
bool Sudoku::check() {
	if (multithread) {
		thread rowCheckers[9], colCheckers[9], sqrCheckers[9];
		for (int i = 0; i < 9; i++) {
			rowCheckers[i] = getRowThread(i);
			colCheckers[i] = getColThread(i);
			sqrCheckers[i] = getSqrThread(i);
		}
		for (int i = 0; i < 9; i++) {
			rowCheckers[i].join();
			colCheckers[i].join();
			sqrCheckers[i].join();
		}
	}

	else {
		for (int i = 0; i < 9; i++){
			checkRow(i);
			checkCol(i);
			checkSqr(i);
		}
	}

	for (bool b : rowValid) if (!b) return false;
	for (bool b : colValid) if (!b) return false;
	for (bool b : sqrValid) if (!b) return false;
	return true;
}

/*
 * Checks single cell.
 */
bool Sudoku::check(int row, int col) {
	int sqr = (row/3)*3+(col/3);

	if (multithread) {
		thread rowChecker = getRowThread(row);
		thread colChecker = getColThread(col);
		thread sqrChecker = getSqrThread(sqr);
		rowChecker.join();
		colChecker.join();
		sqrChecker.join();

	}

	else {
		checkRow(row);
		checkCol(col);
		checkSqr(sqr);
	}

	if (rowValid[row] && colValid[col] && sqrValid[sqr]) return true;
	return false;
}

/*
 * Standard backtracking solver.
 * Time complexity is O(9^N) where N is # of blanks.
 * I can probably shave a little time off by not restarting the loops
 * in every recursive call.
 * But this is not an algorithms class so I'm not too concerned.
 */
bool Sudoku::solve() {
	for (int r = 0; r < 9; r++) {
		for (int c = 0; c < 9; c++) {
			if (!solution[r][c]) {
				for (int n = 1; n <= 9; n++) {
					solution[r][c] = n;
					if (check(r,c) && solve()) return true;
					solution[r][c] = 0;
				}
				return false;
			}
		}
	}
	return true;
}

/*
 * Checks row.
 */
void Sudoku::checkRow(int row) {
	bool nums[10];
	for (int i = 0; i < 10; i++) nums[i] = false;

	for (int c = 0; c < 9; c++) {
		int n = solution[row][c];
		if (n && nums[n]) {
			rowValid[row] = false;
			return;
		}
		nums[n] = true;
	}
	rowValid[row] = true;
}

/*
 * Checks column.
 */
void Sudoku::checkCol(int col) {
	bool nums[10];
	for (int i = 0; i < 10; i++) nums[i] = false;

	for (int r = 0; r < 9; r++) {
		int n = solution[r][col];
		if (n && nums[n]) {
			colValid[col] = false;
			return;
		}
		nums[n] = true;
	}
	colValid[col] = true;
}

/*
 * Checks square.
 */
void Sudoku::checkSqr(int sqr) {
	bool nums[10];
	for (int i = 0; i < 10; i++) nums[i] = false;

	int rstart = (sqr/3)*3, cstart = (sqr%3)*3;
	for (int r = rstart; r < rstart + 3; r++) {
		for (int c = cstart; c < cstart + 3; c++) {
			int n = solution[r][c];
			if (n && nums[n]) {
				sqrValid[sqr] = false;
				return;
			}
			nums[n] = true;
		}
	}
	sqrValid[sqr] = true;
}

/*
 * Returns threads for the checker functions.
 */
thread Sudoku::getRowThread(int row) {return thread(&Sudoku::checkRow, this, row);}
thread Sudoku::getColThread(int col) {return thread(&Sudoku::checkCol, this, col);}
thread Sudoku::getSqrThread(int sqr) {return thread(&Sudoku::checkSqr, this, sqr);}
