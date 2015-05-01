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
var coMap = {};
var sgMap = {};
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

function actionOutput(id) {
  coid = id.replace("co_","");
  co = coMap[coid];
  console.log("output action on " + coid + " state=" + co.getAttribute('state'));
  var cmd = "<co cmd=\"flip\" id=\""+coid+"\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function actionSignal(id) {
  sgid = id.replace("sg_","");
  sg = sgMap[sgid];
  console.log("signal action on " + sgid + " state=" + sg.getAttribute('state'));
  var cmd = "<sg cmd=\"flip\" id=\""+sgid+"\"/>";
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


function handleOutput(co) {
  console.log("output event: " + co.getAttribute('id') + " " + co.getAttribute('state'));
  var div = document.getElementById("co_"+co.getAttribute('id'));
  if( div != null ) {
    coNode = coMap[co.getAttribute('id')];
    coNode.setAttribute('state', co.getAttribute('state'));
    div.style.backgroundImage = getOutputImage(coNode);
  }
  else {
    console.log("output: " + co.getAttribute('id') + " not found");
  }
}


function handleSwitch(sw) {
  console.log("switch event: " + sw.getAttribute('id') + " " + sw.getAttribute('state'));
  var div = document.getElementById("sw_"+sw.getAttribute('id'));
  if( div != null ) {
    swNode = swMap[sw.getAttribute('id')];
    swNode.setAttribute('state', sw.getAttribute('state'));
    div.style.backgroundImage = getSwitchImage(swNode, div);
  }
  else {
    console.log("switch: " + sw.getAttribute('id') + " not found");
  }
}


function handleSignal(sg) {
  console.log("signal event: " + sg.getAttribute('id') + " " + sg.getAttribute('state'));
  var div = document.getElementById("sg_"+sg.getAttribute('id'));
  if( div != null ) {
    sgNode = sgMap[sg.getAttribute('id')];
    sgNode.setAttribute('state', sg.getAttribute('state'));
    sgNode.setAttribute('aspect', sg.getAttribute('aspect'));
    div.style.backgroundImage = getSignalImage(sgNode, div);
  }
  else {
    console.log("signal: " + sg.getAttribute('id') + " not found");
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

    div.style.backgroundImage = getBlockImage(bkNode, div);
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
  else if( evtName == "co" )
    handleOutput(root);
  else if( evtName == "sg" )
    handleSignal(root);
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
  var accnr = parseInt(fb.getAttribute('accnr'));
  var ori   = getOri(fb);

  if( "true" == fb.getAttribute('state') )
    if( accnr > 0 )
      return "url('accessory-"+accnr+"-on."+ori+".svg')";
    else if( curve == "true" )
      return "url('curve-sensor-on."+ori+".svg')";
    else
      return "url('sensor-on."+ori+".svg')";
  else
    if( accnr > 0 )
      return "url('accessory-"+accnr+"-off."+ori+".svg')";
    else if( curve == "true" )
      return "url('curve-sensor-off."+ori+".svg')";
    else
      return "url('sensor-off."+ori+".svg')";
}

function getOutputImage(co) {
  var ori   = getOri(co);
  var svg   = co.getAttribute('svgtype');
  var state = co.getAttribute('state');
  var suffix = '';
  if( state == undefined )
    state = "off";
  return "url('button-"+svg+"-"+state+"."+ ori + ".svg')";
}

function getSignalImage(sg) {
  var ori     = getOri(sg);
  var state   = sg.getAttribute('state');
  var signal  = sg.getAttribute('signal');
  var pattern = parseInt( sg.getAttribute('usepatterns') );
  var suffix  = '';
  
  var greennr  = parseInt( sg.getAttribute('greennr') );;
  var rednr    = parseInt( sg.getAttribute('rednr') );;
  var yellownr = parseInt( sg.getAttribute('yellownr') );;
  var whitenr  = parseInt( sg.getAttribute('whitenr') );;
  var nr       = parseInt( sg.getAttribute('aspect') );
  
  if( pattern == 2 ) {
    if( nr == -1 )
      nr = 0;
    if( nr >= 0 && nr < 5 ) {
      if( greennr == nr )
        state = "green";
      else if( rednr == nr )
        state = "red";
      else if( yellownr == nr )
        state = "yellow";
      else if( whitenr == nr )
        state = "white";
      else
        state = "red";
    }
  }
  console.log("signal image: usepatterns="+pattern+" nr="+nr+" greennr="+greennr+" rednr="+rednr+" yellownr="+yellownr+" whitenr="+whitenr+" state="+state);
  var aspect  = "r";
  if( state == "red"    ) aspect = "r";
  if( state == "green"  ) aspect = "g";
  if( state == "yellow" ) aspect = "y";
  if( state == "white"  ) aspect = "w";

  return "url('signalmain-"+aspect+"."+ ori + ".svg')";
}

function getTrackImage(tk) {
  var ori   = getOri(tk);
  var type  = tk.getAttribute('type');
  var suffix = '';
  if (type == "curve" )
    return "url('curve."+ori+".svg')";
  else if( type == "buffer" || type == "connector" || type == "dcurve" || type == "dir" || type == "dirall" ) {
    return "url('"+type+"."+ori+".svg')";
  }
  else {
    return "url('straight"+"."+ ori + ".svg')";
  }
}

function getSwitchImage(sw, div) {
  var ori   = getOri(sw);
  var type  = sw.getAttribute('type');
  var state = sw.getAttribute('state');
  var accnr = sw.getAttribute('accnr');
  var dir   = sw.getAttribute('dir');
  var rectc = sw.getAttribute('rectcrossing');
  var rasterStr = "";
  var suffix = "";

  console.log("switch type: " + type + " accnr="+accnr);
  
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
  else if (type=="twoway") {
    if (state=="straight")
      return "url('twoway-tr"+"."+ori+".svg')";
    else
      return "url('twoway-tl"+"."+ori+".svg')";
  }
  else if (type=="threeway") {
    if (state=="left")
      return "url('threeway-tl"+"."+ori+".svg')";
    else if (state=="right")
      return "url('threeway-tr"+"."+ori+".svg')";
    else
      return "url('threeway"+"."+ori+".svg')";
  }
  else if (type=="crossing") {
    if( rectc == "true") {
      if (state=="straight")
        return "url('crossing"+"."+ori+".svg')";
      else
        return "url('crossing-t"+"."+ori+".svg')";
    }
    else {
      var direction = (dir == "true" ? "left":"right");
      if (state=="turnout")
        return "url('crossing"+direction+"-t."+ori+".svg')";
      else
        return "url('crossing"+direction+"-r."+ori+".svg')";
    }
  }
  else if (type=="ccrossing") {
    return "url('ccrossing"+"."+ori+".svg')";
  }
  else if (type=="dcrossing") {
    if( ori == "west" || ori == "east") {
      div.style.width    = "64px";
      div.style.height   = "32px";
    }
    else {
      div.style.width    = "32px";
      div.style.height   = "64px";
    }
    var direction = (dir == "true" ? "left":"right");
    if (state=="left")
      return "url('dcrossing"+direction+"-tl."+ori+".svg')";
    else if (state=="right")
      return "url('dcrossing"+direction+"-tr."+ori+".svg')";
    else if (state=="turnout")
      return "url('dcrossing"+direction+"-t."+ori+".svg')";
    else
      return "url('dcrossing"+direction+"."+ori+".svg')";
  }
  else if (type=="decoupler") {
    if (state=="straight")
      return "url('decoupler-on"+"."+ori+".svg')";
    else
      return "url('decoupler"+"."+ori+".svg')";
  }
  else if (type=="accessory") {

    if( accnr == "51" ) {
      if( ori == "west" || ori == "east") {
        div.style.width    = "128px";
        div.style.height   = "64px";
      }
      else {
        div.style.width    = "64px";
        div.style.height   = "128px";
      }
    }
    else if( accnr == "52" ) {
      if( ori == "west" || ori == "east") {
        div.style.width    = "128px";
        div.style.height   = "32px";
      }
      else {
        div.style.width    = "32px";
        div.style.height   = "128px";
      }
    }
    
    if(state=="turnout")
      return "url('accessory-"+accnr+"-off"+"."+ori+".svg')";
    else
      return "url('accessory-"+accnr+"-on"+"."+ori+".svg')";
  }  
}

function getBlockImage(bk, div) {
  var ori   = getOri(bk);
  var label = bk.getAttribute('locid');
  var suffix = "";

  if( ori == "north" || ori == "south" ) {
    div.style.width    = "32px";
    div.style.height   = "128px";
  }
  else {
    div.style.width    = "128px";
    div.style.height   = "32px";
  }

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
     
     
     colist = xmlDoc.getElementsByTagName("co");
     if( colist.length > 0 )
       console.log("processing " + colist.length + " outputs");

     for (var i = 0; i < colist.length; i++) {
       var z     = colist[i].getAttribute('z');
       var ori   = getOriNr(colist[i].getAttribute('ori'));
       var leveldiv = zlevelDivMap[z]; 
       console.log('output: ' + colist[i].getAttribute('id') + "at level " + z);
       coMap[colist[i].getAttribute('id')] = colist[i];
       var newdiv = document.createElement('div');
       newdiv.setAttribute('id', "co_"+colist[i].getAttribute('id'));
       newdiv.setAttribute('onClick', "actionOutput(this.id)");
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       newdiv.style.width    = "32px";
       newdiv.style.height   = "32px";
       newdiv.style.left     = "" + (parseInt(colist[i].getAttribute('x')) * 32) + "px";
       newdiv.style.top      = "" + (parseInt(colist[i].getAttribute('y')) * 32 + yoffset) + "px";
       newdiv.innerHTML      = "";
       newdiv.style.backgroundImage = getOutputImage(colist[i]);
       console.log("Output image="+newdiv.style.backgroundImage);

       leveldiv.appendChild(newdiv);
     }
     
     
     sglist = xmlDoc.getElementsByTagName("sg");
     if( sglist.length > 0 )
       console.log("processing " + sglist.length + " signals");

     for (var i = 0; i < sglist.length; i++) {
       var z     = sglist[i].getAttribute('z');
       var ori   = getOriNr(sglist[i].getAttribute('ori'));
       var leveldiv = zlevelDivMap[z]; 
       console.log('signal: ' + sglist[i].getAttribute('id') + "at level " + z);
       sgMap[sglist[i].getAttribute('id')] = sglist[i];
       var newdiv = document.createElement('div');
       newdiv.setAttribute('id', "sg_"+sglist[i].getAttribute('id'));
       newdiv.setAttribute('onClick', "actionSignal(this.id)");
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       newdiv.style.width    = "32px";
       newdiv.style.height   = "32px";
       newdiv.style.left     = "" + (parseInt(sglist[i].getAttribute('x')) * 32) + "px";
       newdiv.style.top      = "" + (parseInt(sglist[i].getAttribute('y')) * 32 + yoffset) + "px";
       newdiv.innerHTML      = "";
       newdiv.style.backgroundImage = getSignalImage(sglist[i]);
       console.log("Signal image="+newdiv.style.backgroundImage);

       leveldiv.appendChild(newdiv);
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
       console.log("Track image="+newdiv.style.backgroundImage);

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
       
       newdiv.style.backgroundImage = getSwitchImage(swlist[i], newdiv);
       console.log("Switch image="+newdiv.style.backgroundImage);

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
       var ori = getOri(bklist[i]);
       var leveldiv = zlevelDivMap[z]; 
       console.log('block: ' + bklist[i].getAttribute('id') + " at level " + z);
       bkMap[bklist[i].getAttribute('id')] = bklist[i];
       var newdiv = document.createElement('div');
       newdiv.setAttribute('id', "bk_"+bklist[i].getAttribute('id'));
       newdiv.setAttribute('onClick', "actionBlock(this.id)");
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       newdiv.style.width    = "128px";
       newdiv.style.height   = "32px";
       newdiv.style.left     = "" + (parseInt(bklist[i].getAttribute('x')) * 32) + "px";
       newdiv.style.top      = "" + (parseInt(bklist[i].getAttribute('y')) * 32 + yoffset) + "px";
       var label = bklist[i].getAttribute('locid');
       if( label.length == 0 )
         label = bklist[i].getAttribute('id');
       if( ori == "north" || ori == "south" ) {
         newdiv.innerHTML      = "<div class='itemtextV'>"+label+"</div>";
         newdiv.style.lineHeight = "128px";
       }
       else {
         newdiv.innerHTML      = "<label class='itemtext'>"+label+"</label>";
         newdiv.style.lineHeight = "32px";
       }
       newdiv.style.textAlign= "center";
       
       //console.log(xml2string(bklist[i]));
       newdiv.style.backgroundImage = getBlockImage(bklist[i], newdiv);

       leveldiv.appendChild(newdiv);
       //document.body.appendChild(newdiv);
     }
   }
   catch(e) {
     console.log("exception: " + e);
   }

}


