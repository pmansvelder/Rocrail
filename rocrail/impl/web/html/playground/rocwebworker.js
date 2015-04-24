
function debug(msg) {                                                           
  postMessage(JSON.stringify({type:'debug',msg:msg}));                          
}

function doWebSocket() {
  var host = location.hostname;
  host.replace("www.","");
  debug("creating a websocket...ws://" + host + ":" + location.port);
  ws = new WebSocket("ws://"+host+":"+location.port, "rcp");
  ws.onopen = function()
  {
     // Web Socket is connected, send data using send()
     ws.send("<fb cmd=\"flip\"/>");
     debug("websocket connection is established...");
  };
  ws.onerror = function (error) {
    debug('WebSocket Error ' + error);
  };
  ws.onmessage = function (evt) 
  {
    debug("websocket message receiving...");
    var received_msg = evt.data;
    debug("websocket message received: " + received_msg);
    postMessage(JSON.stringify({type:'response', answer:received_msg}));
  };
  ws.onclose = function()
  { 
     // websocket is closed.
    debug("websocket is closed..."); 
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
    ws.close();
  }
  
  debug('Posting message back to main script');
  postMessage(e.data);
}

doWebSocket();
