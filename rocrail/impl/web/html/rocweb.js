/* Variables */
var req;
var yoffset = 48;
var planloaded = false;
var ws = null;
var initWS;
var worker;
var tapholdF1 = 0;
var zlevelDivMap = {};
var zlevelDivList = [];
var zlevelMap = {};
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
var FGroup = 0;

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
    if( locoSelected == lc.getAttribute('id')) {
      select.selectedIndex = i;
      option.selected = 'selected';
      select.value = locoSelected; 
    }
  }
  $('#locoSelect').selectmenu("refresh");
  initThrottle();
  updateFunctionLabels();
  $( "#popupThrottle" ).popup( "open" );
}

function updateFunctionLabels() {
  for(i = 1; i < 15; i++) {
    var F = document.getElementById("F"+i);
    F.innerHTML = "F" + (i + FGroup * 14); 
    //console.log("function button " + i + " = " + F.innerHTML);
  }

  lc = lcMap[locoSelected];
  if( lc != undefined ) {
    //var fundeflist = lc.childNodes;
    var fundeflist = lc.getElementsByTagName("fundef");
    console.log("function defs " + fundeflist.length + " for " + lc.getAttribute('id'));
    if( fundeflist.length > 0 ) {
      for( n = 0; n < fundeflist.length; n++ ){ 
        console.log("fundef " + fundeflist[n].getAttribute('fn') + " text: " + fundeflist[n].getAttribute('text'));
        if( fundeflist[n].getAttribute('icon') ) {
          var F = document.getElementById("F"+fundeflist[n].getAttribute('fn'));
          F.innerHTML = "<img src='"+fundeflist[n].getAttribute('icon')+"'/>";
        }
      }
    }
  }

}

