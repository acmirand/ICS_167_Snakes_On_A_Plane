#include <vector>
#include <string>
#include <utility>
#include "stdlib.h"
#include "time.h"
#include <ctime>

/****************
DIRECTIONS
0 - up
1 - down
2 - left
3 - right
*****************/
class Snake {
private:
	int currDir = 0; //0 - UP, 1 - DOWN, 2 = LEFT, 3 - RIGHT
	int player; //possibly client ID
	std::vector<std::pair<int, int>> snakeArray; //every x-y pair of each part of the snake
	std::pair<int, int> head;
	std::pair<int, int> tail;

public:
	void setPlayer(int p) {
		player = p;
	}

	int getPlayer() {
		return player;
	}

	void setHead(int x, int y) {
		head = std::make_pair(x, y);
	}

	void setTail(int x, int y) {
		tail = std::make_pair(x, y);
	}

	//RETURNS SNAKE HEAD AND TAIL AS "x,y-x,y" (First pair is head, second pair is tail)
	std::string getPosString() {
		std::string h = std::to_string(head.first) + "," + std::to_string(head.second);
		std::string t = std::to_string(tail.first) + "," + std::to_string(tail.second);
		return (h + "-" + t);
	}

	std::pair<int, int> getHead() {
		return head;
	}

	std::pair<int, int> getTail() {
		return tail;
	}

	void setDirection(int dir) {
		//Bool checks to see if it is a legal move
		bool LeftOK = (dir == 2 && currDir != 3);
		bool RightOK = (dir == 3 && currDir != 2);
		bool UpOK = (dir == 0 && currDir != 1);
		bool DownOK = (dir == 1 && currDir != 0);

		if (LeftOK || RightOK || UpOK || DownOK)
			currDir = dir;
	}

	void update() {
		switch (currDir) {
		case 0:
			head.first--;
			break;
		case 1:
			head.first++;
			break;
		case 2:
			head.second--;
			break;
		case 3:
			head.second++;
			break;
		}
	}

	std::string getDirString() {
		return std::to_string(currDir);
	}
	
};

/****************
 BOARD STATES
	0 - free
	1 - wall
	2 - snake 1
	3 - snake 2
	4 - food
*****************/
class Board {
private:
	int board[20][20];
	int row;
	int col;
	std::string boardLayout;
public:

	//constructor
	Board() {
		row = 20;
		col = 20;

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
	}

	int getValue(int x, int y) {
		return board[x][y];
	}

	// 0 - Free, 1 - Wall, 2 - Snake1, 3 - Snake2, 4 - Food
	void setValue(int x, int y, int value) {
		board[x][y] = value;
	}

	std::string getBoardString() {
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

	GameState() {
		setFood();
	}

	/****** BOARD STUFF ******/
	Board getBoard() {
		return board;
	}

	/****** SNAKE STUFF ******/
	Snake getSnake1() {
		return snake1;
	}

	Snake getSnake2() {
		return snake2;
	}

	void setSnake1Dir(int dir) {
		snake1.setDirection(dir);
	}

	void setSnake2Dir(int dir) {
		snake2.setDirection(dir);
	}

	/****** FOOD STUFF ******/
	// Sets random food
	void setFood() {

		srand(time(0));

		while (true) {
			int randX = floor(rand() % 20);
			int randY = floor(rand() % 20);

			if (board.getValue(randX, randY) == 0) {
				board.setValue(randX, randY, 4);
				foodXY = std::make_pair(randX, randY);
				break;
			}
		}
	}

	// Returns current food location
	std::pair<int, int> getFood() {
		return foodXY;
	}

	// Returns string of current food location as "x,y"
	std::string getFoodString() {
		return (std::to_string(foodXY.first) + "," + std::to_string(foodXY.second));
	}
	
	/****** COLLISION CHECKS ******/
	bool isWall(int x, int y) {
		return board.getValue(x, y) == 1;
	}

	bool isFood(int x, int y) {
		return board.getValue(x, y) == 2;
	}

	bool isSnake(int x, int y) {
		return board.getValue(x, y) == 3;
	}

};