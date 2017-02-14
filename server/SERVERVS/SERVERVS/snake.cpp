#include <vector>
#include <string>
#include <utility>
#include "stdlib.h"

class Snake {
private:
	int currDir = 0; //0 - UP, 1 - DOWN, 2 = LEFT, 3 - RIGHT
	std::vector<std::pair<int, int>> snakeArray;
	std::pair<int, int> head;
	std::pair<int, int> tail;

public:

	void setHead(int x, int y) {
		head = std::make_pair(x, y);
	}

	void setTail(int x, int y) {
		tail = std::make_pair(x, y);

	}

	//RETURNS SNAKE HEAD AND TAIL AS "x,y-x,y" (First pair is head, second pair is tail)
	std::string getPos() {
		std::string h = std::to_string(head.first) + "," + std::to_string(head.second);
		std::string t = std::to_string(tail.first) + "," + std::to_string(tail.second);
		return (h + "-" + t);
	}

	void setDirection(int dir) {
		currDir = dir;
	}

	std::string getDirection() {
		return std::to_string(currDir);
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
	std::string boardLayout;

	//constructor
	Board() {
		int row = 20;
		int col = 20;

		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				if (i == 0 || i == row - 1) {
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

		// CREATE THE STRING TO BE SENT OUT TO THE CLIENTS FOR BOARD DRAWING
		for (int i = 0; i < row; ++i) {
			for (int j = 0; j < col; ++j) {
				boardLayout += std::to_string(board[i][j]);
			}
			if (i != row - 1) { //doesn't put an "n" at the end of the string
				boardLayout += "n"; //adds an "n" at the end of every row
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


	std::string getBoard() {
		return boardLayout;
	}
};

class GameState {
private:
	Board board;
	Snake snake1; //p1 snake instance
	Snake snake2; //p2 snake instance
	std::pair<int, int> foodXY; //stores current food location

public:
	void setFood() {
		foodXY = std::make_pair(rand() % 20, rand() % 20);
		
		board.setValue(foodXY.first, foodXY.second, 2); //2 for food
	}

	//RETURNS CURRENT FOOD LOCATION AS "x,y"
	std::string getFood() {
		return (std::to_string(foodXY.first) + "," + std::to_string(foodXY.second));
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