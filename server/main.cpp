#define _CRT_SECURE_NO_WARNINGS

//#include <stdlib.h>
//#include <iostream>
//#include <string>
//#include <sstream>
//#include <time.h>
#include <algorithm>
#include <thread>
#include "snake.cpp"

using namespace std;

webSocket server;
GameState gameState(&server);
std::thread first;
bool playing = false;

void CallInit() {
	gameState.Init();
}

/* called when a client connects */
void openHandler(int clientID){
    ostringstream os;
    os << "Player 2 has joined.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], "print:" + os.str());

		if (server.numOfActiveConnections == 2) {
			server.wsSend(clientIDs[i], "ready:2");
		}
    }
}

/* called when a client disconnects */
void closeHandler(int clientID){
    ostringstream os;
    os << "Player " << clientID << " has left.";
	//std::cout << "No. of Connections: " <<server.numOfActiveConnections << std::endl;
	playing = false;
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
		vector<int> clientIDs = server.getClientIDs();
		for (int i = 0; i < clientIDs.size(); i++) {
			server.wsSend(clientIDs[i], "begin:");
		}

		gameState.SetClientIDs(server.getClientIDs());
		gameState.Init();
		if (clientIDs.size() == 2) {
			playing = true;
		}
	}

	if (command == "setname") {
		if (os.str() == "") {
			// If they did not enter a name, make the default name "Player #". Else, record the passed in value.
			server.SetPlayerName(clientID, "Player " + clientID+1);
		}
		else {
			server.SetPlayerName(clientID, os.str());
		}
	}

	if (command == "getp1name") { 
		server.GetPlayerName(clientID, 0); 
	}

	if (command == "getp2name") { 
		server.GetPlayerName(clientID, 1); 
	}

	//if (command == "p1scored") {
	//	server.UpdateScore(0);

	//	// THIS IS WHERE WE SEND A COMMAND BACK TO THE CLIENT
	//	server.wsSend(clientID, UPDATEP1SCORE + server.GetPlayerScore(0) );
	//	std::cout << "Player 1 scored. Total score is: " << server.GetPlayerScore(0) << std::endl;
	//}

	//if (command == "p2scored") {
	//	server.UpdateScore(1);
	//	server.wsSend(clientID, UPDATEP2SCORE + server.GetPlayerScore(1));
	//	std::cout << "Player 2 scored. Total score is: " << server.GetPlayerScore(1) << std::endl;
	//}

	if (command == "setdir") {
		

		int dirNumber = stoi(os.str()); //Convert the number in string form to an int
		std::cout << "clientID: " << clientID << " dirNumber: " << dirNumber << std::endl;
		if (clientID == 0) {
			gameState.setSnake1Dir(dirNumber);
		}
		if (clientID == 1) {
			gameState.setSnake2Dir(dirNumber);
		}
	}

	if (command == "p1posupdate") {
		server.wsSend(clientID, "p1posupdate:" + gameState.getSnake1().getPosString());
	}

	if (command == "p2posupdate") {
		server.wsSend(clientID, "p2posupdate:" + gameState.getSnake2().getPosString());
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
	if (playing) {
		gameState.UpdateLoop();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		if (gameState.isGameOver()) {
			playing = false;
		}
	}
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

	server.setOpenHandler(openHandler);
	server.setCloseHandler(closeHandler);
	server.setMessageHandler(messageHandler);

	server.startServer(8000);

    return 1;
}
