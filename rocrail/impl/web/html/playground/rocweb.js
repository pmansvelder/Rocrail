/* Variables */
var req;
var yoffset = 48;
var planloaded = false;
var ws = null;
var initWS;
var worker;
var tapholdF1 = 0;
var zlevelMap = {};
var zlevelList = [];
var fbMap = {};
var tkMap = {};
var bkMap = {};
var swMap = {};
var lcMap = {};
var locoSelected = 'none';
var zlevelSelected = 'none';
var zlevelIdx = 0;
var power = 'false';
var donkey = 'false';
var didShowDonkey = false;
var shutdownTimer;

/* JSon test */
function jsonStorageTest() {
  // Test to save a session structure.
  var E03 = {color: "grey", name: "Spot", size: 46};
  E03.test = "Hallo";
  sessionStorage.setItem("E03", JSON.stringify(E03));
  
  var cat = JSON.parse(sessionStorage.getItem("E03"));
  console.log(cat.size + " " + cat.color + " " + cat.test );
}


/* Configuration */
function langDE() {
 document.getElementById("menuPower").innerHTML = "Gleisstrom";
}

function langEN() {
 document.getElementById("menuPower").innerHTML = "Power";
}


/* Info Dialog */
function openInfo()
{
  console.log("close menu");
  $( "#popupMenu" ).popup( "close" );
  
  console.log("open info");
  $('#popupMenu').on("popupafterclose", function(){$( "#popupInfo" ).popup( "open" )});

//  console.log("open info");
//  $( "#popupInfo" ).popup( "open" );
}


function openDonkey()
{
  console.log("open info");
  $( "#popupInfo" ).popup( "open" );
}


/* Throttle Dialog */
function openThrottle()
{
  var select = document.getElementById("locoSelect");
  while(select.options.length > 0) {
    select.remove(0);
  }
  for (var i in lcMap){
    var lc = lcMap[i];
    option = document.createElement( 'option' );
    option.value = lc.getAttribute('id');
    option.innerHTML = lc.getAttribute('id');
    select.add( option );
  }
  $( "#popupThrottle" ).popup( "open" );
}

function closeThrottle()
{
  $( "#popupThrottle" ).popup( "close" );
}


/* Menu Dialog */
function openMenu()
{
  $('#popupMenu').unbind("popupafterclose");
  $( "#popupMenu" ).popup( "open" );
}


/* Options Dialog */
function openOptions()
{
}

function closeOptions()
{
}


/* Send a XML Http request */
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


/* Throttle commands */
$(function(){
  $("#F1").bind("taphold", tapholdF1Handler);
 
  function tapholdF1Handler(e) {
    tapholdF1 = 1;
    console.log("taphold F1");
  }
});