function onFG() {
  if( FGroup == 0 )
    FGroup = 1;
  else
    FGroup = 0;
  updateFunctionLabels(); 
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

function onDirection() {
  lc = lcMap[locoSelected];
  if( lc == undefined )
    return;
  var dir = lc.getAttribute('dir');
  if( dir == 'true' )
    lc.setAttribute('dir', 'false');
  else
    lc.setAttribute('dir', 'true');
  updateDir();
  speedUpdate(parseInt(lc.getAttribute('V')));
}

function updateDir() {
  lc = lcMap[locoSelected];
  if( lc == undefined ) return;
  var dir = lc.getAttribute('dir');
  var V = lc.getAttribute('V');
  if( dir == 'true')
    document.getElementById("direction").innerHTML = "" + V + " >";
  else
    document.getElementById("direction").innerHTML = "< " + V;
}

function onFunction(id, nr) {
  if( tapholdF1 == 1 ) {
    tapholdF1 = 0;
    return;
  }
  lc = lcMap[locoSelected];
  if( lc == undefined ) return;
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
  if( lc == undefined ) return;
  console.log("Speed: " + value + " for loco " + locoSelected);
  var vVal = value * (parseInt(lc.getAttribute('V_max')/100.00));
  lc.setAttribute('V', vVal);
  updateDir();
  var cmd = "<lc throttleid=\"rocweb\" id=\""+locoSelected+"\" V=\""+vVal+"\" dir=\""+lc.getAttribute('dir')+"\"/>";
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
  var zleveldiv = zlevelDivList[zlevelIdx];
  if(zleveldiv == undefined ) {
    zleveldiv = zlevelDivList[0];
    zlevelIdx = 0;
  }
  zlevelSelected = zleveldiv;
  zleveldiv.style.display = 'block'
}
function actionLevelUp() {
  zlevelSelected.style.display = 'none'
    zlevelIdx--;
    var zleveldiv = zlevelDivList[zlevelIdx];
    if(zleveldiv == undefined ) {
      zleveldiv = zlevelDivList[0];
      zlevelIdx = 0;
    }
    zlevelSelected = zleveldiv;
    zleveldiv.style.display = 'block'
}


/* Item commands */
function actionAuto(auto) {
  console.log("auto action " + auto);
  var cmd = "<auto cmd=\""+auto+"\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function actionEBreak() {
  console.log("emergancy break");
  var cmd = "<sys cmd=\"ebreak\" informall=\"true\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

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


function initThrottle() {
  console.log("locoSelect: " + locoSelected );
  var lc = lcMap[locoSelected]
  if( lc != undefined ) {
    var img = document.getElementById("locoImage");
    img.src = lc.getAttribute('image');
    console.log("new image: " + img.src);
    document.getElementById("speedSlider").value = lc.getAttribute('V');
    $("#speedSlider").slider("refresh");
    updateDir();
  }
}


/* Initial functions */
$(document).on("pagecreate",function(){

  // jQuery events go here...
  $("#speedSlider").on( "slidestop", function( event, ui ) {
  value = this.value;
  speedUpdate(value);} );
  
  $('#locoSelect').change(function() {
    locoSelected = this.value;
    localStorage.setItem("locoSelected", locoSelected);
    initThrottle();
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
  //$('.ui-slider').input("none");
  //$('.ui-slider-track').marginLeft("15px");

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
           div.style.backgroundImage = "url(sensor_on_1.png)";
         else
           div.style.backgroundImage = "url(sensor_off_1.png)";
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


function handleLoco(lc) {
  
}


function handleFunction(fn) {
  console.log("function event: " + fn.getAttribute('id') + " changed=" + fn.getAttribute('fnchanged'));
  var lc = lcMap[fn.getAttribute('id')];
  
  if( lc == undefined)
    return;
  
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
  else if( evtName == "lc" )
    handleLoco(root);
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
          h.innerHTML = title;
          processPlan();
          planloaded = true;
          locoSelected = localStorage.getItem("locoSelected");
          console.log("selected loco = " + locoSelected);

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

function getOri(item) {
  var Ori = item.getAttribute('ori');
  if(Ori == "north" )
    return Ori;
  if(Ori == "east")
    return Ori;
  if(Ori == "south")
    return Ori;
  return "west";
}

function getSensorImage(fb) {
  var curve = fb.getAttribute('curve');
  var ori   = getOri(fb);

  if( "true" == fb.getAttribute('state') )
    if( curve == "true" )
      return "url('curve-sensor-on."+ori+".svg')";
    else
      return "url('sensor-on."+ori+".svg')";
  else
    if( curve == "true" )
      return "url('curve-sensor-off."+ori+".svg')";
    else
      return "url('sensor-off."+ori+".svg')";
}

function getTrackImage(tk) {
  var ori  = getOri(tk);
  var type = tk.getAttribute('type');
  var suffix = '';
  if (type == "curve" )
    return "url('curve."+ori+".svg')";
  else if( type == "buffer" || type == "connector" ) {
    return "url("+type+"."+ori+".svg')";
  }
  else {
    return "url('straight"+"."+ ori + ".svg')";
  }
  
}

function getSwitchImage(sw) {
  var ori   = getOri(sw);
  var type  = sw.getAttribute('type');
  var state = sw.getAttribute('state');
  var rasterStr = "";
  var suffix = "";

  if (type=="right") {
    if (state=="straight")
      return "url('turnoutright"+"."+ori+".svg')";
    else
      return "url('turnoutright-t"+"."+ori+".svg')";

  }
  else if (type=="left") {
    if (state=="straight")
      return "url('turnoutleft"+"."+ori+".svg')";
    else
      return "url('turnoutleft-t"+"."+ori+".svg')";

  }
  
}

function getBlockImage(bk) {
  var ori   = getOri(bk);
  ori = (ori % 2 == 0) ? 2 : 1;
  var label = bk.getAttribute('locid');
  var suffix = "";
  
  if( "true" == bk.getAttribute('smallsymbol') )
    suffix = "-s";

  if( "true" == bk.getAttribute('entering') )
    return "url('block-ent"+suffix+"."+ori+".svg')";
  else if( "closed" == bk.getAttribute('state') )
    return "url('block-closed"+suffix+"."+ori+".svg')";
  if( "true" == bk.getAttribute('reserved') )
    return "url('block-res"+suffix+"."+ori+".svg')";
  else if( label.length > 0 )
    return "url('block-occ"+suffix+"."+ori+".svg')";
  else
    return "url('block"+suffix+"."+ori+".svg')";
  
}

/* Process the plan.xml */
function processPlan() {
//only if req shows "loaded"
   try {
     xmlDoc = req.responseXML;
     
     zlevellist = xmlDoc.getElementsByTagName("zlevel");
     if( zlevellist.length > 0 ) {
       var title = "";
       console.log("processing " + zlevellist.length + " zlevels");
       for (var i = 0; i < zlevellist.length; i++) {
         zlevelMap[z] = zlevellist[i];
         var z = zlevellist[i].getAttribute('z');
         console.log('zlevel: ' + z + " title: " + zlevellist[i].getAttribute('title'));

         var newdiv = document.createElement('div');
         newdiv.setAttribute('id', "level_" + z);
         zlevelDivMap[z] = newdiv;
         zlevelDivList[i] = newdiv;

         if( zlevelSelected == 'none' ) {
           zlevelSelected = newdiv;
           zlevelIdx = i;
           title = zlevellist[i].getAttribute('title');
         }
         else {
           console.log("disable level " + z);
           newdiv.style.display = 'none'
         }
         document.body.appendChild(newdiv);
       }
       var h = document.getElementById("title");
       h.innerHTML = title;
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
       var leveldiv = zlevelDivMap[z]; 
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
       var leveldiv = zlevelDivMap[z]; 
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

       var leveldiv = zlevelDivMap[z]; 
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
       var ori   = getOriNr(bklist[i].getAttribute('ori'));
       ori = (ori % 2 == 0) ? 2 : 1;
       var leveldiv = zlevelDivMap[z]; 
       console.log('block: ' + bklist[i].getAttribute('id') + " at level " + z);
       bkMap[bklist[i].getAttribute('id')] = bklist[i];
       var newdiv = document.createElement('div');
       newdiv.setAttribute('id', "bk_"+bklist[i].getAttribute('id'));
       newdiv.setAttribute('onClick', "actionBlock(this.id)");
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       newdiv.style.width    = ori==1?"128px":"32px";
       newdiv.style.height   = ori==1?"32px":"128px";
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


