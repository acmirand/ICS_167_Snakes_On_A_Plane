#include <deque>
#include <string>
#include <sstream>
#include <utility>
#include <ctime>
#include <iostream>
#include <chrono>
#include <thread>
#include <conio.h>
//#include <windows.h>
#include "stdlib.h"
#include "time.h"
#include "websocket.h"

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

//std::string PRINT = "print:";
//
///*
//COMMAND LIST
//*/
////Scores
//std::string UPDATEP1SCORE = "updatep1score:";
//std::string UPDATEP2SCORE = "updatep2score:";
//
//
///*
//TEMPLATE
//POSSIBLY HAVE THE CLIENT SAVE THIS STRING AND USE IT WHEN IT RECEIVES RESETBOARD
//*/
////Game State commands
//std::string DRAWBOARD = "drawboard:";
//std::string RESETBOARD = "RESETBOARD:";
//
///*
//TEMPLATE PROTOCOL
//sendFood:23,14
//*/
//std::string SENDFOOD = "sendfood:";
//
///*
//TEMPLATE PROTOCOL
//FIRST X-Y PAIR IS THE HEAD AND THE SECOND IS THE TAIL
//P1POSUPDATE:23,14-23,13
//*/
//std::string P1POSUPDATE = "p1posupdate:";
//std::string P2POSUPDATE = "p2posupdate:";
//
///*
//TEMPLATE PROTOCOL
//The client will send a command that is like this.
//setdir:1
//*/
//std::string  SETDIR = "setdir:";

/****************
DIRECTIONS
0 - up
1 - down
2 - left
3 - right
*****************/
class Snake {
private:
	int currDir = 1; //0 - UP, 1 - DOWN, 2 = LEFT, 3 - RIGHT
	int player; //possibly client ID
	std::deque<std::pair<int, int>> snakeArray; //every x-y pair of each part of the snake
	std::pair<int, int> head;

public:
	void setPlayer(int p) {
		player = p;
	}

	int getPlayer() {
		return player;
	}

	// Set the Head and Tail in the Snake Array
	// Clear the contents of the array so games do not carry over into each other.
	// Pusht the head to the back and then the tail.
	// This will push the head of the snake to position 0 and tail to the last element.
	void InitializeSnake(int xH, int yH) {
		snakeArray.clear();
		snakeArray.push_back(std::make_pair(xH, yH));
	}

	void setHead(int x, int y) {
		snakeArray[0] = std::make_pair(x, y);
	}

	//RETURNS SNAKE HEAD AND TAIL AS "x,y-x,y" (First pair is head, second pair is tail)
	std::string getPosString() {
		std::string h = std::to_string(snakeArray.front().first) + "," + std::to_string(snakeArray.front().second);
		return (h);
	}

	// Access the first element in the snake array.
	std::pair<int, int> getHead() {
		return snakeArray.front();
	}

	// Access the last element in the snake array.
	std::pair<int, int> getTail() {
		return snakeArray.back();
	}

	void Insert(int x, int y) {
		snakeArray.push_front(std::make_pair(x, y));
		head = snakeArray[0];
	}

