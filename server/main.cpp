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

webSocket server;
GameState gameState(&server);
bool gameInSession = false;
time_t gameStart;


/* called when a client connects */
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

/* called when a client disconnects */
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

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message) {
	queue<std::string> requestQueue;
	ostringstream os;
	int cmdCutOff = 0;

	/*
		1. One message comes in, put into queue
		2. While queue is not empty, process requests inside queue
		2. Put slight delay on serving requests
	*/
	//time_t received = time(0);
	std::chrono::milliseconds received = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	requestQueue.push(message);

	while (requestQueue.size() != 0) {
		// Parse the string until it reaches a ":". This will mark the command being passed in.
		// Everything after that point will be consider data to be dealt with.
		std::string request = requestQueue.front();
		requestQueue.pop();

		std::string command;
		for (int i = 0; message.length(); ++i) {
			if (message[i] == 58) {
				cmdCutOff = i;
				break;
			}
		}

		// Parse the info for processing.
		os << request.substr(cmdCutOff + 1);
		command = request.substr(0, cmdCutOff);
		std::transform(command.begin(), command.end(), command.begin(), ::tolower);

		if (command == "startgame") {
			vector<int> clientIDs = server.getClientIDs();
			for (int i = 0; i < clientIDs.size(); i++) {
				server.wsSend(clientIDs[i], "begin:");
			}

			gameState.SetClientIDs(server.getClientIDs());
			gameState.Init();
			gameInSession = true;
		}

		if (command == "setname") {

			if (os.str() == "") {
				// If they did not enter a name, make the default name "Player #". Else, record the passed in value.
				gameState.SetPlayerName(clientID, "P" + std::to_string(clientID + 1));
			}
			else {
				gameState.SetPlayerName(clientID, os.str());
			}
		}


		if (command == "setdir") {

			int dirNumber = stoi(os.str()); //Convert the number in string form to an int

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

		if (command == "clienttime") {
			std::string clientAtime = os.str();
			//Send back the following
			//  Client's A time
			//	Time that clienttime was received
			//	time the server sends servertime
			//std::cout << received.count() << std::endl;
			//std::cout << std::to_string(received.count()) << std::endl;
			server.wsSend(clientID, "servertime:" + clientAtime + "," + std::to_string(received.count()) + "," + 
				std::to_string((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())));
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
}

double randomNum() {
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(0,2.0);
	return distribution(generator);
	//return rand() % (max - min + 1) + min;
}

/* called once per select() loop */
void periodicHandler() {
	double offset = randomNum();
	static time_t next = time(0) + offset;
	time_t current = time(0);
	if (current >= next && gameInSession) {
		ostringstream os;
		string timestring = ctime(&current);
		timestring = timestring.substr(0, timestring.size() - 1);
		os << timestring;

		//std::this_thread::sleep_for(std::chrono::milliseconds(randomInt(100,700)));
		gameState.UpdateLoop();

		vector<int> clientIDs = server.getClientIDs();
		for (int i = 0; i < clientIDs.size(); i++)
			server.wsSend(clientIDs[i], os.str());

		next = time(0) + offset;
	}
}

int main(int argc, char *argv[]) {
	int port;

	cout << "Starting on port 8000..." << endl << endl;

	server.setOpenHandler(openHandler);
	server.setCloseHandler(closeHandler);
	server.setMessageHandler(messageHandler);
	server.setPeriodicHandler(periodicHandler);

	server.startServer(8000);

	return 1;
}
