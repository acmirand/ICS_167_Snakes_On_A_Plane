#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <algorithm>
#include "websocket.h"
#include "snake.cpp"

using namespace std;

webSocket server;
GameState gameState;

std::string PRINT = "print:";

/*
	COMMAND LIST
*/
//Scores
std::string UPDATEP1SCORE = "updatep1score:";
std::string UPDATEP2SCORE = "updatep2score:";


/*
TEMPLATE
DRAWBOARD:111111111111n100000000001n100000000001n100000000001n100000000001n100000000001n100000000001n100000000001n111111111111
POSSIBLY HAVE THE CLIENT SAVE THIS STRING AND USE IT WHEN IT RECEIVES RESETBOARD
*/
//Game State commands
std::string DRAWBOARD = "drawboard:";
std::string RESETBOARD = "RESETBOARD:";

/*
TEMPLATE PROTOCOL
sendFood:23,14
*/
std::string SENDFOOD = "sendfood:";

/*
TEMPLATE PROTOCOL
FIRST X-Y PAIR IS THE HEAD AND THE SECOND IS THE TAIL
P1POSUPDATE:23,14-23,13
*/
std::string P1POSUPDATE = "p1posupdate:";
std::string P2POSUPDATE = "p2posupdate:";

/*
TEMPLATE PROTOCOL
The client will send a command that is like this.
setdir:1
*/
std::string  SETDIR = "setdir:";

/* called when a client connects */
void openHandler(int clientID){
    ostringstream os;
    os << "Stranger " << clientID << " has joined.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
    server.wsSend(clientID, "");
}