function onFunction(id, nr) {
  if( tapholdF1 == 1 ) {
    tapholdF1 = 0;
    return;
  }
  lc = lcMap[locoSelected];
  console.log("Funtion: " + id + " ("+nr+") for loco " + locoSelected);
  var group = (nr-1)/4+1;
  var fx = parseInt(lc.getAttribute('fx'));
  var mask = 1 << nr;
  var on = fx&mask?"false":"true";
  var cmd = "<fn id=\""+locoSelected+"\" fnchanged=\""+nr+"\" group=\""+group+"\" f"+nr+"=\""+on+"\"/>"
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function speedUpdate(value) {
  lc = lcMap[locoSelected];
  console.log("Speed: " + value + " for loco " + locoSelected);
  var vVal = value * (parseInt(lc.getAttribute('V_max')/100.00));

  document.getElementById("direction").innerHTML = "" + value + " >";
  var cmd = "<lc throttleid=\"rocweb\" id=\""+locoSelected+"\" V=\""+vVal+"\" dir=\"%s\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}


/* System commands */
function actionPower() {
  var mOn = power == 'true' ? false:true;
  var cmd = "<sys informall=\"true\" cmd=\""+(mOn?"go":"stop")+"\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}


function actionLevelDown() {
  zlevelSelected.style.display = 'none'
  zlevelIdx++;
  var zleveldiv = zlevelList[zlevelIdx];
  if(zleveldiv == undefined ) {
    zleveldiv = zlevelList[0];
    zlevelIdx = 0;
  }
  zlevelSelected = zleveldiv;
  zleveldiv.style.display = 'block'
}
function actionLevelUp() {
  zlevelSelected.style.display = 'none'
    zlevelIdx--;
    var zleveldiv = zlevelList[zlevelIdx];
    if(zleveldiv == undefined ) {
      zleveldiv = zlevelList[0];
      zlevelIdx = 0;
    }
    zlevelSelected = zleveldiv;
    zleveldiv.style.display = 'block'
}


/* Item commands */
function actionSensor(id)
{
  fbid = id.replace("fb_","");
  console.log("sensor action on " + fbid );
  fb = fbMap[fbid];
  var cmd;
  if( "true" == fb.getAttribute('state') )
    cmd = "<fb state=\"false\" id=\""+fbid+"\"/>";
  else
    cmd = "<fb state=\"true\" id=\""+fbid+"\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function actionSwitch(id) {
  swid = id.replace("sw_","");
  sw = swMap[swid];
  console.log("switch action on " + swid + " state=" + sw.getAttribute('state'));
  var cmd = "<sw cmd=\"flip\" id=\""+swid+"\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function actionBlock(id)
{
  localStorage.setItem("block", id);
  document.getElementById("blockTitle").innerHTML = "Block: " + localStorage.getItem("block");
  $( "#popupBlock" ).popup( "open", {positionTo: '#'+id} );
}

/* Initial functions */
$(document).on("pagecreate",function(){

  // jQuery events go here...
  $("#speedSlider").on( "slidestop", function( event, ui ) {
  value = this.value;
  speedUpdate(value);} );
  
  $('#locoSelect').change(function() {
    console.log("locoSelect: " + this.value );
    var lc = lcMap[this.value]
    var img = document.getElementById("locoImage");
    img.src = "images/" + lc.getAttribute('image');
    console.log("new image: " + img.src);
    locoSelected = this.value;
  } );
  
  $('#languageSelect').change(function() {
    console.log("languageSelect: " + this.value );
    localStorage.lang = this.value;

    if( this.value == "de" )
      langDE();
    else if( this.value == "en" )
      langEN();
  } );

});

$(document).ready(function(){
  console.log("document ready");
  //$('.ui-slider-handle').height(50)
  var lang = localStorage.lang;
  var sel = document.getElementById('languageSelect');
  
  if( lang == "de" ) {
    langDE();
    //sel.selectedIndex = 0;
  }
  else {
    langEN();
    //sel.selectedIndex = 1;
  }

})


/* Old update function: Deprecated */
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


/* Load the plan.xml at startup */
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


/* Utilities */
function xml2string(node) {
  if (typeof(XMLSerializer) !== 'undefined') {
     var serializer = new XMLSerializer();
     return serializer.serializeToString(node);
  } else if (node.xml) {
     return node.xml;
  }
}

/* Update event handlers */
function handleSensor(fb) {
  console.log("sensor event: " + fb.getAttribute('id') + " " + fb.getAttribute('state'));
  var div = document.getElementById("fb_"+fb.getAttribute('id'));
  if( div != null ) {
    fbNode = fbMap[fb.getAttribute('id')];
    fbNode.setAttribute('state', fb.getAttribute('state'));
    div.style.backgroundImage = getSensorImage(fbNode);
  }
  else {
    console.log("sensor: " + fb.getAttribute('id') + " not found");
  }
}


function handleSwitch(sw) {
  console.log("switch event: " + sw.getAttribute('id') + " " + sw.getAttribute('state'));
  var div = document.getElementById("sw_"+sw.getAttribute('id'));
  if( div != null ) {
    swNode = swMap[sw.getAttribute('id')];
    swNode.setAttribute('state', sw.getAttribute('state'));
    div.style.backgroundImage = getSwitchImage(swNode);
  }
  else {
    console.log("switch: " + sw.getAttribute('id') + " not found");
  }
}


function handleFunction(fn) {
  console.log("function event: " + fn.getAttribute('id') + " changed=" + fn.getAttribute('fnchanged'));
  var lc = lcMap[fn.getAttribute('id')];
  var fnchanged = fn.getAttribute('fnchanged');
  var fx = parseInt(lc.getAttribute('fx'));
  var mask = 1 << parseInt(fnchanged); 
  var on = fn.getAttribute("f"+fnchanged);
  
  if( on == "true")
    fx = fx | mask;
  else
    fx = fx & ~mask;
  lc.setAttribute('fx', ""+fx)
  
  if( fn.getAttribute('id') == locoSelected ) {
    var div = document.getElementById("F"+fnchanged);
    if( on == "true")
      div.style.backgroundColor = "#FF8888";
    else
      div.style.backgroundColor = ''
  }
}


function handleBlock(bk) {
  console.log("block event: " + bk.getAttribute('id') + " " + bk.getAttribute('state'));
  var div = document.getElementById("bk_"+bk.getAttribute('id'));
  if( div != null ) {
    bkNode = bkMap[bk.getAttribute('id')];
    bkNode.setAttribute('state', bk.getAttribute('state'));
    bkNode.setAttribute('locid', bk.getAttribute('locid'));
    bkNode.setAttribute('reserved', bk.getAttribute('reserved'));
    bkNode.setAttribute('entering', bk.getAttribute('entering'));
    
    var label = bk.getAttribute('locid');
    if( label.length == 0 )
      label = bk.getAttribute('id');
    div.innerHTML = "<label class='itemtext'>"+label+"</label>";

    div.style.backgroundImage = getBlockImage(bkNode);
  }
  else {
    console.log("block: " + bk.getAttribute('id') + " not found");
  }
}

function handleState(state) {
  power = state.getAttribute('power');
  console.log("power: " + power );
  if( power == "true" )
    document.getElementById("headerPower").style.backgroundColor= "#FF8888";
  else 
    document.getElementById("headerPower").style.backgroundColor= '';
}

function handleSystem(sys) {
  //<sys cmd="shutdown" informall="true"/>
  var cmd = sys.getAttribute('cmd');
  if( cmd == "shutdown" ) {
    console.log("server shutdown");
    var cmd = "<sys shutdown=\"true\"/>";
    worker.postMessage(JSON.stringify({type:'shutdown', msg:cmd}));
    zlevelSelected.style.display = 'none'
  }
}

/* Processing events from server */
function evaluateEvent(xmlStr) {
  var xmlDoc = ( new window.DOMParser() ).parseFromString(xmlStr, "text/xml");
  var root = xmlDoc.documentElement;
  var evtName = root.nodeName;
  console.log("evaluate: " + evtName);
  if( evtName == "fb" )
    handleSensor(root);
  else if( evtName == "bk" )
    handleBlock(root);
  else if( evtName == "state" )
    handleState(root);
  else if( evtName == "sys" )
    handleSystem(root);
  else if( evtName == "fn" )
    handleFunction(root);
  else if( evtName == "sw" )
    handleSwitch(root);
}

function processResponse() {
  console.log("readyState="+req.readyState+" status="+req.status);
  if (req.readyState == 4 && (req.status == 0 || req.status == 200)) {
    // only if "OK"
    $.mobile.loading("show");
    try {
      xmlDoc = req.responseXML;
      if( xmlDoc != null ) {
        planlist = xmlDoc.getElementsByTagName("plan")
        if( planlist.length > 0 ) {
          var h = document.getElementById("title");
          donkey = planlist[0].getAttribute('donkey');
          title = planlist[0].getAttribute('title');
          console.log( "processing plan: " + title + " key=" + donkey );
          h.innerHTML = "Rocrail: " + title;
          processPlan();
          planloaded = true;

          if( donkey == 'true') {
            document.getElementById("donkey").style.display = 'none'
          }
          else {
            console.log( "5 minutes before shutdown..." );
            var shutdownTimer = setInterval(function () {doShutdown()}, (5 * 60 * 1000) );
            function doShutdown() {
              console.log("no key; shutdown...");
              clearInterval(shutdownTimer);
              var cmd = "<sys shutdown=\"true\"/>";
              worker.postMessage(JSON.stringify({type:'shutdown', msg:cmd}));
              zlevelSelected.style.display = 'none';
              document.getElementById("donkeyWarning").style.display = 'none'
              openDonkey();
            }

          }

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
            if( !didShowDonkey && donkey == 'false' ) {
              openDonkey();
              didShowDonkey = true;
            }
            /* ToDo: Evaluate server event. */
            evaluateEvent(result.answer);
          }
          else if(result.type == 'command') {
            console.log("command: "+result.msg);
            /* ToDo: Send to server. */
          }
          else {
            console.log("data: " + e.data);
          }
        }
      }
     
    }
    catch(e) {
      console.log("exception: " + e);
    }
    $.mobile.loading("hide");
    
  }

}


function getOriNr(Ori) {
  if(Ori == "north" )
    return 2;
  if(Ori == "east")
    return 3;
  if(Ori == "south")
    return 4;
  return 1;
}

function getSensorImage(fb) {
  var curve = fb.getAttribute('curve');
  var ori   = getOriNr(fb.getAttribute('ori'));
  if( curve != "true" )
    ori = (ori % 2 == 0) ? 2 : 1;

  if( "true" == fb.getAttribute('state') )
    if( curve == "true" )
      return "url('img/csensor_on_"+ori+".png')";
    else
      return "url('img/sensor_on_"+ori+".png')";
  else
    if( curve == "true" )
      return "url('img/csensor_off_"+ori+".png')";
    else
      return "url('img/sensor_off_"+ori+".png')";
}

function getTrackImage(tk) {
  var ori  = getOriNr(tk.getAttribute('ori'));
  var type = tk.getAttribute('type');
  var suffix = '';
  if (type == "curve" )
    return "url('img/curve"+suffix+"_"+ori+".png')";
  else if( type == "buffer" || type == "connector" ) {
    if (ori == 1)
      ori = 3;
    else if (ori == 3)
      ori = 1;
    return "url('img/"+type+suffix+"_"+ori+".png')";
  }
  else {
    return "url('img/track"+suffix+"_"+ (ori % 2 == 0 ? 2 : 1) + ".png')";
  }
  
}

function getSwitchImage(sw) {
  var ori   = getOriNr(sw.getAttribute('ori'));
  var type  = sw.getAttribute('type');
  var state = sw.getAttribute('state');
  var rasterStr = "";
  var suffix = "";
  if (ori == 1)
    ori = 3;
  else if (ori == 3)
    ori = 1;

  if (type=="right") {
    if (state=="straight")
      return "url('img/turnout"+rasterStr+"_rs"+suffix+"_"+ori+".png')";
    else
      return "url('img/turnout"+rasterStr+"_rt"+suffix+"_"+ori+".png')";

  }
  else if (type=="left") {
    if (state=="straight")
      return "url('img/turnout"+rasterStr+"_ls"+suffix+"_"+ori+".png')";
    else
      return "url('img/turnout"+rasterStr+"_lt"+suffix+"_"+ori+".png')";

  }
  
}

function getBlockImage(bk) {
  var ori   = getOriNr(bk.getAttribute('ori'));
  ori = (ori % 2 == 0) ? 2 : 1;
  var label = bk.getAttribute('locid');
  var suffix = "";
  
  if( "true" == bk.getAttribute('smallsymbol') )
    suffix = "_s";

  if( "true" == bk.getAttribute('entering') )
    return "url('img/block_enter_"+ori+suffix+".png')";
  else if( "closed" == bk.getAttribute('state') )
    return "url('img/block_closed_"+ori+suffix+".png')";
  if( "true" == bk.getAttribute('reserved') )
    return "url('img/block_reserved_"+ori+suffix+".png')";
  else if( label.length > 0 )
    return "url('img/block_occ_"+ori+suffix+".png')";
  else
    return "url('img/block_"+ori+suffix+".png')";
  
}

/* Process the plan.xml */
function processPlan() {
//only if req shows "loaded"
   try {
     xmlDoc = req.responseXML;
     
     zlevellist = xmlDoc.getElementsByTagName("zlevel");
     if( zlevellist.length > 0 ) {
       console.log("processing " + zlevellist.length + " zlevels");
       for (var i = 0; i < zlevellist.length; i++) {
         var z = zlevellist[i].getAttribute('z');
         console.log('zlevel: ' + z + " title: " + zlevellist[i].getAttribute('title'));

         var newdiv = document.createElement('div');
         newdiv.setAttribute('id', "level_" + z);
         zlevelMap[z] = newdiv;
         zlevelList[i] = newdiv;

         if( zlevelSelected == 'none' ) {
           zlevelSelected = newdiv;
           zlevelIdx = i;
         }
         else {
           console.log("disable level " + z);
           newdiv.style.display = 'none'
         }
         document.body.appendChild(newdiv);
       }
     }
     
     
     lclist = xmlDoc.getElementsByTagName("lc");
     if( lclist.length > 0 )
       console.log("processing " + lclist.length + " locos");

     for (var i = 0; i < lclist.length; i++) {
       console.log('loco: ' + lclist[i].getAttribute('id') );
       lcMap[lclist[i].getAttribute('id')] = lclist[i];
     }
     
     
     tklist = xmlDoc.getElementsByTagName("tk");
     if( tklist.length > 0 )
       console.log("processing " + tklist.length + " tracks");

     for (var i = 0; i < tklist.length; i++) {
       var z     = tklist[i].getAttribute('z');
       var ori   = getOriNr(tklist[i].getAttribute('ori'));
       var leveldiv = zlevelMap[z]; 
       console.log('track: ' + tklist[i].getAttribute('id') + "at level " + z);
       tkMap[tklist[i].getAttribute('id')] = tklist[i];
       var newdiv = document.createElement('div');
       newdiv.setAttribute('id', "tk_"+tklist[i].getAttribute('id'));
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       newdiv.style.width    = "32px";
       newdiv.style.height   = "32px";
       newdiv.style.left     = "" + (parseInt(tklist[i].getAttribute('x')) * 32) + "px";
       newdiv.style.top      = "" + (parseInt(tklist[i].getAttribute('y')) * 32 + yoffset) + "px";
       newdiv.innerHTML      = "";
       newdiv.style.backgroundImage = getTrackImage(tklist[i]);
       //console.log("Track image="+newdiv.style.backgroundImage + "    " + getTrackImage(tklist[i]));

       leveldiv.appendChild(newdiv);
     }
     

     swlist = xmlDoc.getElementsByTagName("sw");
     if( swlist.length > 0 )
       console.log("processing " + swlist.length + " switches");

     for (var i = 0; i < swlist.length; i++) {
       var z     = swlist[i].getAttribute('z');
       var leveldiv = zlevelMap[z]; 
       console.log('switch: ' + swlist[i].getAttribute('id') + "at level " + z);
       swMap[swlist[i].getAttribute('id')] = swlist[i];
       var newdiv = document.createElement('div');
       newdiv.setAttribute('id', "sw_"+swlist[i].getAttribute('id'));
       newdiv.setAttribute('onClick', "actionSwitch(this.id)");
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       newdiv.style.width    = "32px";
       newdiv.style.height   = "32px";
       newdiv.style.left     = "" + (parseInt(swlist[i].getAttribute('x')) * 32) + "px";
       newdiv.style.top      = "" + (parseInt(swlist[i].getAttribute('y')) * 32 + yoffset) + "px";
       newdiv.innerHTML      = "";
       
       newdiv.style.backgroundImage = getSwitchImage(swlist[i]);
       //console.log("Sensor image="+newdiv.style.backgroundImage);

       leveldiv.appendChild(newdiv);
     }
     
     
     fblist = xmlDoc.getElementsByTagName("fb");
     if( fblist.length > 0 )
       console.log("processing " + fblist.length + " sensors");

     for (var i = 0; i < fblist.length; i++) {
       var z     = fblist[i].getAttribute('z');
       var curve = fblist[i].getAttribute('curve');
       var ori   = getOriNr(fblist[i].getAttribute('ori'));
       if( z == undefined )
         z = '0';
       if( curve != "true" )
         ori = (ori % 2 == 0) ? 2 : 1;

       var leveldiv = zlevelMap[z]; 
       console.log('sensor: ' + fblist[i].getAttribute('id') + "at level " + z);
       fbMap[fblist[i].getAttribute('id')] = fblist[i];
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
       
       newdiv.style.backgroundImage = getSensorImage(fblist[i]);
       //console.log("Sensor image="+newdiv.style.backgroundImage);

       leveldiv.appendChild(newdiv);
       //document.body.appendChild(newdiv);
     }
     
     bklist = xmlDoc.getElementsByTagName("bk");
     if( bklist.length > 0 )
       console.log("processing " + bklist.length + " blocks");
     for (var i = 0; i < bklist.length; i++) {
       var z = bklist[i].getAttribute('z');
       if( z == undefined )
         z = '0';
       var leveldiv = zlevelMap[z]; 
       console.log('block: ' + bklist[i].getAttribute('id') + " at level " + z);
       bkMap[bklist[i].getAttribute('id')] = bklist[i];
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
       var label = bklist[i].getAttribute('locid');
       if( label.length == 0 )
         label = bklist[i].getAttribute('id');
       newdiv.innerHTML      = "<label class='itemtext'>"+label+"</label>";
       newdiv.style.textAlign= "center";
       
       //console.log(xml2string(bklist[i]));
       newdiv.style.backgroundImage = getBlockImage(bklist[i]);

       leveldiv.appendChild(newdiv);
       //document.body.appendChild(newdiv);
     }
   }
   catch(e) {
     console.log("exception: " + e);
   }

}


