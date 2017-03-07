/*
Group Members:
Ney Congjuico     ncongjui@uci.edu    12432102
Zachary Hart      zhhart@uci.edu      70953123
Daniel Lara       larad@uci.edu       49651280
Avelino Miranda   acmirand@uci.edu    16732033
*/

var numPlayers = 0;

var canvas;
var context;

// Array values and content
var boardName;
var board;
var wall = 1;
var free = 0;
var snake1_space = 2;
var snake2_space = 3;
var food_space = 4;
var boardInitialized = false;

//Controls
var keystate;
var move_left = 37; //left-key
var move_right = 39; //right-key
var move_up = 38; //up-key
var move_down = 40; //down-key

//Snake Game
var p1_score = 0;
var p2_score = 0;
var r = 20;
var c = 20;

//Client Side
var cDir = 0;

// OBJECTS / COMMANDS TO BE PROCESSED
var set = new Set();
var map = new Map();
var queue = [];

function dumby(message) {
    console.log(message);
}

var boardInitialized = false;
var lagValue = 0;

var img_snake_head1 = document.getElementById("snake_head1"); //puts the snake image into the canvas.
var img_snake_head2 = document.getElementById("snake_head2");

function AddToSet(timeY) {
    set.add(timeY);
}

function AddToMap(timeY, cmdStruct) {
    map.set(timeY, cmdStruct);
}

function InitializeBoardArray(dimensions) {
    
    if (!boardInitialized) {
        // THIS CODE WORKS; THIS IS OUR BACKUP PLAN
        var cmdCutOff;
        for (var i = 0; i < dimensions.length; ++i) {
            if (dimensions[i] == 'x') {
                cmdCutOff = i; break;
            }
        }
        r = parseInt(dimensions.substring(0, cmdCutOff));
        c = parseInt(dimensions.substring(cmdCutOff + 1));

        board = [];
        for (var x = 0; x < c; x++) {
            board.push([]);
            for (var y = 0; y < r; y++) {
                if (x === 0 || x === r - 1) {
                    board[x].push(wall);
                }
                else if (y === 0 || y === c - 1) {
                    board[x].push(wall);
                }
                else {
                    board[x].push(free);
                }
            }
        }
        canvas = document.createElement("canvas");
        canvas.width = c * c;
        canvas.height = r * r;
        canvas.style = "position:absolute; left: 50%; width: 400px; margin-left: -200px;";
        context = canvas.getContext("2d");
        context.font = "20px Times New Roman";
        document.body.appendChild(canvas);
        boardInitialized = true;
    }
    else {
        ClearBoard();
        p1_score = 0;
        p2_score = 0;
    }
    //setInterval(sendTime(), 1000); //update latency every second
}

function ClearBoard() {
    for (var x = 0; x < c; x++) {
        for (var y = 0; y < r; y++) {
            if (x === 0 || x === r - 1) {
                board[x][y] = wall;
            }
            else if (y === 0 || y === c - 1) {
                board[x][y] = wall;
            }
            else {
                board[x][y] = free;
            }
        }
    }
}

var time = 0;
var delayTime = Date.now();

function drawBoard() {
    getInput();
    document.getElementById("realTime").value = lagValue;

    var current = Date.now();

    if (current - delayTime < 125) {
        if (set.size > 0) {

            var mapIter = map.entries();
            for (var i = 0; i < set.size; ++i) {
                if (current - mapIter.next().value.timeB < 125) {
                    queue.push(map[set[i]]);
                    map.delete[set[i]];
                }
            }
        }
    }
    else {
        delayTime = Date.now();
    }

    while (queue.size > 0) {
        var cmdStruct = queue.shift();
        cmdStruct.func(cmdStruct.str);
    }


    var width = r;
    var height = c;
    var snake1_pat = context.createPattern(snake_head1, "repeat"); //needs to have repeat for the image.
    var snake2_pat = context.createPattern(snake_head2, "repeat");
    var breadstick_pat = context.createPattern(breadstick, "repeat");
    for (var x = 0; x < width; x++) { //loops through the game board array and fills each space accordingly.
        for (var y = 0; y < height; y++) {

            switch (board[x][y]) {
                case free:
                    context.fillStyle = "#fff";
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
                    context.fillStyle = "#000";
                    break;
            }
            context.fillRect(x * width, y * height, width, height); //fills the canvas rectangle style.
        }
    }
    context.fillStyle = "#000"; //white background.
    context.fillText(p1id + " Score: " + p1_score, 20, canvas.height - 21); //for the score.
    context.fillText(p2id + " Score: " + p2_score, canvas.width - 120, canvas.height - 21);

    if (time++ == 750) {
        sendTime();
        time = 0;
    }

    window.requestAnimationFrame(drawBoard, canvas);
}

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

var cSnake =
{
    direction: null,
    _snake_array: null,
    last: null,

    init: function (direction, x, y) { //specifies which direction snake is facing towards and creates an array that keeps track where the snake's coordinates (head and tail is)
        this.direction = direction;
        this._snake_array = [];
        this.insert(x, y);
    },
    insert: function (x, y) { //not exactly sure what this does. the tutorial went through it real quick.
        this._snake_array.unshift({ x: x, y: y }); //from my understanding this puts the x and y coordinate to the front of the array.
        this.last = this._snake_array[0]; //by shifting the snake_array[0] to the front, this.last is set to the new last. (head of the snake)
    },
    remove: function () {
        return this._snake_array.pop(); //removes the last set of coordinate in the array to be set into free space.
    }
}

// x,y-x,y
function P1PosUpdate(pos) {
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

    board[xHead][yHead] = snake1_space;
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

    board[x][y] = free;
}

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

function UpdateP1Score(score) {
    p1_score = parseInt(score);
}

function UpdateP2Score(score) {
    p2_score = parseInt(score);
}

var prevKey2 = move_down;

function getInput() {
    if (keystate[move_left]) {
        if (prevKey2 != move_left) {
            prevKey2 = move_left;
            cDir = 2;
            send("setdir:2," + Date.now().toString());
        }
    }
    if (keystate[move_right]) {
        if (prevKey2 != move_right) {
            prevKey2 = move_right;
            cDir = 3;
            send("setdir:3," + Date.now().toString());
        }
    }
    if (keystate[move_up]) {
        if (prevKey2 != move_up) {
            prevKey2 = move_up;
            cDir = 0;
            send("setdir:0," + Date.now().toString());
        }
    }
    if (keystate[move_down]) {
        if (prevKey2 != move_down) {
            prevKey2 = move_down;
            cDir = 1;
            send("setdir:1," + Date.now().toString());
        }
    }
}

function sendTime() {
    timeAcheck = Date.now().toString();
    send("clienttime:" + timeAcheck);
}

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
    var timeA = parseInt(message.substring(0, clientTimeIndex));
    var timeX = parseInt(message.substring(clientTimeIndex + 1, serverTime1Index));
    var timeY = parseInt(message.substring(serverTime1Index + 1));

    var realTime = timeY + (((timeB - timeA) - (timeY - timeX)) / 2);
    lagValue = realTime - timeA;
}

var prevKey = null;

function main() {

    //for testing purposes
    document.getElementById("ip").value = "127.0.0.1";
    document.getElementById("port").value = "8000";

    startBtn.addEventListener('click', function () {
        send("startgame:");
    });
    keystate = {};
    document.addEventListener("keydown", function (evt) {
        if (prevKey != evt.keyCode) {
            keystate[evt.keyCode] = true;
            prevKey = evt.keyCode;
        }
    });

    document.addEventListener("keyup", function (evt) {
        delete keystate[evt.keyCode];
    });
}

main();
