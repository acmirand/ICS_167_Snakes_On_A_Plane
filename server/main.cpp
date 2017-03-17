/*
Group Members:
Ney Congjuico     ncongjui@uci.edu    12432102
Zachary Hart      zhhart@uci.edu      70953123
Daniel Lara       larad@uci.edu       49651280
Avelino Miranda   acmirand@uci.edu    16732033
*/

#define _CRT_SECURE_NO_WARNINGS

#include "snake.cpp"

using namespace std;
using namespace chrono;

webSocket server;
GameState gameState(&server);
bool gameInSession = false;

milliseconds timer = duration_cast< milliseconds >(system_clock::now().time_since_epoch());

struct MessageEntry {
	int clientID; // Specifies clientID
	std::string command; // The command
	std::string message; // The string associated with the command
	milliseconds timeReceived; // The time the message came in to the server
	milliseconds timeX; // Time server receives message
	milliseconds delay; // Delay added to incoming messages
	milliseconds timeA; // Timestamp when client sent message to server
	milliseconds outgoingDelay; // Random artificial delay associated to outgoing delays, to be used in periodicHandler
	milliseconds timeY; // Time server processes and sends message to client
};

// Declare these functions to be used later
milliseconds randomNum();
void ProcessRequest(int clientID, std::string command, std::string message, milliseconds timeX, milliseconds timeA, milliseconds timeY);
std::default_random_engine generator;

// Set global variables to allow access in multiple functions
// This is our queue. there are many like it, but this one is ours.
queue<MessageEntry> requestQueue;
queue<MessageEntry> outgoingQueue;
set<milliseconds> timeASet;
std::map<milliseconds, MessageEntry> buffer;

// Called when a client connects
// Dictates when a player joins/game is ready to start
void openHandler(int clientID) {
	ostringstream os;
	os << "Player 2 has joined.";
	vector<int> clientIDs = server.getClientIDs();
	for (int i = 0; i < clientIDs.size(); i++) {
		if (clientIDs[i] != clientID)
			server.wsSend(clientIDs[i], "print:" + os.str());
		if (server.numOfActiveConnections == 2) {
			server.wsSend(clientIDs[i], "ready:2");
		}
	}
}

// Called when a client disconnects
void closeHandler(int clientID) {
	ostringstream os;
	os << "Stranger " << clientID << " has left.";
	vector<int> clientIDs = server.getClientIDs();
	for (int i = 0; i < clientIDs.size(); i++) {
		if (clientIDs[i] != clientID)
			server.wsSend(clientIDs[i], os.str());
	}
	gameInSession = false;
}


/*
Called when server receives the command message from client
1. One message comes in, put into queue
2. While queue is not empty, process requests inside queue
3. Put slight delay on serving requests
*/
void messageHandler(int clientID, string message) {
	ostringstream os;
	int cmdCutOff = 0;
	std::chrono::milliseconds received = duration_cast< milliseconds >(system_clock::now().time_since_epoch()); // TimeX
	std::string command;
	for (int i = 0; message.length(); ++i) {
		if (message[i] == ':') {
			cmdCutOff = i;
			break;
		}
	}

	// Parse the info for processing.
	os << message.substr(cmdCutOff + 1); // Splices message input to be used in command
	command = message.substr(0, cmdCutOff); // Splices message til command
	std::transform(command.begin(), command.end(), command.begin(), ::tolower); // Converts all uppercase letters to lowercase in string command

	// Example message- setdir:1,1234567890
	if (command == "setdir") {
		std::string remainingCommand = os.str();
		for (int i = 0; i < remainingCommand.length(); ++i) {
			if (remainingCommand[i] == ',') {
				cmdCutOff = i;
				break;
			}
		}

		// Sets the delay
		milliseconds delay = randomNum();

		// Creates the inputs for set and buffer
		std::string::size_type sz = 0;
		milliseconds timeA(std::stoll(remainingCommand.substr(cmdCutOff + 1), &sz, 0));
		MessageEntry toInsert = MessageEntry{ clientID, command, os.str().substr(0,cmdCutOff), received, received + delay, delay, timeA, randomNum(), received}; // Delay for timeY is done in periodicHandler

		// Set is used for ordering reasons, Buffer to keep track of all commands to be processed
		timeASet.insert(timeA);
		buffer.insert(std::pair<milliseconds, MessageEntry>(timeA, toInsert));
	}

	// Example message- clienttime:1234567890
	if (command == "clienttime") {
		milliseconds delay = randomNum();

		std::string::size_type sz = 0;
		milliseconds timeA(std::stoll(os.str(), &sz, 0));
		MessageEntry toInsert = MessageEntry{ clientID, command, os.str(), received, received + delay, delay, timeA, randomNum(), received }; // Delay for timeY is done in periodicHandler

		timeASet.insert(timeA);
		buffer.insert(std::pair<milliseconds, MessageEntry>(timeA, toInsert));
	}

	// Example message- startgame:
	if (command == "startgame") {
		vector<int> clientIDs = server.getClientIDs();

		// Server sends a "begin:" message to each client
		// received by client.js
		for (int i = 0; i < clientIDs.size(); i++) {
			server.wsSend(clientIDs[i], "begin:");
		}
		
		timer = duration_cast< milliseconds >(system_clock::now().time_since_epoch()); // Resets timer
		
		// Tells gameState (in charged of gamelogic) to start the game
		gameState.SetClientIDs(server.getClientIDs());
		gameState.Init();
		gameInSession = true;
	}

	// Example message- setname:Breadstick
	if (command == "setname") {

		// If user does not enter a name, make the default name "Player #". Else, record the passed in value.
		if (os.str() == "") {
			gameState.SetPlayerName(clientID, "P" + std::to_string(clientID + 1));
		}
		else {
			gameState.SetPlayerName(clientID, os.str());
		}
	}
}

