var Server;
var p1id;
var p2id;

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
    log(p1id + " " + p2id);
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
});

//OH NOES! Disconnection occurred.
Server.bind('close', function( data ) {
        document.getElementById("cntBtn").disabled = false;
        log( "Disconnected." );
});

//Log any messages sent from server
Server.bind('message', function (payload) {

    if (payload == "startGame") {
        self.location = "m1test.html";
    }
    else {
        log(payload);
    }
});

Server.connect();
}
