var connectBtn = document.getElementById('cntBtn');
var startBtn = document.getElementById('startBtn');

connectBtn.addEventListener('click', function() {
  connect();
});


// Start of server
var Server;
var p1id;
var p2id;

//variables to switch screen to snake
var snakeGame = document.getElementById('snake');

//COMMANDS TO SEND TO THE SERVER
var SETP1NAME = "setp1name";
var SETP2NAME = "setp2name";
var GETP1NAME = "getp1name";
var GETP2NAME = "getp2name";

function log( text ) {
    $log = $('#log');
    //Add text to log
    $log.append(($log.val()?"\n":'')+text);
    //Autoscroll
    $log[0].scrollTop = $log[0].scrollHeight - $log[0].clientHeight;
}


function send( text ) {
Server.send( 'message', text );
}

function connect(){
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
Server.bind('open', function() {
    document.getElementById("cntBtn").disabled = true;
    log("Connected.");

    setTimeout(function(){
        var page1 = document.getElementById('connectPage'); /*variable for dean's page*/
        var page2 = document.getElementById('snakePage'); /*variable for ney's page*/

        connectBtn.style.visibility = 'hidden';
        startBtn.style.visibility = "visible";
    }, 1000);

    log('Welcome to snakes ' + p1id + " and " + p2id + "!");
    log("Click the Start Game button when you're ready to play.");

    var textInput = document.getElementById('log').value.split('\n');
    var lastValue;
    //get last value of log/textArea
    for(var i = 0; i < textInput.length; ++i){
      if(i == textInput.length-1){
          console.log('Inside textArea: ' + textInput[i]);
          lastValue = textInput[i];
      }
    }
});

//OH NOES! Disconnection occurred.
Server.bind('close', function( data ) {
        document.getElementById("cntBtn").disabled = false;
        log( "Disconnected." );
});

//Log any messages sent from server
Server.bind('message', function (payload) {

    if (payload == "startGame") {
        //self.location = "m1test.html";
    }
    else {
        log(payload);
    }
});

Server.connect();
}