var retryWebSocket = 0;

function debug(msg) {                                                           
  postMessage(JSON.stringify({type:'debug',msg:msg}));                          
}

function alert(msg) {                                                           
  postMessage(JSON.stringify({type:'alert',msg:msg}));                          
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
  };
  ws.onerror = function (error) {
    alert('WebSocket Error ' + error);
    if( retryWebSocket < 10 ) {
      doWebSocket();
      debug('WebSocket retry='+retryWebSocket);
    }
    else
      alert('WebSocket fatal error; Give up.');
  };
  ws.onmessage = function (evt) 
  {
    debug("websocket message receiving...");
    var received_msg = evt.data;
    debug("websocket message received: " + received_msg);
    postMessage(JSON.stringify({type:'response', answer:received_msg}));
  };
  ws.onclose = function(event)
  { 
     // websocket is closed.
    alert("websocket is closed: " + event.code);
    if( event.code == 1006 ) {
      doWebSocket();
      debug('WebSocket retry='+retryWebSocket);
    }
  };
  
}

onmessage = function(e) {
  debug('Message received from main script');
  var result = JSON.parse(e.data);
  if(result.type == 'command') {
    debug("worker command: "+result.msg);
    ws.send(result.msg);
    /* ToDo: Send to server. */
  }
  if(result.type == 'shutdown') {
    debug("closing websocket...");
    ws.close(1000, "<shutdown/>");
    return;
  }
  
  debug('Posting message back to main script');
  postMessage(e.data);
}

doWebSocket();
