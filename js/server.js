var Server;
var p1id;
var p2id;

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

Server.bind('getACs', function () {
  log("hello");
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

  if (payload == "Game On!") {
      self.location = "m1test.html";
      log("game on");
  }

log( payload );
});

Server.connect();
}