// Uses uniform random distribution
milliseconds randomNum() {
	std::uniform_int_distribution<int> distribution(0, 250);
	milliseconds returnValue(distribution(generator));
	return returnValue;
}

milliseconds threshold(125); // Bucket is every 1/8ths of a second
milliseconds nextFrame = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
milliseconds offset(500); 

/* called once per select() loop */
void periodicHandler() {

	milliseconds offset(500);
	milliseconds current = duration_cast< milliseconds >(system_clock::now().time_since_epoch());

	// Enters 'if statement' once bucket threshold is hit
	if (current - timer >= threshold) {
		if (!timeASet.empty()) {
			for (auto i = timeASet.begin(); i != timeASet.end();) {
				if (current - buffer[*i].timeX < threshold) { // Using set as index, processes orders accordingly that fall within the bucket

					// Adds to queue to be processed.
					requestQueue.push(buffer[*i]);

					// Clears up the buffer and set
					buffer.erase(*i); 
					i = timeASet.erase(i);
				}
				else {
					++i;
				}
			}
		}
		
		// Resets timer
		timer = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	}

	// Enters this 'if statement' to update the gameState and populates outgoingQueue
	if (current >= nextFrame && gameInSession) {
		
		// Keep going through the queue and determine if it should be processed or not
		// This is to simulate latency
		while (!requestQueue.empty()) {
			MessageEntry message = { requestQueue.front().clientID, requestQueue.front().command, requestQueue.front().message,requestQueue.front().timeReceived, requestQueue.front().timeX, requestQueue.front().delay, requestQueue.front().timeA , requestQueue.front().outgoingDelay , requestQueue.front().timeY };
			message.timeY = duration_cast< milliseconds >(system_clock::now().time_since_epoch()) + message.outgoingDelay; // This is where the delay for timeY is added

			// Removes element from queue, reducing its size
			requestQueue.pop();
			
			// Passes message to outgoingQueue to be processed below
			outgoingQueue.push(message);
		}

		// This is where the outgoingQueue (with the added delay) is handled
		while (!outgoingQueue.empty()) {
			MessageEntry message = { outgoingQueue.front().clientID, outgoingQueue.front().command, outgoingQueue.front().message, outgoingQueue.front().timeReceived, outgoingQueue.front().timeX, outgoingQueue.front().delay, outgoingQueue.front().timeA , outgoingQueue.front().outgoingDelay , outgoingQueue.front().timeY };
			outgoingQueue.pop();

			// If given outgoingDelay of message is less than current time process message
			if (duration_cast< milliseconds >(system_clock::now().time_since_epoch()) - message.timeY >= message.outgoingDelay) {

				// Processes client inputs (updates gameState per client command)
				ProcessRequest(message.clientID, message.command, message.message, message.timeReceived, message.timeA, message.timeY);
			}

			// Else put back message into outgoingQueue
			else {
				outgoingQueue.push(message);
			}
		}

		// Updates the game state and sends the states to all clients
		gameState.UpdateLoop();

		// Resets nextFrame
		nextFrame = duration_cast< milliseconds >(system_clock::now().time_since_epoch()) + offset;
	}
}

void ProcessRequest(int clientID, std::string command, std::string message, milliseconds timeX, milliseconds timeA, milliseconds timeY) {
	if (command == "setdir") {
		int dirNumber = stoi(message); // Convert the number in string form to an int
		if (clientID == 0) {
			gameState.setSnake1Dir(dirNumber); // Updates direction respective of clientID
		}
		if (clientID == 1) {
			gameState.setSnake2Dir(dirNumber);
		}
	}
	if (command == "clienttime") {
		std::string clientAtime = message;
		server.wsSend(clientID, "servertime:" + clientAtime + "," + std::to_string(timeX.count()) + "," +
			std::to_string(timeY.count())); // Sends command- servertime: to client which calculates latency
	}
}

int main(int argc, char *argv[]) {
	cout << "Starting on port 8000..." << endl << endl;

	server.setOpenHandler(openHandler);
	server.setCloseHandler(closeHandler);
	server.setMessageHandler(messageHandler);
	server.setPeriodicHandler(periodicHandler);

	server.startServer(8000);

	return 1;
}
