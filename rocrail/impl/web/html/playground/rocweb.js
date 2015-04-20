var req;
var yoffset = 48;
var planloaded = false;
var ws = null;
var initWS;
var worker;

function openInfo()
{
  console.log("close menu");
  $( "#popupMenu" ).popup( "close" );
  
  console.log("open info");
  $('#popupMenu').on("popupafterclose", function(){$( "#popupInfo" ).popup( "open" )});

//  console.log("open info");
//  $( "#popupInfo" ).popup( "open" );
}


function openThrottle()
{
  $( "#popupThrottle" ).popup( "open" );
}

function closeThrottle()
{
  $( "#popupThrottle" ).popup( "close" );
}

function openMenu()
{
  $('#popupMenu').unbind("popupafterclose");
  $( "#popupMenu" ).popup( "open" );
  
  // Test to save a session structure.
  var E03 = {color: "grey", name: "Spot", size: 46};
  E03.test = "Hallo";
  sessionStorage.setItem("E03", JSON.stringify(E03));

  var cat = JSON.parse(sessionStorage.getItem("E03"));
  console.log(cat.size + " " + cat.color + " " + cat.test );

}

function openOptions()
{
}

function closeOptions()
{
}

function sendCommand(cmd) {
  // send an XMLHttpRequest
  console.log("send command: " + cmd);
  try {
    var xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function()
    {
      if (xmlhttp.readyState==4 && xmlhttp.status==200)
      {
        processUpdate(xmlhttp);
      }
    };
    
    xmlhttp.open("GET", "rocweb.xml?"+cmd, true);
    xmlhttp.send("");
  } 
    catch(e) {
      console.log("exception: " + e);
  }
  
}

function actionPower() {
  var cmd = "<system cmd=\"poweron\"/>";
  //sendCommand(cmd);
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}


function actionSensor(id)
{
  console.log("sensor action on " + id );
  var xmlhttp;
  // send an XMLHttpRequest
  try {
    //req.abort();
    xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function()
    {
      if (xmlhttp.readyState==4 && xmlhttp.status==200)
      {
        processUpdate(xmlhttp);
      }
    };
    
    xmlhttp.open("GET", "rocweb.xml?fb_flip="+id.replace("fb_",""), true);
    xmlhttp.send("");
  } 
    catch(e) {
      console.log("exception: " + e);
    }
  }



function actionBlock(id)
{
  localStorage.setItem("block", id);
  document.getElementById("blockTitle").innerHTML = "Block: " + localStorage.getItem("block");
  $( "#popupBlock" ).popup( "open", {positionTo: '#'+id} );
}

function speedUpdate(value) {
  document.getElementById("direction").innerHTML = "" + value + " >";
  $("#direction").css("background","#557755");
}

$(document).on("pagecreate",function(){

  // jQuery events go here...
  $("#speedSlider").on( "slidestop", function( event, ui ) {
  value = this.value;
  speedUpdate(value);} );
  
  $('#locoSelect').change(function() {
    console.log("locoSelect: " + this.value );
  } );

});

/*
$(document).ready(function(){
  //$('.ui-slider-handle').height(50)
})
*/

function processUpdate(p_req) {
//only if req shows "loaded"
if (p_req.readyState == 4) {
 console.log("response status = "+p_req.status);
 if (p_req.status == 0 || p_req.status == 200) {
   try {
     xmlDoc = p_req.responseXML;
     fblist = xmlDoc.getElementsByTagName("fb");
     if( fblist.length > 0 )
       console.log("updating " + fblist.length + " sensors");

     for (var i = 0; i < fblist.length; i++) {
       console.log("sensor: " + fblist[i].getAttribute('id'));
       var div = document.getElementById("fb_"+fblist[i].getAttribute('id'));
       if( div != null ) {
         if( "true" == fblist[i].getAttribute('state') )
           div.style.backgroundImage = "url(img/sensor_on_1.png)";
         else
           div.style.backgroundImage = "url(img/sensor_off_1.png)";
       }
       else {
         console.log("sensor: " + fblist[i].getAttribute('id') + " not found");
       }
     
     }
   }
   catch(e) {
     console.log("exception: " + e);
   }

 }
 }
}

function loadPlan() {
 // send an XMLHttpRequest
 try {
   req = new XMLHttpRequest();
   req.onreadystatechange = processResponse;
   req.open("GET", "plan.xml", true);
   req.send("");
 } 
 catch(e) {
   console.log("exception: " + e);
 }
}

function xml2string(node) {
  if (typeof(XMLSerializer) !== 'undefined') {
     var serializer = new XMLSerializer();
     return serializer.serializeToString(node);
  } else if (node.xml) {
     return node.xml;
  }
}

