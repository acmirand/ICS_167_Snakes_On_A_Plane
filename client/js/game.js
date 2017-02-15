var canvas;
var context;
var board;
var boardArray;
var wall = 1;
var free = 0;
var r = 2;
var c = 0;

function drawBoard(boardString)  {

  for (var i=0; i< boardString.length; ++i) {
    if (boardString[i] == 'n') {
      r++;
    }
    else if (r <= 3 && boardString[i] != 'n') {
      c++;
    }
  }

  var j = 0;

  board = new Array(c);
  for (var i = 0; i < c; ++i){
    board[i] = new Array(r);
  }

  for (var i = 0; i < c; ++i){
    for (var j = 0; j < r; ++j){
      board[i][j] = 0;
    }
  }
  // board = new Array(c);
  // for (var i = 0; i < c; ++i){
  //   board[i] = 0;
  // }
  // board[0] = new Array(r);
  // for (var i = 0; i < r; ++i){
  //   board[0][i] = 0;
  // }
  var index = 0;
  for (var i = 0; i < c; ++i){
    for (var j = 0; j < r; ++j){
      if (boardString[index] == '0') {			//free space
        board[i][j] = free;
      }
      else if (boardString[index] == '1') {		//wall
        board[i][j] = wall;
      }
      ++index;
  }
}

  // for (var i = 0; i < boardString.length; ++i) {
  //
  //   console.log(i);
  //
	// 	if (boardString[i] == 'n') { //increments row counter when loop reaches an "n"
	// 		++j;
  //     //board.push([]);
	// 	}
  //   else{
	// 	if (boardString[i] == '0') {			//free space
	// 		//board[i].push(free);
  //     board[i][j] = free;
  //
	// 	}
	// 	else if (boardString[i] == '1') {		//wall
	// 		//board[i].push(wall);
  //     board[i][j] = wall;
  //
	// 	}
  // }
	//}
  for (var x =0; x < r; x++){ //loops through the game board array and fills each space accordingly.
    for (var y = 0; y < c; y++){
      switch (board[x][y]){
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
      context.fillRect(x*c, y*r, c, r); //fills the canvas rectangle style.
    }
  }
  context.fillStyle = "#000"; //white background.
  //context.fillText("P1 Score: " + p1_score, 50, canvas.height - 50); //for the score.
  //context.fillText("P2 Score: " + p2_score, canvas.width - 150, canvas.height - 50);

    window.requestAnimationFrame(drawBoard,canvas);
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

   page1.style.display = "none";
   page2.style.display = "block";

   send("startgame:");
 });
}

main();
