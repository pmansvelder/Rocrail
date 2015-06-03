var retryWebSocket = 0;

function debug(msg) {                                                           
  postMessage("<debug>"+msg+"</debug>");                          
}

function alert(msg) {                                                           
  postMessage("<alert>"+msg+"</alert>");                          
}

function fatal(msg) {                                                           
  postMessage("<fatal>"+msg+"</fatal>");                          
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
    debug("websocket connection is established...");
    ws.send("<sys cmd=\"getstate\"/>");
  };
  ws.onerror = function (error) {
    debug('WebSocket Error: ' + error);
    ws.close(1000, "<error/>");
    if( retryWebSocket < 50 ) {
      doWebSocket();
      debug('WebSocket retry='+retryWebSocket);
    }
    else
      fatal('Rocweb:\nWebSocket fatal error; Give up after 50 retries.');
  };
  ws.onmessage = function (evt) 
  {
    //debug("onmessage: "+evt.data);
    postMessage(evt.data);
  };
  ws.onclose = function(event)
  { 
     // websocket is closed.
    if( event.code == 1006 && retryWebSocket < 50) {
      doWebSocket();
      debug('WebSocket retry='+retryWebSocket);
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
