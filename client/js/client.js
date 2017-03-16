/*
Group Members:
Ney Congjuico     ncongjui@uci.edu    12432102
Zachary Hart      zhhart@uci.edu      70953123
Daniel Lara       larad@uci.edu       49651280
Avelino Miranda   acmirand@uci.edu    16732033
*/

var connectBtn = document.getElementById('cntBtn');
var startBtn = document.getElementById('startBtn');
connectBtn.addEventListener('click', function () {
    connect();
});

// Variables declaring server/client informations
var Server;
var playerID;
var p1id;
var p2id;
var p1_score;
var p2_score;

// Variable to activate snake canvas
var snakeGame = document.getElementById('snake');

// BEGINNING LIST OF COMMANDS TO SEND TO THE SERVER

// PLAYERS
var SETNAME = "setname:";
// GAME STATE
var RESETGAME = "resetgame:";
var STARTGAME = "startgame:";
var DIR = "setdir:"; // In this variable, you will add an int to be the direction, followed by a ':'
// TIME COMMANDS
var CLIENTTIME = "clienttime:";

// END OF COMMANDS

// This helper function allows you to easily make a change direction message
function makeDirMessage(directionInt) {
    return DIR + directionInt.toString();
}

// Displays text to connection canvas
function log(text) {
    $log = $('#log');
    // Add text to log
    $log.append(($log.val() ? "\n" : '') + text);
    // Autoscroll
    $log[0].scrollTop = $log[0].scrollHeight - $log[0].clientHeight;
}

// Sends specific commands(clienttime/directions/startgame) to server to process
function send(text) {
    Server.send('message', text);
}

function connect() {
    playerID = document.getElementById('p1id').value; // Accesses the HTML textfield p1id which contains the player's chosen name or default
    log('Connecting...');
    Server = new FancyWebSocket('ws://' + document.getElementById('ip').value + ':' + document.getElementById('port').value); // Establishes server connection
    $('#message').keypress(function (e) {
        if (e.keyCode == 13 && this.value) {
            log('You: ' + this.value);
            send(this.value);
            $(this).val('');
        }
    });

    // Let the user know they are connected to the server
    Server.bind('open', function () {
        document.getElementById("cntBtn").disabled = true;
        log("Connected.");

        // Once a connection to the server has been established server receives the name of the player.
        send(SETNAME + playerID);

        // Displayed when player established connection.
        log("Waiting for Player 2...");
    });

    // OH NOES! Disconnection occurred.
    Server.bind('close', function (data) {
        document.getElementById("cntBtn").disabled = false;
        log("Disconnected.");
    });

    // Log any messages sent from server
    Server.bind('message', function (payload) {

        var cmdCutOff;
        for (var i = 0; i < payload.length; ++i) {
            if (payload[i] == ':') {
                cmdCutOff = i; break;
            }
        }
        var command = payload.substring(0, cmdCutOff);
        command.toLowerCase();
        var message = payload.substring(cmdCutOff + 1);

        var timeIndex = -1;
        for (var i = 0; i < message.length; ++i){
            if (message[i] == '@') {
                timeIndex = i; break;
            }
        }

        // IF THE COMMAND IS ONE OF THE FOLLOWING, P1POSUPDATE, P2POSUPDATE,
        // UPDATEP1SCORE, UPDATEP2SCORE, SENDFOOD, CLEARP1TAIL, OR CLEARP2TAIL,
        // MAKE SURE TO GET THE TIMEY SO CORRECT ORDER IS PROCESSED.
        if (timeIndex != -1) {
            // THIS IS IMPORTANT INFORMATION!!!
            var str = message;
            message = message.substring(0, timeIndex);
        }
        if (command == "print") { // Displays Player 2 has connected
            log(message);
        }
        else if (command == "ready") {
            if (message == "2") { // Server sends back that there are now 2 connections established

                setTimeout(function () {
                    connectBtn.style.visibility = 'hidden'; // Hides connection button
                    startBtn.style.visibility = "visible"; // Activates start button
                }, 300);
            }
        }
        else if (command == "begin") {
            var cntCanvas = document.getElementById('connectPage');
            var snkCanvas = document.getElementById('snakePage');

            cntCanvas.style.display = 'none'; // Connection canvas is hidden from display
            snkCanvas.style.display = 'block'; // Game canvas is displayed
        }

        // TIME DEPENDENT COMMANDS
        // Explanation of cmdStruct
        // str is input to the function
        // timeB is the current time the client receives server command
        // func is the function called client side to render server information
        // IncomingSet is the 'bucket' in which server commands are put in to be processed
        else if (command == "p1posupdate") {
            var cmdStruct = { str: message, timeB: Date.now(), func: "P1PosUpdate" };
            AddToIncomingSet(cmdStruct);
        }
        else if (command == "p2posupdate") {
            var cmdStruct = { str: message, timeB: Date.now(), func: "P2PosUpdate" };
            AddToIncomingSet(cmdStruct);
        }
        else if (command == "updateP1Score") {
            var cmdStruct = { str: message, timeB: Date.now(), func: "UpdateP1Score" };
            AddToIncomingSet(cmdStruct);
        }
        else if (command == "updateP2Score") {
            var cmdStruct = { str: message, timeB: Date.now(), func: "UpdateP2Score" };
            AddToIncomingSet(cmdStruct);
        }
        else if (command == "sendfood") {
            var cmdStruct = { str: message, timeB: Date.now(), func: "SetFood" };
            AddToIncomingSet(cmdStruct);
        }
        else if (command == "clearp1tail") {
            var cmdStruct = { str: message, timeB: Date.now(), func: "ClearP1Tail" };
            AddToIncomingSet(cmdStruct);
        }
        else if (command == "clearp2tail") {
            var cmdStruct = { str: message, timeB: Date.now(), func: "ClearP2Tail" };
            AddToIncomingSet(cmdStruct);
        }

        // TIME INDEPENDENT COMMANDS
        // Called when game starts/restarts
        else if (command == "drawboard") {
            InitializeBoardArray(message);
            drawBoard();
        }
        else if (command == "player1name") {
            p1id = message;
        }
        else if (command == "player2name") {
            p2id = message;
        }

        // Called everytime client receives a command from server.
        else if (command == "servertime") {
            calculateServerTime(Date.now().toString(), message);
        }
    });

    Server.connect();
}
