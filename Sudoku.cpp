// thread example
#include <iostream>       // std::cout
#include <thread>         // std::thread

using namespace std;

void checkRow(int board[9][9], bool rowValid[], int row) {
	bool nums[9];
	for (int i = 0; i < 9; i++) nums[i] = false;
	for (int c = 0; c < 9; c++) {
		int n = board[row][c];
		if (nums[n]) {
			rowValid[row] = false;
			return;
		}
		nums[n] = true;
	}
	rowValid[row] = true;
}

void checkCol(int board[9][9], bool colValid[], int col) {
	bool nums[9];
	for (int i = 0; i < 9; i++) nums[i] = false;
	for (int r = 0; r < 9; r++) {
		int n = board[r][col];
		if (nums[n]) {
			colValid[col] = false;
			return;
		}
		nums[n] = true;
	}
	colValid[col] = true;
}

void checkSqr(int board[9][9], bool sqrValid[], int sqr) {
	bool nums[9];
	for (int i = 0; i < 9; i++) nums[i] = false;
	int rstart = (sqr/3)*3, cstart = (sqr%3)*3;
	for (int r = rstart; r < rstart + 3; r++) {
		for (int c = cstart; c < cstart + 3; c++) {
			int n = board[r][c];
			if (nums[n]) {
				sqrValid[sqr] = false;
				return;
			}
			nums[n] = true;
		}
	}
	sqrValid[sqr] = true;
}

int main()
{
	int board[9][9] = {
				{6,2,4,5,3,9,1,8,7},
				{5,1,9,7,2,8,6,3,4},
				{8,3,7,6,1,4,2,9,5},
				{1,4,3,8,6,5,7,2,9},
				{9,5,8,2,4,7,3,6,1},
				{7,6,2,3,9,1,4,5,8},
				{3,7,1,9,5,6,8,4,2},
				{4,9,6,1,8,2,5,7,3},
				{2,8,5,4,7,3,9,1,6}
		};

	bool rowValid[9], colValid[9], sqrValid[9];
	thread rowCheckers[9], colCheckers[9], sqrCheckers[9];
	for (int i = 0; i < 9; i++) {
		rowCheckers[i] = thread(checkRow, board, rowValid, i);
		colCheckers[i] = thread(checkCol, board, colValid, i);
		sqrCheckers[i] = thread(checkSqr, board, sqrValid, i);
	}
	for (int i = 0; i < 9; i++) {
		rowCheckers[i].join();
		colCheckers[i].join();
		sqrCheckers[i].join();
	}

	for (bool x : rowValid) cout << x;

	return 0;
}
