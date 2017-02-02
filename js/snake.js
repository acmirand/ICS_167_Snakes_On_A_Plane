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
var SETP1NAME = "setp1name:";
var SETP2NAME = "setp2name:";
var GETP1NAME = "getp1name:";
var GETP2NAME = "getp2name:";
var P1SCORED = "p1scored:";
var P2SCORED = "p2scored:";
var RESETGAME = "resetgame:";

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
    p2id = document.getElementById('p2id').value; //get p2 id from text field
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

        setTimeout(function () {
            var page1 = document.getElementById('connectPage'); /*variable for dean's page*/
            var page2 = document.getElementById('snakePage'); /*variable for ney's page*/

            connectBtn.style.visibility = 'hidden';
            startBtn.style.visibility = "visible";
        }, 1000);

        // Once a connection to the server has been established, send over the names
        // of the players. 
        send(SETP1NAME + p1id);
        send(SETP2NAME + p2id);

        //log('Welcome to snakes ' + p1id + " and " + p2id + "!");
        log("Click the Start Game button when you're ready to play.");

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
            log(payload);
        }
        else if (command == "updateP1Score") {
            p1_score = parseInt(message);
        }
        else if (command == "updateP2Score") {
            p2_score = parseInt(message);
        }

    });

    Server.connect();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GAME CODE BELOW
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Snake Game
var col = 30; //gameboard size
var row = 30;

//array value content
var free_space = 0;
var snake1_space = 1;
var snake2_space = 2;
var food_space = 3;
var wall_space = 4;

//directional relations
var dir_up = 0;
var dir_down = 1;
var dir_left = 2;
var dir_right = 3;

//p1 keystate numbers
var move_left = 37; //left-key
var move_right = 39; //right-key
var move_up = 38; //up-key
var move_down = 40; //down-key

//p2 keystate numbers
var move_left2 = 65; //a-key
var move_right2 = 68; //d-key
var move_up2 = 87; //w-key
var move_down2 = 83; //s-key

var game_state =
{
  width: null,
  height: null,
  _game_state: null,

  init: function(free,wall,c,r){
    this.width = c;
    this.height = r;
    this._game_state = [];
    for(var x = 0; x < c; x++){
      this._game_state.push([]);
      for(var y = 0; y < r; y++){
        if(x === 0 || x === r-1){
          this._game_state[x].push(wall);
        }
        else if(y === 0 || y === c-1){
          this._game_state[x].push(wall);
        }
        else{
          this._game_state[x].push(free);
        }
      }
    }
  },
  set: function(val,x,y){
    this._game_state[x][y] = val;
  },
  get: function(x,y){
    return this._game_state[x][y];
  }
}

function setFood() {
  var food_set = true;
  while (food_set){
    var randx = Math.floor(Math.random() * row);
    var randy = Math.floor(Math.random() * col);
    if (game_state.get(randx,randy) === free_space)
    {
      game_state.set(food_space, randx, randy);
      food_set = false;
    }
  }
}

var p1_snake =
{
  direction: null,
  _snake_array: null,
  last: null,

  init: function(direction,x,y){
    this.direction = direction;
    this._snake_array = [];
    this.insert(x,y);
  },
  insert: function(x,y){
    this._snake_array.unshift({x:x, y:y});
    this.last = this._snake_array[0];
  },
  remove: function(){
    return this._snake_array.pop();
  }
}

var p2_snake =
{
  direction: null,
  _snake_array: null,
  last: null,

  init: function(direction,x,y){
    this.direction = direction;
    this._snake_array = [];
    this.insert(x,y);
  },
  insert: function(x,y){
    this._snake_array.unshift({x:x, y:y});
    this.last = this._snake_array[0];
  },
  remove: function(){
    return this._snake_array.pop();
  }
}

var canvas;
var context;
var frames1;
var frames2;
var frameSpeed;
var keystate;

var img_snake_head1 = document.getElementById("snake_head1");
var img_snake_head2 = document.getElementById("snake_head2");

function init() {

    // TELL THE SERVER TO RESET THE SCORE
    send(RESETGAME);
  game_state.init(free_space,wall_space,col,row);
  var p1_start = {x:col-3, y:row-2};
  var p2_start = {x:col-5, y:row-5};

  p1_snake.init(dir_up, p1_start.x, p1_start.y);
  game_state.set(snake1_space, p1_start.x, p1_start.y);
  p2_snake.init(dir_left, p2_start.x, p2_start.y);
  game_state.set(snake2_space, p2_start.x, p2_start.y);
  setFood();

}