function processResponse() {
  console.log("readyState="+req.readyState+" status="+req.status);
  if (req.readyState == 4 && (req.status == 0 || req.status == 200)) {
    // only if "OK"
    try {
      xmlDoc = req.responseXML;
      if( xmlDoc != null ) {
        planlist = xmlDoc.getElementsByTagName("plan")
        if( planlist.length > 0 ) {
          console.log( "processing plan: " + planlist[0].nodeName );
          var h = document.getElementById("title");
          h.innerHTML = "Rocrail: " + planlist[0].getAttribute('title');
          processPlan();
          planloaded = true;
        }
        else {
          console.log( "processing event" );
          processUpdate(req);
        }
      }
      else {
        console.log( "??? xmlDoc=" + xmlDoc);
      }

      if( planloaded ) {
        worker = new Worker("rocwebworker.js");
        worker.onmessage = function (e) {
          var result = JSON.parse(e.data);
          if(result.type == 'debug') {
            console.log(result.msg);
          } 
          else if(result.type == 'response') {
            console.log("response: "+result.answer);
            /* ToDo: Evaluate server event. */
          }
          else if(result.type == 'command') {
            console.log("command: "+result.msg);
            /* ToDo: Send to server. */
          }
          else {
            console.log("data: " + e.data);
          }
        }
   /*     
        console.log( "long poll server event..." );
        try {
          req = new XMLHttpRequest();
          req.onreadystatechange = processResponse;
          req.open("GET", "update.xml", true);
          req.send("");
        } 
        catch(e) {
          console.log("exception: " + e);
        }
        */
      }
     
    }
    catch(e) {
      console.log("exception: " + e);
    }
    
  }

}


function processPlan() {
//only if req shows "loaded"
   try {
     xmlDoc = req.responseXML;
     
     fblist = xmlDoc.getElementsByTagName("fb");
     if( fblist.length > 0 )
       console.log("processing " + fblist.length + " sensors");

     for (var i = 0; i < fblist.length; i++) {
       console.log('sensor: ' + fblist[i].getAttribute('id'));
       var newdiv = document.createElement('div');
       newdiv.setAttribute('id', "fb_"+fblist[i].getAttribute('id'));
       newdiv.setAttribute('onClick', "actionSensor(this.id)");
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       newdiv.style.width    = "32px";
       newdiv.style.height   = "32px";
       newdiv.style.left     = "" + (parseInt(fblist[i].getAttribute('x')) * 32) + "px";
       newdiv.style.top      = "" + (parseInt(fblist[i].getAttribute('y')) * 32 + yoffset) + "px";
       newdiv.innerHTML      = "";
       
       if( "true" == fblist[i].getAttribute('state') )
         newdiv.style.backgroundImage = "url('img/sensor_on_1.png')";
       else
         newdiv.style.backgroundImage = "url('img/sensor_off_1.png')";

       document.body.appendChild(newdiv);
     }
     
     bklist = xmlDoc.getElementsByTagName("bk");
     if( bklist.length > 0 )
       console.log("processing " + bklist.length + " blocks");
     for (var i = 0; i < bklist.length; i++) {
       console.log('block: ' + bklist[i].getAttribute('id'));
       var newdiv = document.createElement('div');
       newdiv.setAttribute('id', "bk_"+bklist[i].getAttribute('id'));
       newdiv.setAttribute('onClick', "actionBlock(this.id)");
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       newdiv.style.width    = "128px";
       newdiv.style.height   = "32px";
       newdiv.style.lineHeight = "32px";
       newdiv.style.left     = "" + (parseInt(bklist[i].getAttribute('x')) * 32) + "px";
       newdiv.style.top      = "" + (parseInt(bklist[i].getAttribute('y')) * 32 + yoffset) + "px";
       newdiv.innerHTML      = "<label class='itemtext'>"+bklist[i].getAttribute('locid')+"</label>";
       newdiv.style.textAlign= "center";
       
       //console.log(xml2string(bklist[i]));
       console.log("block entering=" + bklist[i].getAttribute("entering") + " x,y " + newdiv.style.left + "," + newdiv.style.top );

       if( "true" == bklist[i].getAttribute('entering') )
         newdiv.style.backgroundImage = "url('img/block_enter_1.png')";
       else if( "closed" == bklist[i].getAttribute('state') )
         newdiv.style.backgroundImage = "url('img/block_closed_1.png')";
       else
         newdiv.style.backgroundImage = "url('img/block_occ_1.png')";

       document.body.appendChild(newdiv);
     }
   }
   catch(e) {
     console.log("exception: " + e);
   }

}


