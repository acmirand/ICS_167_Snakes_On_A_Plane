#include <vector>
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
	std::pair<int, int> foodXY; //stores current food location

	int frames = 0;
	double frameSpeed = 60;

	webSocket* serverRef;
	std::vector<int> clientIDs;
	ostringstream os;

public:

	GameState(webSocket* server) {
		serverRef = server;
	}

	void UpdateLoop() {
		std::time_t startTime = std::time(0);
		std::pair<int, int> snake1_tail;
		std::pair<int, int> snake2_tail;

		while (true) {
			snake1_tail = snake1.getTail();
			snake2_tail = snake2.getTail();

			//Keypress listener
			//Would need to make this work for 2 clients
			if (GetAsyncKeyState(VK_UP)) {
				std::cout << "Pressed UP" << std::endl;
				snake1.setDirection(0);
				snake2.setDirection(0);
			} else if (GetAsyncKeyState(VK_DOWN)) {
				std::cout << "Pressed DOWN" << std::endl;
				snake1.setDirection(1);
				snake2.setDirection(1);
			} else if (GetAsyncKeyState(VK_LEFT)) {
				std::cout << "Pressed LEFT" << std::endl;
				snake1.setDirection(2);
				snake2.setDirection(2);
			} else if (GetAsyncKeyState(VK_RIGHT)) {
				std::cout << "Pressed RIGHT" << std::endl;
				snake1.setDirection(3);
				snake2.setDirection(3);
			}
			//if (!UpdateSnake1()) break;
			//if (!UpdateSnake2()) break;
			UpdateSnake1();
			UpdateSnake2();

			for (int i = 0; i < clientIDs.size(); ++i) {
				serverRef->wsSend(clientIDs[i], "p1posupdate:" + snake1.getPosString());
				serverRef->wsSend(clientIDs[i], "p2posupdate:" + snake2.getPosString());
				serverRef->wsSend(clientIDs[i], "clearp1tail:" + std::to_string(snake1_tail.first) + "," + std::to_string(snake1_tail.second));
				serverRef->wsSend(clientIDs[i], "clearp2tail:" + std::to_string(snake2_tail.first) + "," + std::to_string(snake2_tail.second));
			}
			
			std::this_thread::sleep_for(std::chrono::milliseconds(750));
		}
	}

	void SetClientIDs(std::vector<int> ClientIDs) {
		clientIDs = ClientIDs;
	}

	/****** BOARD STUFF ******/
	Board getBoard() {
		return board;
	}

	void Init() {

		// Server Game State
		board.ResetBoard();
		setFood();
		board.setValue(snake1.getHead().first, snake1.getHead().second, 2);
		board.setValue(snake1.getTail().first, snake1.getTail().second, 2);
		board.setValue(snake2.getHead().first, snake2.getHead().second, 3);
		board.setValue(snake2.getTail().first, snake2.getTail().second, 3);

		// Client Updates
		for (int i = 0; i < clientIDs.size(); ++i) {

			// SEND THE BOARD LAYOUT
			os << "20x20";
			serverRef->wsSend(clientIDs[i], "drawboard:" + os.str());

			// SEND THE STARTING POSITION FOR PLAYER 1
			os.str(std::string());
			//os.clear();
			//os << "3,2-3,2";
			SetSnake1Position(3, 2, 3, 2);
			serverRef->wsSend(clientIDs[i], "p1posupdate:" + snake1.getPosString());
			//gameState.getBoard().setValue(3, 2, 3); //sets snake 1 position on board

			// SEND THE STARTING POSITION FOR PLAYER 2 
			os.str(std::string());
			//os.clear();
			//os << "5,5-5,5";
			SetSnake2Position(5, 5, 5, 5);
			serverRef->wsSend(clientIDs[i], "p2posupdate:" + snake2.getPosString());
			//gameState.getBoard().setValue(5, 5, 4); //sets snake 1 position on board

			// SEND THE STARTING FOOD POSITION
			os.str(std::string());
			os.clear();
			os << getFoodString();
			serverRef->wsSend(clientIDs[i], "sendfood:" + os.str());
		}

		UpdateLoop();
	}

	/****** SNAKE STUFF ******/
	Snake getSnake1() {
		return snake1;
	}

	Snake getSnake2() {
		return snake2;
	}

	bool UpdateSnake1() {
		std::cout << "S1 CurrDir: " << snake1.GetDir() << std::endl;

		int nextX = snake1.getHead().first;
		int nextY = snake1.getHead().second;

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
			Init();
			return false;
		}
		else {
			board.setValue(snake1.getTail().first, snake1.getTail().second, 0);
			snake1.setHead(nextX, nextY);
			snake1.setTail(nextX, nextY);
			board.setValue(snake1.getHead().first, snake1.getHead().second, 2);
			return true;
		}
	}

	bool UpdateSnake2() {

		int nextX = snake2.getHead().first;
		int nextY = snake2.getHead().second;

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
			Init();
			return false;
		}
		else {
			board.setValue(snake2.getTail().first, snake2.getTail().second, 0);
			snake2.setHead(nextX, nextY);
			snake2.setTail(nextX, nextY);
			board.setValue(snake2.getHead().first, snake2.getHead().second, 3);
			return true;
		}
	}

	void SetSnake1Position(int xHead, int yHead, int xTail, int yTail) {
		snake1.setHead(xHead, yHead);
		snake1.setTail(xTail, yTail);
	}

	void SetSnake2Position(int xHead, int yHead, int xTail, int yTail) {
		snake2.setHead(xHead, yHead);
		snake2.setTail(xTail, yTail);
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
};