	// Return the last element in the array and then pop it from the array.
	std::pair<int, int> RemoveTail() {
		std::pair<int, int> tail = getTail();
		snakeArray.pop_back();
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

	void resetDirection() {
		currDir = 1;
	}
	//void update() {

	//	int nextX = head.first;
	//	int nextY = head.second;

	//	switch (currDir) {
	//	case 0:
	//		nextY--;
	//		break;
	//	case 1:
	//		nextY++;
	//		break;
	//	case 2:
	//		nextX--;
	//		break;
	//	case 3:
	//		nextX++;
	//		break;
	//	}

	//	tail.first = nextX;
	//	tail.second = nextY;
	//}

	std::string getDirString() {
		return std::to_string(currDir);
	}

	int GetDir() {
		return currDir;
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

	void ResetBoard() {
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
};

class GameState {
private:
	Board board;
	Snake snake1; //p1 snake instance
	Snake snake2; //p2 snake instance
	int snake1score = 0;
	int snake2score = 0;
	std::pair<int, int> foodXY; //stores current food location

	int frames = 0;
	double frameSpeed = 60;

	bool gameOver = false;

	webSocket* serverRef;
	std::vector<int> clientIDs;
	ostringstream os;

	std::pair<int, int> oldTail_1;
	std::pair<int, int> oldTail_2;

public:

	GameState(webSocket* server) {
		serverRef = server;
	}

	void Init() {
		/***********************
		Server Game State
		***********************/
		//reset score
		snake1score = 0;
		snake2score = 0;

		//reset board
		board.ResetBoard();
		setFood();
		snake1.InitializeSnake(3, 2);
		snake2.InitializeSnake(5, 5);

		board.setValue(snake1.getHead().first, snake1.getHead().second, 2);
		board.setValue(snake2.getHead().first, snake2.getHead().second, 3);

		//reset snake direction
		snake1.resetDirection();
		snake2.resetDirection();

		/***********************
		Client Updates
		***********************/
		for (int i = 0; i < clientIDs.size(); ++i) {

			// SEND THE BOARD LAYOUT
			os.str(std::string());
			os.clear();
			os << "20x20";
			serverRef->wsSend(clientIDs[i], "drawboard:" + os.str());

			// SEND THE STARTING POSITION FOR PLAYER 1
			serverRef->wsSend(clientIDs[i], "p1posupdate:" + snake1.getPosString());

			// SEND THE STARTING POSITION FOR PLAYER 2 
			serverRef->wsSend(clientIDs[i], "p2posupdate:" + snake2.getPosString());

			serverRef->wsSend(clientIDs[i], "updateP1Score:" + std::to_string(snake1score));
			serverRef->wsSend(clientIDs[i], "updateP2Score:" + std::to_string(snake2score));

			// SEND THE STARTING FOOD POSITION
			os.str(std::string());
			os.clear();
			os << getFoodString();
			serverRef->wsSend(clientIDs[i], "sendfood:" + os.str());
		}

		//UpdateLoop();
	}

	void UpdateLoop() {

		bool snake1Ate = false;
		bool snake2Ate = false;

		//while (true) {

			//Keypress listener
			//Would need to make this work for 2 clients
			/*if (GetAsyncKeyState(VK_UP)) {
				snake1.setDirection(0);
				snake2.setDirection(0);
			}
			else if (GetAsyncKeyState(VK_DOWN)) {
				snake1.setDirection(1);
				snake2.setDirection(1);
			}
			else if (GetAsyncKeyState(VK_LEFT)) {
				snake1.setDirection(2);
				snake2.setDirection(2);
			}
			else if (GetAsyncKeyState(VK_RIGHT)) {
				snake1.setDirection(3);
				snake2.setDirection(3);
			}*/

			snake1Ate = UpdateSnake1();
			snake2Ate = UpdateSnake2();

			for (int i = 0; i < clientIDs.size(); ++i) {

				serverRef->wsSend(clientIDs[i], "p1posupdate:" + snake1.getPosString());
				serverRef->wsSend(clientIDs[i], "p2posupdate:" + snake2.getPosString());

				if (snake1Ate || snake2Ate) {

					serverRef->wsSend(clientIDs[i], "updateP1Score:" + std::to_string(snake1score));
					serverRef->wsSend(clientIDs[i], "updateP2Score:" + std::to_string(snake2score));
					serverRef->wsSend(clientIDs[i], "sendfood:" + os.str());
				}

				serverRef->wsSend(clientIDs[i], "clearp1tail:" + std::to_string(oldTail_1.first) + "," + std::to_string(oldTail_1.second));
				serverRef->wsSend(clientIDs[i], "clearp2tail:" + std::to_string(oldTail_2.first) + "," + std::to_string(oldTail_2.second));
			}

			//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		//}
	}

	void SetClientIDs(std::vector<int> ClientIDs) {
		clientIDs = ClientIDs;
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

	bool UpdateSnake1() {
		int nextX = snake1.getHead().first;
		int nextY = snake1.getHead().second;

		bool snakeAte = false;

		switch (snake1.GetDir()) {
		case 0:
			nextY--;
			break;
		case 1:
			nextY++;
			break;
		case 2:
			nextX--;
			break;
		case 3:
			nextX++;
			break;
		}

		if (isWall(nextX, nextY) || isSnake(nextX, nextY)) {
			//Init(); //resets the game
			gameOver = true;
			return false;
		}

		if (isFood(nextX, nextY)) {

			oldTail_1.first = nextX;
			oldTail_1.second = nextY;

			snake1score++;
			setFood();

			os.str(std::string());
			os.clear();
			os << getFoodString();

			snakeAte = true;
		}
		else {
			oldTail_1 = snake1.RemoveTail();
			board.setValue(oldTail_1.first, oldTail_1.second, 0);
			snakeAte = false;
		}

		snake1.Insert(nextX, nextY);
		board.setValue(snake1.getHead().first, snake1.getHead().second, 2);

		return snakeAte;
	}

	bool UpdateSnake2() {
		int nextX = snake2.getHead().first;
		int nextY = snake2.getHead().second;

		bool snakeAte = false;

		switch (snake2.GetDir()) {
		case 0:
			nextY--;
			break;
		case 1:
			nextY++;
			break;
		case 2:
			nextX--;
			break;
		case 3:
			nextX++;
			break;
		}

		if (isWall(nextX, nextY) || isSnake(nextX, nextY)) {
			//Init(); //resets the game
			gameOver = true;
			return false;
		}

		if (isFood(nextX, nextY)) {

			oldTail_2.first = nextX;
			oldTail_2.second = nextY;

			snake2score++;

			setFood();
			os.str(std::string());
			os.clear();
			os << getFoodString();

			snakeAte = true;
		}
		else {
			oldTail_2 = snake2.RemoveTail();
			board.setValue(oldTail_2.first, oldTail_2.second, 0);
			snakeAte = false;
		}

		snake2.Insert(nextX, nextY);
		board.setValue(snake2.getHead().first, snake2.getHead().second, 3);

		return snakeAte;
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

	bool isSnake(int x, int y) {
		return board.getValue(x, y) == 2 || board.getValue(x, y) == 3;
	}

	bool isFood(int x, int y) {
		return board.getValue(x, y) == 4;
	}

	bool isGameOver() {
		return gameOver;
	}
};