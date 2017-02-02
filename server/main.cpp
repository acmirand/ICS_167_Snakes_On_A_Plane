#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <algorithm>
#include "websocket.h"

using namespace std;

webSocket server;

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
		server.wsSend(clientID, "Welcome " + server.GetPlayerName(clientID, 0) + " to the game.");

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
		server.wsSend(clientID, "Welcome " + server.GetPlayerName(clientID, 1) + " to the game.");
	}

	if (command == "getp1name") { 
		server.GetPlayerName(clientID, 0); 
	}

	if (command == "getp2name") { 
		server.GetPlayerName(clientID, 1); 
	}

	if (command == "p1scored") {

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

    cout << "Please set server port: ";
    cin >> port;

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    //server.setPeriodicHandler(periodicHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(port);

    return 1;
}
