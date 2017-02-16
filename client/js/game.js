var canvas;
var context;

// Array values and content
var boardName;
var board;
var wall = 1;
var free = 0;
var boardInitialized = false;

//Controls
var move_left = 37; //left-key
var move_right = 39; //right-key
var move_up = 38; //up-key
var move_down = 40; //down-key

//Snake Game
var r = 0;
var c = 0;

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

    //var colSet = false;
    //// READ IN THE STRING WHICH IS THE LEVEL
    //for (var i = 0; i < boardName.length; ++i) {
    //    if (boardName[i] == 'n') {
    //        r++;
    //        colSet = true;
    //    }
    //    if (colSet == false) {
    //        ++c;
    //    }

    //}

    //board = [];
    //for (var x = 0; x < c; x++) {
    //    board.push([]);
    //    for (var y = 0; y < r; y++) {
    //        if (x === 0 || x === r - 1) {
    //            board[x].push(free);
    //        }
    //        else if (y === 0 || y === c - 1) {
    //            board[x].push(free);
    //        }
    //        else {
    //            board[x].push(free);
    //        }
    //    }
    //}

    ////INITIALIZE THE ARRAY
    //board = new Array(c);
    //for (var i = 0; i < c; ++i) {
    //    board[i] = new Array(r);
    //}

    ////INITIALIZE THE 2D PART OF THE ARRAY
    //for (var i = 0; i < c; ++i) {
    //    for (var j = 0; j < r; ++j) {
    //        board[i][j] = 0;
    //    }
    //}
}

function drawBoard() {

  //   //RUN THROUGH THE ENTIRE STRING USING INDEX
  //   //THEN USE A DOULBE FOR LOOP TO ASSIGN EACH POSITION ON THE BOARD
  //   //THE APPROPRIATE DESGINATION.
  //var index = 0;
  //for (var i = 0; i < c; ++i){
  //  for (var j = 0; j < r; ++j){
  //      if (boardName[index] == '0') {			//free space
  //          board[i][j] = free;
  //    }
  //      else if (boardName[index] == '1') {		//wall
  //          board[i][j] = wall;
  //    }
  //    ++index;
  //  }
  //}

  var width = r;
  var height = c;
  for (var x =0; x < width; x++){ //loops through the game board array and fills each space accordingly.
      for (var y = 0; y < height; y++) {

          switch (board[x][y]) {
        case free:
            context.fillStyle = "#fff";
          break;
        // case snake1_space:
        //   context.fillStyle = snake1_pat;
        //   break;
        // case snake2_space:
        //   context.fillStyle = snake2_pat;
        //   break;
        // case food_space:
        //   context.fillStyle = "#f00";
        //   break;
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

  //window.requestAnimationFrame(drawBoard,canvas);
}

function update() {
    if (keystate[move_left]) send(makeDirMessage(2));
    if (keystate[move_right]) send(makeDirMessage(3));
    if (keystate[move_up]) send(makeDirMessage(0));
    if (keystate[move_down]) send(makeDirMessage(1));
}

function main() {
 canvas = document.createElement("canvas");
 canvas.width = c*c;
 canvas.height = r*r;
 canvas.style = "position:absolute; left: 50%; width: 400px; margin-left: -200px;";
 context = canvas.getContext("2d");
 context.font = "20px Times New Roman";
 document.body.appendChild(canvas);

 startBtn.addEventListener('click', function() {
   var page1 = document.getElementById('connectPage');
   var page2 = document.getElementById('snakePage');

   page1.style.display = 'none';
   page2.style.display = 'block';

   send("startgame:");
 });
}

main();
