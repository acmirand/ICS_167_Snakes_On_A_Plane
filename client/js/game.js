/*
Group Members:
Ney Congjuico     ncongjui@uci.edu    12432102
Zachary Hart      zhhart@uci.edu      70953123
Daniel Lara       larad@uci.edu       49651280
Avelino Miranda   acmirand@uci.edu    16732033
*/

var MITIGATION = true;
//var MITIGATION = false;

// Webpage variables
var canvas;
var context;
var img_snake_head1 = document.getElementById("snake_head1"); // States which snake image is attached to the game canvas.
var img_snake_head2 = document.getElementById("snake_head2");

// Array values and content
var board;
var wall = 1;
var free = 0;
var snake1_space = 2;
var snake2_space = 3;
var food_space = 4;
var boardInitialized = false; // Declared true once the array 'board' is filled

// Controls
var keystate;
var move_left = 37; // left-arrow key
var move_right = 39; // right-arrow key
var move_up = 38; // up-arrow key
var move_down = 40; // down-arrow key
var prevKey = -1;

// Snake Game
var p1_score = 0;
var p2_score = 0;
var rows = 20;
var columns = 20;
var lagValue = 0;

// OBJECTS / COMMANDS TO BE PROCESSED
var incomingSet = [];
var set = [];
function AddToIncomingSet(timeY) {
    incomingSet.push(timeY);
}

// Initializes the array 'board' to be rendered with the appropriate dimensions sent by the server
function InitializeBoardArray(dimensions) {
    if (!boardInitialized) {
        setInterval(sendTime, 500); // Sends Client's Time every half a second for NTP calculations
        var cmdCutOff;
        for (var i = 0; i < dimensions.length; ++i) {
            if (dimensions[i] == 'x') {
                cmdCutOff = i; break;
            }
        }

        // Rows and Columns sent by server
        rows = parseInt(dimensions.substring(0, cmdCutOff));
        columns = parseInt(dimensions.substring(cmdCutOff + 1));

        board = [];

        // Populates the empty array 'board'
        for (var x = 0; x < columns; x++) {
            board.push([]);
            for (var y = 0; y < rows; y++) {
                if (x === 0 || x === rows - 1) {
                    board[x].push(wall);
                }
                else if (y === 0 || y === columns - 1) {
                    board[x].push(wall);
                }
                else {
                    board[x].push(free);
                }
            }
        }
        canvas = document.createElement("canvas");
        canvas.width = columns * columns;
        canvas.height = rows * rows;
        canvas.style = "position:absolute; left: 50%; width: 400px; margin-left: -200px;";
        context = canvas.getContext("2d");
        context.font = "20px Times New Roman";
        document.body.appendChild(canvas);
        boardInitialized = true;
    }

    // Enters this else statement if game restarts
    else {
        ClearBoard(); // Clears array 'board' of any previous player positions
        p1_score = 0; // Resets each player's score
        p2_score = 0;
        prevKey = -1; // Resets prevKey for re-checking
    }
}

function ClearBoard() {
    for (var x = 0; x < columns; x++) {
        for (var y = 0; y < rows; y++) {
            if (x === 0 || x === rows - 1) {
                board[x][y] = wall;
            }
            else if (y === 0 || y === columns - 1) {
                board[x][y] = wall;
            }
            else {
                board[x][y] = free;
            }
        }
    }
}

function drawBoard() {
    getInput(); // Checks for any client input
    document.getElementById("realTime").value = "Latency: " + lagValue + " ms"; // Displays latency

    var width = rows;
    var height = columns;
    var snake1_pat = context.createPattern(snake_head1, "repeat"); // Needs to have repeat for the image
    var snake2_pat = context.createPattern(snake_head2, "repeat");
    var breadstick_pat = context.createPattern(breadstick, "repeat"); // Food of the snake

    for (var x = 0; x < width; x++) { // Used to render what is found in the array 'board'
        for (var y = 0; y < height; y++) {
            switch (board[x][y]) {
                case free:
                    context.fillStyle = "#fff"; // White Space
                    break;
                case snake1_space:
                    context.fillStyle = snake1_pat;
                    break;
                case snake2_space:
                    context.fillStyle = snake2_pat;
                    break;
                case food_space:
                    context.fillStyle = breadstick_pat;
                    break;
                case wall:
                    context.fillStyle = "#000"; // Black Space
                    break;
            }
            context.fillRect(x * width, y * height, width, height); // Fills the canvas rectangle style
        }
    }
    context.fillStyle = "000";
    context.fillText(p1id + " Score: " + p1_score, 20, canvas.height - 21); // Displays Score for each player
    context.fillText(p2id + " Score: " + p2_score, canvas.width - 120, canvas.height - 21);

    window.requestAnimationFrame(drawBoard, canvas); // Renders the Canvas every frame
}

// Server sends a X and Y index , populates the array 'board' with a food space
function SetFood(pos) {
    var cmdCutOff;
    for (var i = 0; i < pos.length; ++i) {
        if (pos[i] == ',') {
            cmdCutOff = i; break;
        }
    }
    var xFood = pos.substring(0, cmdCutOff);
    var yFood = pos.substring(cmdCutOff + 1);
    board[xFood][yFood] = food_space;
}

