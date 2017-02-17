var connectBtn = document.getElementById('cntBtn');
var startBtn = document.getElementById('startBtn');

connectBtn.addEventListener('click', function () {
    connect();
});


// Start of server
var Server;
var p1id;
var p2id;
var p1_score;
var p2_score;

//variables to switch screen to snake
var snakeGame = document.getElementById('snake');

//COMMANDS TO SEND TO THE SERVER
//PLAYERS
var SETP1NAME = "setp1name:";
var SETP2NAME = "setp2name:";
var GETP1NAME = "getp1name:";
var STARTGAME = "startgame:";
var SETNAME = "setname:";
//var GETP2NAME = "getp2name:";

//SCORES
var P1SCORED = "p1scored:";
var P2SCORED = "p2scored:";

//GAME STATE
var RESETGAME = "resetgame:";
var DIR = "setdir:"; //In this variable, you will add an int to be the direction, followed by a ':'

//This helper function allows you to easily make a change direction message
function makeDirMessage(directionInt) {
    return DIR + String(directionInt);
}

function log(text) {
    $log = $('#log');
    //Add text to log
    $log.append(($log.val() ? "\n" : '') + text);
    //Autoscroll
    $log[0].scrollTop = $log[0].scrollHeight - $log[0].clientHeight;
}


function send(text) {
    Server.send('message', text);
}

function connect() {
    p1id = document.getElementById('p1id').value; //get p1 id from text field
    //p2id = document.getElementById('p2id').value; //get p2 id from text field
    log('Connecting...');

    Server = new FancyWebSocket('ws://' + document.getElementById('ip').value + ':' + document.getElementById('port').value);

    $('#message').keypress(function (e) {
        if (e.keyCode == 13 && this.value) {
            log('You: ' + this.value);
            send(this.value);

            $(this).val('');
        }
    });

    //Let the user know we're connected
    Server.bind('open', function () {
        document.getElementById("cntBtn").disabled = true;
        log("Connected.");

        // Once a connection to the server has been established, send over the names
        // of the players.
        //send(SETP1NAME + p1id);
        //send(SETP2NAME + p2id);
        send(SETNAME + p1id);

        //log('Welcome to snakes ' + p1id + " and " + p2id + "!");
        log("Waiting for player 2...");

        var textInput = document.getElementById('log').value.split('\n');
        var lastValue;
        //get last value of log/textArea
        for (var i = 0; i < textInput.length; ++i) {
            if (i == textInput.length - 1) {
                console.log('Inside textArea: ' + textInput[i]);
                lastValue = textInput[i];
            }
        }
    });

    //OH NOES! Disconnection occurred.
    Server.bind('close', function (data) {
        document.getElementById("cntBtn").disabled = false;
        log("Disconnected.");
    });

    //Log any messages sent from server
    Server.bind('message', function (payload) {

        var cmdCutOff;
        for (var i = 0; i < payload.length; ++i){
            if (payload[i] == ':') {
                cmdCutOff = i; break;
            }
        }
        var command = payload.substring(0, cmdCutOff);
        command.toLowerCase();
        var message = payload.substring(cmdCutOff + 1);

        if (command == "print") {
            log(message);
        } else if (command == "ready") {
            console.log("starting game!" + " message: " + message);
            if (message == "2") {

                setTimeout(function () {
                    var page1 = document.getElementById('connectPage');
                    var page2 = document.getElementById('snakePage');

                    connectBtn.style.visibility = 'hidden';
                    startBtn.style.visibility = "visible";
                }, 300);
            }
        } else if (command == "begin") {
            var page1 = document.getElementById('connectPage');
            var page2 = document.getElementById('snakePage');

            page1.style.display = 'none';
            page2.style.display = 'block';
        }
        else if (command == "updateP1Score") {
            p1_score = parseInt(message);
        }
        else if (command == "updateP2Score") {
            p2_score = parseInt(message);
        }
        else if (command == "drawboard") {
            InitializeBoardArray(message);
            drawBoard();
        }
        else if (command == "resetboard") {
          
        }
        else if (command == "p1posupdate") {
            P1PosUpdate(message);
        }
        else if (command == "clearp1tail") {
            ClearP1Tail(message);
        }
        else if (command == "clearp2tail") {
            ClearP2Tail(message);
        }
        else if (command == "p2posupdate") {
            P2PosUpdate(message);
        }
        else if (command == "sendfood") {
            SetFood(message);
        }


    });

    Server.connect();
}
