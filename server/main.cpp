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

struct MessageEntry {
	int clientID;
	std::string command;
	std::string message;
	long long timeReceived;
	long long timeToBeProcessed;
	long long delay;
};

// Declare these functions to be used later
long long randomNum();
void ProcessRequest(int clientID, std::string command, std::string message, long long timeReceived);
std::default_random_engine generator;

// this is our queue. there are many like it, but this mine.
queue<MessageEntry> requestQueue;

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

	ostringstream os;
	int cmdCutOff = 0;

	/*
		1. One message comes in, put into queue
		2. While queue is not empty, process requests inside queue
		2. Put slight delay on serving requests
	*/

	std::chrono::milliseconds received = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

		std::string command;
		for (int i = 0; message.length(); ++i) {
			if (message[i] == 58) {
				cmdCutOff = i;
				break;
			}
		}

	// Parse the info for processing.
	//os << request.substr(cmdCutOff + 1);
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
		gameInSession = true;
	}
	else if (command == "setname") {

		if (os.str() == "") {
			// If they did not enter a name, make the default name "Player #". Else, record the passed in value.
			gameState.SetPlayerName(clientID, "P" + std::to_string(clientID + 1));
		}
		else {
			gameState.SetPlayerName(clientID, os.str());
		}
	}
	else {
		long long delay = randomNum();
		requestQueue.push(MessageEntry{ clientID, command, os.str(), received.count(), received.count() + delay, delay });
	}
}

long long randomNum() {
	std::uniform_int_distribution<long long> distribution(0, 500);

	//std::cout << distribution(generator);
	return distribution(generator);
	//return rand() % (max - min + 1) + min;
}

int timerz = 0;

/* called once per select() loop */
void periodicHandler() {

	time_t offset = 1; //randomNum();
	static time_t next = time(0) + offset;
	time_t current = time(0);
	if (current >= next && gameInSession) {

		ostringstream os;
		string timestring = ctime(&current);
		timestring = timestring.substr(0, timestring.size() - 1);
		os << timestring;

		// Keep going through the queue and determine if it should be processed or not.
		// This is to simulate latency.
		if (!requestQueue.empty()) {

			MessageEntry message = { requestQueue.front().clientID, requestQueue.front().command,requestQueue.front().message,requestQueue.front().timeReceived, requestQueue.front().timeToBeProcessed, requestQueue.front().delay };

			//std::cout << message.clientID << " " << message.command << " " << message.message << " " << message.timeReceived << std::endl;

			requestQueue.pop();
			if ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - message.timeToBeProcessed >=  message.delay)) {
				std::cout << message.timeReceived << " and the delay is = " << message.delay << std::endl;
				ProcessRequest(message.clientID, message.command, message.message, message.timeReceived);
			}
			else {
				std::cout << message.timeReceived  << "Re-Queued" << std::endl;
				requestQueue.push(message);
			}
		}

		//std::this_thread::sleep_for(std::chrono::milliseconds(randomInt(100,700)));
		gameState.UpdateLoop();

		vector<int> clientIDs = server.getClientIDs();
		for (int i = 0; i < clientIDs.size(); i++)
			server.wsSend(clientIDs[i], os.str());

		next = time(0) + offset;
	}
}

void ProcessRequest(int clientID, std::string command, std::string message, long long timeReceived) {

	if (command == "setdir") {

		int dirNumber = stoi(message); //Convert the number in string form to an int

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
		std::string clientAtime = message;
		//Send back the following
		//  Client's A time
		//	Time that clienttime was received
		//	time the server sends servertime
		//std::cout << received.count() << std::endl;
		//std::cout << std::to_string(received.count()) << std::endl;
		server.wsSend(clientID, "servertime:" + clientAtime + "," + std::to_string(timeReceived) + "," +
			std::to_string((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())));
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
