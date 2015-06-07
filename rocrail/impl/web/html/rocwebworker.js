var retryWebSocket = 0;
var retryMax = 20;

function debug(msg) {                                                           
  postMessage("<debug>"+msg+"</debug>");                          
}

function alert(msg) {                                                           
  postMessage("<alert>"+msg+"</alert>");                          
}

function fatal(msg) {                                                           
  postMessage("<fatal>"+msg+"</fatal>");                          
}

function connected(msg) {                                                           
  postMessage("<connected>"+msg+"</connected>");                          
}

function doWebSocket() {
  var host = location.hostname;
  host.replace("www.","");
  debug("creating a websocket...ws://" + host + ":" + location.port);
  
  ws = new WebSocket("ws://"+host+":"+location.port, "rcp");
  retryWebSocket++;
  ws.onopen = function()
  {
    retryWebSocket = 0;
    ws.send("<sys cmd=\"getstate\"/>");
    connected("websocket connection is established...");
  };
  ws.onerror = function (error) {
    alert('WebSocket Error: ' + error);
    /*
    ws.close(1000, "<error/>");
    if( retryWebSocket < retryMax ) {
      doWebSocket();
      debug('WebSocket retry='+retryWebSocket);
    }
    else
      fatal("Rocweb:\nWebSocket fatal error; Give up after "+retryMax+" retries: "+error);
    */
  };
  ws.onmessage = function (evt) 
  {
    //debug("onmessage: "+evt.data);
    postMessage(evt.data);
  };
  ws.onclose = function(event)
  { 
     // websocket is closed.
    if( event.code == 1006 && retryWebSocket < retryMax) {
      doWebSocket();
      alert('WebSocket retry='+retryWebSocket);
    }
    else if( event.code == 1001 ) {
      alert("Rocweb:\nWebSocket is closed: " + event.code + "\nEnd of Demo.");
    }
    else {
      alert("Rocweb:\nWebSocket is closed: " + event.code);
    }
  };
  
}

onmessage = function(e) {
  //debug('command received:' + e.data);
  ws.send(e.data);
}

doWebSocket();
