#include <vector>
#include <utility>
#include "stdlib.h"

class Snake {
private:
	enum directions {
		UP, DOWN, LEFT, RIGHT
	};
	directions currDir = UP;
	std::vector<std::pair<int, int>> snakeArray;
	std::pair<int, int> last;

	Snake() {};

public:
	void changeDirection(int dir) {
		
	}

	void insert() {

	}

	void remove() {

	}
	
};

/*
	0 - free
	1 - wall
	2 - food
	3 - snake
*/
class Board {
private:
	int board[20][20];

	//constructor
	Board() {
		int row = 20;
		int col = 20;

		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				if (i == 0 || i == row-1) {
					board[i][j] = 1;
				}
				else if (j == 0 || j == col - 1) {
					board[i][j] = 1; 
				}
				else {
					board[i][j] = 0;
				}
			}
		}
	}
public:
	int getValue(int x, int y) {
		return board[x][y];
	}
	void setValue(int x, int y, int value) {
		board[x][y] = value;
	}
};

class GameState {
private:
	Board board;
	Snake snake1;
	Snake snake2;

public:
	void setFood() {
		int randx = rand() % 20;
		int randy = rand() % 20;

		board.setValue(randx, randy, 2); //2 for food
	}

	bool checkWall(int x, int y) {
		if (board.getValue(x, y) == 1) return true;

		return false;
	}

	bool checkFood(int x, int y) {
		if (board.getValue(x, y) == 2) return true;

		return false;
	}

};