// BEGINNING OF P1 RENDER FUNCTIONS
function P1PosUpdate(pos) { // Client processes server message of P1 indices
    var cmdCutOff;
    for (var i = 0; i < pos.length; ++i) {
        if (pos[i] == '-') {
            cmdCutOff = i; break;
        }
    }
    var HeadPos = pos.substring(0, cmdCutOff);
    for (var i = 0; i < HeadPos.length; ++i) {
        if (HeadPos[i] == ',') {
            cmdCutOff = i; break;
        }
    }
    var xHead = parseInt(HeadPos.substring(0, cmdCutOff));
    var yHead = parseInt(HeadPos.substring(cmdCutOff + 1));
    board[xHead][yHead] = snake1_space; // Client populates the array 'board' to be rendered
}
function ClearP1Tail(pos) {
    var cmdCutOff;
    for (var i = 0; i < pos.length; ++i) {
        if (pos[i] == ',') {
            cmdCutOff = i; break;
        }
    }
    var x = parseInt(pos.substring(0, cmdCutOff));
    var y = parseInt(pos.substring(cmdCutOff + 1));
    board[x][y] = free; // Client clears up the trail end of snake and renders it as a free space
}
function UpdateP1Score(score) {
    p1_score = parseInt(score);
}
// END OF P1 RENDER FUNCTIONS

// BEGINNING OF P2 RENDER FUNCTIONS *Similar functionality as P1 instead populates P2 space*
function P2PosUpdate(pos) {
    var cmdCutOff;
    for (var i = 0; i < pos.length; ++i) {
        if (pos[i] == '-') {
            cmdCutOff = i; break;
        }
    }
    var HeadPos = pos.substring(0, cmdCutOff);
    for (var i = 0; i < HeadPos.length; ++i) {
        if (HeadPos[i] == ',') {
            cmdCutOff = i; break;
        }
    }
    var xHead = parseInt(HeadPos.substring(0, cmdCutOff));
    var yHead = parseInt(HeadPos.substring(cmdCutOff + 1));
    board[xHead][yHead] = snake2_space;
}
function ClearP2Tail(pos) {
    var cmdCutOff;
    for (var i = 0; i < pos.length; ++i) {
        if (pos[i] == ',') {
            cmdCutOff = i; break;
        }
    }
    var x = parseInt(pos.substring(0, cmdCutOff));
    var y = parseInt(pos.substring(cmdCutOff + 1));
    board[x][y] = free;
}
function UpdateP2Score(score) {
    p2_score = parseInt(score);
}
// END OF P2 RENDER FUNCTIONS

function getInput() {
    if (keystate[move_left]) {
        if (prevKey != move_left) { // This if nest is to check for previous input, prevents snake into eating oneself
            prevKey = move_left;
            send("setdir:2," + Date.now().toString()); // Client sends move command for Server to process
        }
    }
    if (keystate[move_right]) {
        if (prevKey != move_right) {
            prevKey = move_right;
            send("setdir:3," + Date.now().toString());
        }
    }
    if (keystate[move_up]) {
        if (prevKey != move_up) {
            prevKey = move_up;
            send("setdir:0," + Date.now().toString());
        }
    }
    if (keystate[move_down]) {
        if (prevKey != move_down) {
            prevKey = move_down;
            send("setdir:1," + Date.now().toString());
        }
    }
}

function BucketProcessing() {
    if (incomingSet.length > 0) {
        set = incomingSet.slice();
        incomingSet = [];
    }

    for (var i = 0; i < set.length; ++i) {
        var cmdStruct = set[i];

        // Processes the cmd struct constructed from Server and finds the "func"(function) associated
        switch (cmdStruct["func"]) {
            // Calls respective function and passes the "str" input for rendering purposes
            case "P1PosUpdate":
                P1PosUpdate(cmdStruct["str"]); break; 
            case "P2PosUpdate":
                P2PosUpdate(cmdStruct["str"]); break;
            case "UpdateP1Score":
                UpdateP1Score(cmdStruct["str"]); break;
            case "UpdateP2Score":
                UpdateP2Score(cmdStruct["str"]); break;
            case "SetFood":
                SetFood(cmdStruct["str"]); break;
            case "ClearP1Tail":
                ClearP1Tail(cmdStruct["str"]); break;
            case "ClearP2Tail":
                ClearP2Tail(cmdStruct["str"]); break;
        }
    }
    set = []; // Clears the bucket
}

function sendTime() {
    send("clienttime:" + Date.now().toString());
}

// Called when client receives server command, calculates latency
function calculateServerTime(timeB, message) {
    var clientTimeIndex;
    var serverTime1Index;
    for (var i = 0; i < message.length; ++i) {
        if (message[i] == ',') {
            clientTimeIndex = i; break;
        }
    }
    for (var i = clientTimeIndex + 1; i < message.length; ++i) {
        if (message[i] == ',') {
            serverTime1Index = i; break;
        }
    }
    var timeA = parseInt(message.substring(0, clientTimeIndex)); // Timestamp of client sent message to server
    var timeX = parseInt(message.substring(clientTimeIndex + 1, serverTime1Index)); // Timestamp of server received client message
    var timeY = parseInt(message.substring(serverTime1Index + 1)); // Timestamp of server processed client message and sent back to client
    var realTime = timeY + (((timeB - timeA) - (timeY - timeX)) / 2); // Calculation of real time
    lagValue = realTime - timeA; // Latency calculated to be displayed
}

function main() {

    if (MITIGATION) {
        setInterval(BucketProcessing, 125); // Simulates Buckets, queues up commands to be processed every 1/8ths of a second
    }

    // Automatically inputs values into text fields *Quick-Testing Purposes*
    document.getElementById("ip").value = "127.0.0.1";
    document.getElementById("port").value = "8000";

    startBtn.addEventListener('click', function () {
        send("startgame:"); // Sends to server to start the game
    });

    // Used for key-pressed detection
    keystate = {};
    document.addEventListener("keydown", function (evt) {
        keystate[evt.keyCode] = true;
    });
    document.addEventListener("keyup", function (evt) {
        delete keystate[evt.keyCode];
    });
}

main();