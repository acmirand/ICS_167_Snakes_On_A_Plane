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
var move_left = 37; //left-key
var move_right = 39; //right-key
var move_up = 38; //up-key
var move_down = 40; //down-key

//Snake Game
var r = 20;
var c = 20;

var img_snake_head1 = document.getElementById("snake_head1"); //puts the snake image into the canvas.
var img_snake_head2 = document.getElementById("snake_head2");

function InitializeBoardArray(dimensions) {

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
    canvas.width = c*c;
    canvas.height = r*r;
    canvas.style = "position:absolute; left: 50%; width: 400px; margin-left: -200px;";
    context = canvas.getContext("2d");
    context.font = "20px Times New Roman";
    document.body.appendChild(canvas);
}

function drawBoard() {
  var width = r;
  var height = c;
  var snake1_pat = context.createPattern(snake_head1, "repeat"); //needs to have repeat for the image.
  var snake2_pat = context.createPattern(snake_head2, "repeat");
  for (var x =0; x < width; x++){ //loops through the game board array and fills each space accordingly.
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
           context.fillStyle = "#f00";
           break;
        case wall:
            context.fillStyle = "#000";
          break;
      }
      context.fillRect(x*width, y*height, width, height); //fills the canvas rectangle style.
        }
    }
    context.fillStyle = "#000"; //white background.
  //context.fillText("P1 Score: " + p1_score, 50, canvas.height - 50); //for the score.
  //context.fillText("P2 Score: " + p2_score, canvas.width - 150, canvas.height - 50);

    window.requestAnimationFrame(drawBoard,canvas);
}

function SetFood(pos) {

    console.log(pos);

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

// x,y-x,y
function P1PosUpdate(pos) {
    var cmdCutOff;
    for (var i = 0; i < pos.length; ++i) {
        if (pos[i] == '-') {
            cmdCutOff = i; break;
        }
    }

    var HeadPos = pos.substring(0, cmdCutOff);
    var TailPos = pos.substring(cmdCutOff + 1);

    for (var i = 0; i < HeadPos.length; ++i) {
        if (HeadPos[i] == ',') {
            cmdCutOff = i; break;
        }
    }
    var xHead = parseInt(HeadPos.substring(0, cmdCutOff));
    var yHead = parseInt(HeadPos.substring(cmdCutOff + 1));

    for (var i = 0; i < TailPos.length; ++i) {
        if (TailPos[i] == ',') {
            cmdCutOff = i; break;
        }
    }
    var xTail = parseInt(TailPos.substring(0, cmdCutOff));
    var yTail = parseInt(TailPos.substring(cmdCutOff + 1));

    board[xHead][yHead] = snake1_space;
    board[xTail][yTail] = snake1_space;
}

function P2PosUpdate(pos) {
    var cmdCutOff;
    for (var i = 0; i < pos.length; ++i) {
        if (pos[i] == '-') {
            cmdCutOff = i; break;
        }
    }

    var HeadPos = pos.substring(0, cmdCutOff);
    var TailPos = pos.substring(cmdCutOff + 1);

    for (var i = 0; i < HeadPos.length; ++i) {
        if (HeadPos[i] == ',') {
            cmdCutOff = i; break;
        }
    }
    var xHead = parseInt(HeadPos.substring(0, cmdCutOff));
    var yHead = parseInt(HeadPos.substring(cmdCutOff + 1));

    for (var i = 0; i < TailPos.length; ++i) {
        if (TailPos[i] == ',') {
            cmdCutOff = i; break;
        }
    }
    var xTail = parseInt(TailPos.substring(0, cmdCutOff));
    var yTail = parseInt(TailPos.substring(cmdCutOff + 1));

    board[xHead][yHead] = snake2_space;
    board[xTail][yTail] = snake2_space;
}

function getInput() {
    if (keystate[move_left]) send(makeDirMessage(2));
    if (keystate[move_right]) send(makeDirMessage(3));
    if (keystate[move_up]) send(makeDirMessage(0));
    if (keystate[move_down]) send(makeDirMessage(1));
}

function loop() {
    P1PosUpdate();
    P2PosUpdate();
    drawBoard();
}

function main() {

 startBtn.addEventListener('click', function() {
   var page1 = document.getElementById('connectPage');
   var page2 = document.getElementById('snakePage');

   page1.style.display = 'none';
   page2.style.display = 'block';

   send("startgame:");
 });

 keystate = {};
 document.addEventListener("keydown", function (evt) {
     keystate[evt.keyCode] = true;
 });

 document.addEventListener("keyup", function (evt) {
     delete keystate[evt.keyCode];
 });

}

main();