/* called when a client disconnects */
void closeHandler(int clientID){
    ostringstream os;
    os << "Stranger " << clientID << " has left.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message){

    ostringstream os;
	int cmdCutOff = 0;

	// Parse the string until it reaches a ":". This will mark the command being passed in.
	// Everything after that point will be consider data to be dealt with.
	std::string command;
	for (int i = 0; message.length(); ++i) {
		if (message[i] == 58) {
			cmdCutOff = i;
			break;
		}
	}

	// Parse the info for processing.
	os << message.substr(cmdCutOff + 1);
	command = message.substr(0, cmdCutOff);
	std::transform(command.begin(), command.end(), command.begin(), ::tolower);

	if (command == "startgame") {

		// SEND THE BOARD LAYOUT
		os << "20x20";
		server.wsSend(clientID, DRAWBOARD + os.str());

		// SEND THE STARTING POSITION FOR PLAYER 1
		os.str(std::string());
		os.clear();
		os << "3,2-3,2";
		server.wsSend(clientID, P1POSUPDATE + os.str() );
		//gameState.getBoard().setValue(3, 2, 3); //sets snake 1 position on board

		// SEND THE STARTING POSITION FOR PLAYER 2 
		os.str(std::string());
		os.clear();
		os << "5,5-5,5";
		server.wsSend(clientID, P2POSUPDATE + os.str());
		//gameState.getBoard().setValue(5, 5, 4); //sets snake 1 position on board

		// SEND THE STARTING FOOD POSITION
		os.str(std::string());
		os.clear();
		os << gameState.getFoodString();
		server.wsSend(clientID, SENDFOOD + os.str());
	}

	if (command == "resetgame") {
		server.ResetGame();
		server.wsSend(clientID, UPDATEP1SCORE + server.GetPlayerScore(0));
		server.wsSend(clientID, UPDATEP2SCORE + server.GetPlayerScore(1));
	}

	// Commands to process incoming messages.
	if (command == "setp1name") {

		// If they did not enter a name, make the default name "Player 1". Else, record the passed in value.
		if (os.str() == "") { 
			server.SetPlayerName(0, "Player 1"); 
		}
		else{ 
			server.SetPlayerName(0, os.str());
		}

		// To ensure the server is keeping track of the names, ask the server for 
		//the name of player one to print out to the console and send it to the client.
		std::cout << server.GetPlayerName(clientID, 0) << " has joined the game as Player 1." << std::endl;
		server.wsSend(clientID, PRINT + "Welcome " + server.GetPlayerName(clientID, 0) + " to the game.");
		server.wsSend(clientID, UPDATEP1SCORE + server.GetPlayerScore(0));
	}

	if (command == "setp2name") { 

		// If they did not enter a name, make the default name "Player 2". Else, record the passed in value.
		if (os.str() == "") {
			server.SetPlayerName(1, "Player 2");
		}
		else {
			server.SetPlayerName(1, os.str());
		}
		// To ensure the server is keeping track of the names, ask the server for 
		//the name of player 2 to print out to the console and send it to the client.
		std::cout << server.GetPlayerName(clientID, 1) << " has joined the game as Player 2." << std::endl;
		server.wsSend(clientID, PRINT + "Welcome " + server.GetPlayerName(clientID, 1) + " to the game.");
		server.wsSend(clientID, UPDATEP2SCORE + server.GetPlayerScore(1));
	}

	if (command == "getp1name") { 
		server.GetPlayerName(clientID, 0); 
	}

	if (command == "getp2name") { 
		server.GetPlayerName(clientID, 1); 
	}

	if (command == "p1scored") {
		server.UpdateScore(0);

		// THIS IS WHERE WE SEND A COMMAND BACK TO THE CLIENT
		server.wsSend(clientID, UPDATEP1SCORE + server.GetPlayerScore(0) );
		std::cout << "Player 1 scored. Total score is: " << server.GetPlayerScore(0) << std::endl;
	}

	if (command == "p2scored") {
		server.UpdateScore(1);
		server.wsSend(clientID, UPDATEP2SCORE + server.GetPlayerScore(1));
		std::cout << "Player 2 scored. Total score is: " << server.GetPlayerScore(1) << std::endl;
	}

	if (command == "setdir") {
		int dirNumber = stoi(os.str()); //Convert the number in string form to an int
		/*vector<int> clientIDs = server.getClientIDs();
		int i = 0;
		for (; i < clientIDs.size(); ++i) {
			if (clientIDs.at(i) == clientID) {
				break;
			}
		}*/
		if (clientID == 0) {
			gameState.setSnake1Dir(dirNumber);
		}
		if (clientID == 1) {
			gameState.setSnake2Dir(dirNumber);
		}
	}

	if (command == "p1posupdate") {
		server.wsSend(clientID, P1POSUPDATE + gameState.getSnake1().getPosString());
	}

	if (command == "p2posupdate") {
		server.wsSend(clientID, P1POSUPDATE + gameState.getSnake2().getPosString());
	}

	//std::cout << os.str() << std::endl << std::endl;

	//server.SetPlayerName(os.str());
	//server.GetPlayerNames(clientID);

    //os << "Stranger " << clientID << " says: " << message;

    //vector<int> clientIDs = server.getClientIDs();
    //for (int i = 0; i < clientIDs.size(); i++){
    //    if (clientIDs[i] != clientID)
    //        server.wsSend(clientIDs[i], os.str());
    //}
}

/* called once per select() loop */
void periodicHandler(){
    static time_t next = time(NULL) + 10;
    time_t current = time(NULL);
    if (current >= next){
        ostringstream os;
        string timestring = ctime(&current);
        timestring = timestring.substr(0, timestring.size() - 1);
        os << timestring;

        vector<int> clientIDs = server.getClientIDs();
        for (int i = 0; i < clientIDs.size(); i++)
            server.wsSend(clientIDs[i], os.str());

        next = time(NULL) + 10;
    }
}

int main(int argc, char *argv[]){
    int port;

    cout << "Starting on port 8000..." << endl << endl;
    //cin >> port;

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    //server.setPeriodicHandler(periodicHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(8000);

    return 1;
}