function p1_update() {
  frames1++;
  if (keystate[move_left] && p1_snake.direction !== dir_right) p1_snake.direction = dir_left;
  if (keystate[move_right] && p1_snake.direction !== dir_left) p1_snake.direction = dir_right;
  if (keystate[move_up] && p1_snake.direction !== dir_down) p1_snake.direction = dir_up;
  if (keystate[move_down] && p1_snake.direction !== dir_up) p1_snake.direction = dir_down;

  if (frames1 % frameSpeed === 0) {
    var nx = p1_snake.last.x;
    var ny = p1_snake.last.y;
    switch (p1_snake.direction){
      case dir_left:
        nx--;
        break;
      case dir_right:
        nx++;
        break;
      case dir_up:
        ny--;
        break;
      case dir_down:
        ny++;
        break;
    }
    if (game_state.get(nx, ny) === snake1_space || game_state.get(nx, ny) === wall_space || game_state.get(nx, ny) === snake2_space) {
      return init();
    }
    if (game_state.get(nx,ny) === food_space){
      var tail = {x:nx, y:ny};
        //UPDATE SERVER WITH THE SCORE HERE
      send(P1SCORED);
      //p1_score++;
      setFood();
    }
    else{
      var tail = p1_snake.remove();
      game_state.set(free_space, tail.x, tail.y);
      tail.x = nx;
      tail.y = ny;
    }
    p1_snake.insert(nx,ny);
    game_state.set(snake1_space,tail.x,tail.y);
    }
}

function p2_update() {
  frames2++;
  if (keystate[move_left2] && p2_snake.direction !== dir_right) p2_snake.direction = dir_left;
  if (keystate[move_right2] && p2_snake.direction !== dir_left) p2_snake.direction = dir_right;
  if (keystate[move_up2] && p2_snake.direction !== dir_down) p2_snake.direction = dir_up;
  if (keystate[move_down2] && p2_snake.direction !== dir_up) p2_snake.direction = dir_down;

  if (frames2 % frameSpeed === 0){
    var nx = p2_snake.last.x;
    var ny = p2_snake.last.y;
    switch (p2_snake.direction){
      case dir_left:
        nx--;
        break;
      case dir_right:
        nx++;
        break;
      case dir_up:
        ny--;
        break;
      case dir_down:
        ny++;
        break;
    }
    if (game_state.get(nx, ny) === snake2_space || game_state.get(nx, ny) === wall_space || game_state.get(nx, ny) === snake1_space) {
      return init();
    }
    if (game_state.get(nx,ny) === food_space){
      var tail = {x:nx, y:ny};
      send(P2SCORED);
      //p2_score++;
      setFood();
    }
    else{
      var tail = p2_snake.remove();
      game_state.set(free_space, tail.x, tail.y);
      tail.x = nx;
      tail.y = ny;
    }
    p2_snake.insert(nx,ny);
    game_state.set(snake2_space,tail.x,tail.y);
    }
  }

function draw_board() {
  var width = row;
  var height = col;
  var snake1_pat = context.createPattern(snake_head1, "repeat");
  var snake2_pat = context.createPattern(snake_head2, "repeat");
  for (var x =0; x < game_state.width; x++){
    for (var y = 0; y < game_state.height; y++){
      switch (game_state.get(x,y)){
        case free_space:
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
        case wall_space:
          context.fillStyle = "#000";
          break;
      }
      context.fillRect(x*width, y*height, width, height);
    }
  }
  context.fillStyle = "#000";
  context.fillText("P1 Score: " + p1_score, 50, canvas.height - 50);
  context.fillText("P2 Score: " + p2_score, canvas.width - 150, canvas.height - 50);
}

function loop() {
  p1_update();
  p2_update();
  draw_board();
  window.requestAnimationFrame(loop,canvas);
}

function main() {
    frames1 = 0;
    frames2 = 0;
    frameSpeed = 10;
  canvas = document.createElement("canvas");
  canvas.width = col*col;
  canvas.height = row*row;
  context = canvas.getContext("2d");
  context.font = "20px Times New Roman";
  document.body.appendChild(canvas);
  keystate = {};
  document.addEventListener("keydown",function(evt) {
    keystate[evt.keyCode] = true;
  });
  document.addEventListener("keyup",function(evt) {
    delete keystate[evt.keyCode];
  });

  startBtn.addEventListener('click', function() {
    var page1 = document.getElementById('connectPage'); /*variable for dean's page*/
    var page2 = document.getElementById('snakePage'); /*variable for ney's page*/

    page1.style.display = "none";
    page2.style.display = "block";
    init();
    loop();
  });
}

main();
