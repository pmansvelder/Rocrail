/* Variables */
var req;
var yoffset = 48;
var blockPointsize = 12;
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
var txMap = {};
var sbMap = {};
var ttMap = {};
var fyMap = {};
var stMap = {};
var carMap = {};
var lcCatMap = {};
var locoSelected = 'none';
var locoBlockSelect = 'none';
var zlevelSelected = 'none';
var zlevelIdx = 0;
var power = 'false';
var autoMode = 'off';
var donkey = 'false';
var didShowDonkey = false;
var shutdownTimer;
var FGroup = 0;
var rocrailversion = '';
var rocrailpwd = '';

function forceRedraw(div){
  var disp = div.style.display;
  div.style.display = 'none';
  var trick = div.offsetHeight;
  div.style.display = disp;
};

function trace(msg) {
  var debug = localStorage.getItem("debug");
  if( debug == undefined )
    debug = "false";
  if( debug == "true" )
    console.log(msg);
}

/* JSon test */
function jsonStorageTest() {
  // Test to save a session structure.
  var E03 = {color: "grey", name: "Spot", size: 46};
  E03.test = "Hallo";
  sessionStorage.setItem("E03", JSON.stringify(E03));
  
  var cat = JSON.parse(sessionStorage.getItem("E03"));
  trace(cat.size + " " + cat.color + " " + cat.test );
}


/* Configuration */
function langDE() {
  var donkey = "<br>Rocrail wird vollständig auf freiwilliger Basis betrieben. Trotzdem benötigt Rocrail Geld. Ihre fortwährende finanzielle Unterstützung ist notwendig, um Rocrail verfügbar zu halten. Falls Sie bereits einen Beitrag geleistet haben, können Sie einen Schlüssel beantragen, um diesen Dialog zu unterdrücken: donate@rocrail.net<br>";
  donkey += "<div id=\"donkeyWarning\"><b>Rocweb beendet nach 5 Minuten die Kommunikation!</b></div>";
  document.getElementById("donkey").innerHTML = donkey;

  document.getElementById("menuInfo").innerHTML = "Informationen";
  document.getElementById("menuSystem").innerHTML = "System";
  document.getElementById("menuOptions").innerHTML = "Optionen";
  document.getElementById("systemTitle").innerHTML = "System";
  document.getElementById("systemInitField").innerHTML = "Feld initialisieren";
  document.getElementById("systemQuerySensors").innerHTML = "Tages-Anfang";
  document.getElementById("systemEmergencyBreak").innerHTML = "Nothalt";
  document.getElementById("optionsTitle").innerHTML = "Optionen";
  document.getElementById("labOptionDebug").innerHTML = "Debug";
  document.getElementById("labOptionSimSensors").innerHTML = "Rückmelder simulieren";
  document.getElementById("labLocoCatEngine").innerHTML = "Antriebsart";
  document.getElementById("labLocoCatEra").innerHTML = "Epoche";
  document.getElementById("labLocoCatRoadname").innerHTML = "Gesellschaft";
  document.getElementById("labBlockStart").innerHTML = "Zug starten";
  document.getElementById("labBlockStop").innerHTML = "Zug anhalten";
  document.getElementById("labBlockClose").innerHTML = "Schliessen";
  document.getElementById("labBlockOpen").innerHTML = "Öffnen";
  document.getElementById("labTTNext").innerHTML = "Nächstes Gleis";
  document.getElementById("labTTPrev").innerHTML = "Vorheriges Gleis";
  document.getElementById("labFYNext").innerHTML = "Nächstes Gleis";
  document.getElementById("labFYPrev").innerHTML = "Vorheriges Gleis";
  document.getElementById("labStageClose").innerHTML = "Schliessen";
  document.getElementById("labStageOpen").innerHTML = "Öffnen";
  document.getElementById("labStageCompress").innerHTML = "Komprimieren";
}

function langEN() {
  var donkey = "<br>Rocrail runs entirely on volunteer labor. However, Rocrail also needs contributions of money. Your continued support is vital for keeping Rocrail available. If you already did support you can ask a key to disable this on startup dialog: donate@rocrail.net<br>";
  donkey += "<div id=\"donkeyWarning\"><b>Rocweb will stop communicate in 5 minutes!</b></div>";
  document.getElementById("donkey").innerHTML = donkey;

  document.getElementById("menuInfo").innerHTML = "Information";
  document.getElementById("menuSystem").innerHTML = "System";
  document.getElementById("menuOptions").innerHTML = "Options";
  document.getElementById("systemTitle").innerHTML = "System";
  document.getElementById("systemInitField").innerHTML = "Init field";
  document.getElementById("systemQuerySensors").innerHTML = "Start of day";
  document.getElementById("systemEmergencyBreak").innerHTML = "Emergency break";
  document.getElementById("optionsTitle").innerHTML = "Options";
  document.getElementById("labOptionDebug").innerHTML = "Debug";
  document.getElementById("labOptionSimSensors").innerHTML = "Simulate sensors";
  document.getElementById("labLocoCatEngine").innerHTML = "Engine";
  document.getElementById("labLocoCatEra").innerHTML = "Era";
  document.getElementById("labLocoCatRoadname").innerHTML = "Roadname";
  document.getElementById("labBlockStart").innerHTML = "Start train";
  document.getElementById("labBlockStop").innerHTML = "Stop train";
  document.getElementById("labBlockClose").innerHTML = "Close";
  document.getElementById("labBlockOpen").innerHTML = "Open";
  document.getElementById("labTTNext").innerHTML = "Next track";
  document.getElementById("labTTPrev").innerHTML = "Previous track";
  document.getElementById("labFYNext").innerHTML = "Next track";
  document.getElementById("labFYPrev").innerHTML = "Previous track";
  document.getElementById("labStageClose").innerHTML = "Close";
  document.getElementById("labStageOpen").innerHTML = "Open";
  document.getElementById("labStageCompress").innerHTML = "Compress";
}

function langNL() {
  var donkey = "<br>Rocrail wordt uitsluitend door vrijwilligers ontwikkeld. Om Rocrail “in de lucht te houden” en steeds weer aan te kunnen passen aan nieuwe modelspoor hardware is er jaarlijks een bescheiden financieel budget nodig. Vrijwillige bijdragen zijn daarvoor noodzakelijk. Als je al een financiële ondersteuning gegeven hebt kun je op donate@rocrail.net een sleutel aanvragen om dit venster te onderdrukken.<br>";
  donkey += "<div id=\"donkeyWarning\"><b>Rocweb stopt met communicatie na 5 minuten!</b></div>";
  document.getElementById("donkey").innerHTML = donkey;
  
  document.getElementById("menuInfo").innerHTML = "Informatie";
  document.getElementById("menuSystem").innerHTML = "Systeem";
  document.getElementById("menuOptions").innerHTML = "Opties";
  document.getElementById("systemTitle").innerHTML = "Systeem";
  document.getElementById("systemInitField").innerHTML = "Init veld";
  document.getElementById("systemQuerySensors").innerHTML = "Begin van de dag";
  document.getElementById("systemEmergencyBreak").innerHTML = "Noodstop";
  document.getElementById("optionsTitle").innerHTML = "Opties";
  document.getElementById("labOptionDebug").innerHTML = "Debug";
  document.getElementById("labOptionSimSensors").innerHTML = "Melders simuleren";
  document.getElementById("labLocoCatEngine").innerHTML = "Aandrijving";
  document.getElementById("labLocoCatEra").innerHTML = "Periode";
  document.getElementById("labLocoCatRoadname").innerHTML = "Maatschappij";
  document.getElementById("labBlockStart").innerHTML = "Start trein";
  document.getElementById("labBlockStop").innerHTML = "Stop trein";
  document.getElementById("labBlockClose").innerHTML = "Sluiten";
  document.getElementById("labBlockOpen").innerHTML = "Openen";
  document.getElementById("labTTNext").innerHTML = "Volgende spoor";
  document.getElementById("labTTPrev").innerHTML = "Vorige spoor";
  document.getElementById("labFYNext").innerHTML = "Volgende spoor";
  document.getElementById("labFYPrev").innerHTML = "Vorige spoor";
  document.getElementById("labStageClose").innerHTML = "Sluiten";
  document.getElementById("labStageOpen").innerHTML = "Openen";
  document.getElementById("labStageCompress").innerHTML = "Comprimeer";
}


/* Info Dialog */
function openInfo()
{
  trace("close menu");
  $( "#popupMenu" ).popup( "close" );
  
  trace("open info");
  $('#popupMenu').on("popupafterclose", function(){$( "#popupInfo" ).popup( "open" )});

//  trace("open info");
//  $( "#popupInfo" ).popup( "open" );
}

function onLocoImage() {
  trace("close throttle");
  $( "#popupThrottle" ).popup( "close" );
  trace("open loco select");
  $('#popupThrottle').on("popupafterclose", function(){
    $('#popupThrottle').unbind( "popupafterclose" );
    $( "#popupLocoSelect" ).popup( "open" );
    });
}

function onLocoSelected(sel) {
  console.log("selected loco: " + sel);
  
  locoSelected = sel;
  localStorage.setItem("locoSelected", locoSelected);
  initThrottle();
  updateDir();
  updateFunctionLabels();

  
  trace("close loco select");
  $( "#popupLocoSelect" ).popup( "close" );
  trace("open throttle");
  $('#popupLocoSelect').on("popupafterclose", function(){
    $('#popupLocoSelect').unbind( "popupafterclose" );
    $( "#popupThrottle" ).popup( "open" );
    });
}


function openDonkey()
{
  trace("open info");
  $( "#popupInfo" ).popup( "open" );
}


/* Throttle Dialog */
function openThrottle()
{
  /*
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
  */
  initThrottle();
  updateFunctionLabels();
  $( "#popupThrottle" ).popup( "open" );
}

function updateFunctionLabels() {
  var lights = document.getElementById("F0");
  lights.innerHTML = "F0"
  for(i = 1; i < 15; i++) {
    var F = document.getElementById("F"+i);
    F.innerHTML = "F" + (i + FGroup * 14); 
    F.style.backgroundColor = '';
  }

  lc = lcMap[locoSelected];
  if(lc == undefined)
    lc = carMap[locoSelected];
  if( lc != undefined ) {
    //var fundeflist = lc.childNodes;
    var fundeflist = lc.getElementsByTagName("fundef");
    trace("function defs " + fundeflist.length + " for " + lc.getAttribute('id'));

    var fx = parseInt(lc.getAttribute('fx'));
    if( fundeflist.length > 0 ) {
      for( n = 0; n < fundeflist.length; n++ ) {
        var fn = fundeflist[n].getAttribute('fn');
        var iFn = parseInt(fn);
        trace("fundef " + fn + " text: " + fundeflist[n].getAttribute('text'));
        if( FGroup == 0 && iFn > 14 ) {
          continue;
        }
        if( FGroup == 1 && iFn > 14 ) {
          iFn -= 14;
        }
        var F = document.getElementById("F"+iFn);
        if( fundeflist[n].getAttribute('icon') ) {
          F.innerHTML = "<img src='"+fundeflist[n].getAttribute('icon')+"'/>";
        }
        else if( fundeflist[n].getAttribute('text') ) {
          F.innerHTML = "<label style='font-size:10px'>" +fundeflist[n].getAttribute('text')+ "</label>";;
        }
        var mask = 1 << iFn; 
        if( fx & mask )
          F.style.backgroundColor = "#FF8888";

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


function openSystem() {
  trace("close menu");
  $( "#popupMenu" ).popup( "close" );
  $('#popupMenu').on("popupafterclose", function(){$( "#popupSystem" ).popup( "open" )});
}

/* Options Dialog */
function openOptions() {
  trace("close menu");
  $( "#popupMenu" ).popup( "close" );
  
  trace("open info");
  var debug = localStorage.getItem("debug");
  $('#optionDebug').prop('checked', debug=="true"?true:false).checkboxradio('refresh');
  var simsensors = localStorage.getItem("simsensors");
  $('#optionSimSensors').prop('checked', simsensors=="true"?true:false).checkboxradio('refresh');

  var category = localStorage.getItem("category");
  
  if( category == "era" )
    $("#locoCatEra").prop("checked", true).checkboxradio('refresh');
  else if( category == "roadname" )
    $("#locoCatRoadname").prop("checked", true).checkboxradio('refresh');
  else
    $("#locoCatEngine").prop("checked", true).checkboxradio('refresh');
  
  var select = document.getElementById("languageSelect");


  $('#popupMenu').on("popupafterclose", function(){$( "#popupOptions" ).popup( "open" )});
}

function onCatEngine() {
  localStorage.setItem("category", "engine");
}

function onCatEra() {
  localStorage.setItem("category", "era");
}

function onCatRoadname() {
  localStorage.setItem("category", "roadname");
}

function closeOptions()
{
}


/* Send a XML Http request */
function sendCommand(cmd) {
  // send an XMLHttpRequest
  trace("send command: " + cmd);
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
      console.log("exception: " + e.stack);
  }
}


/* Throttle commands */
$(function(){
  $("#F1").bind("taphold", tapholdF1Handler);
 
  function tapholdF1Handler(e) {
    tapholdF1 = 1;
    trace("taphold F1");
  }
});

function onDirection() {
  lc = lcMap[locoSelected];
  if( lc == undefined )
    lc = carMap[locoSelected];
  if( lc == undefined )
    return;
  var V = lc.getAttribute('V');
  var dir = lc.getAttribute('dir');
  if( dir == 'true' )
    lc.setAttribute('dir', 'false');
  else
    lc.setAttribute('dir', 'true');
  updateDir();
  if( V == undefined )
    V = "0";
  speedUpdate(parseInt(V));
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
  if(lc == undefined)
    lc = carMap[locoSelected];
  if( lc == undefined ) return;
  if( FGroup == 1 )
    nr += 14;
  trace("Funtion: " + id + " ("+nr+") for loco " + locoSelected);
  var group = (nr-1)/4+1;
  var fx = parseInt(lc.getAttribute('fx'));
  var mask = 1 << nr;
  var on = fx&mask?"false":"true";
  var cmd = "<fn id=\""+locoSelected+"\" fnchanged=\""+nr+"\" group=\""+group+"\" f"+nr+"=\""+on+"\"/>"
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function speedUpdate(value) {
  lc = lcMap[locoSelected];
  if( lc == undefined )
    lc = carMap[locoSelected];
  if( lc == undefined ) return;
  trace("Speed: " + value + " for loco " + locoSelected);
  var vVal = value * (parseInt(lc.getAttribute('V_max')/100.00));
  lc.setAttribute('V', vVal);
  var cmd = "<lc throttleid=\"rocweb\" id=\""+locoSelected+"\" V=\""+vVal+"\" dir=\""+lc.getAttribute('dir')+"\"/>";
  updateDir();
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
  trace("auto action " + auto);
  if( autoMode == "on" )
    auto = "off";
  var cmd = "<auto cmd=\""+auto+"\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function actionEBreak() {
  trace("emergancy break");
  var cmd = "<sys cmd=\"ebreak\" informall=\"true\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function actionSystemInitfield() {
  $( "#popupSystem" ).popup( "close" );
  var cmd = "<model cmd=\"initfield\" informall=\"true\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function actionSystemQuerySensors() {
  $( "#popupSystem" ).popup( "close" );
  var cmd = "<sys cmd=\"sod\" informall=\"true\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function actionSensor(id)
{
  var simsensors = localStorage.getItem("simsensors");
  if( simsensors == undefined )
    simsensors = "false";
  if( simsensors == "false" )
    return;

  fbid = id.replace("fb_","");
  trace("sensor action on " + fbid );
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
  trace("switch action on " + swid + " state=" + sw.getAttribute('state'));
  var cmd = "<sw cmd=\"flip\" id=\""+swid+"\" manualcmd=\"true\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function actionOutput(id) {
  coid = id.replace("co_","");
  co = coMap[coid];
  trace("output action on " + coid + " state=" + co.getAttribute('state'));
  var cmd = "<co cmd=\"flip\" id=\""+coid+"\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function actionSignal(id) {
  sgid = id.replace("sg_","");
  sg = sgMap[sgid];
  trace("signal action on " + sgid + " state=" + sg.getAttribute('state'));
  var cmd = "<sg cmd=\"flip\" id=\""+sgid+"\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function actionTurntable(id) {
  ttid = id.replace("tt_","");
  sessionStorage.setItem("turntable", ttid);
  document.getElementById("turntableTitle").innerHTML = ttid;

  ttNode = ttMap[ttid];
  $( "#popupTurntable" ).popup( "open", {positionTo: '#'+id} );

}

function actionFiddleYard(id) {
  fyid = id.replace("fy_","");
  sessionStorage.setItem("fiddleyard", fyid);
  document.getElementById("fiddleyardTitle").innerHTML = fyid;

  fyNode = fyMap[fyid];
  $( "#popupFiddleYard" ).popup( "open", {positionTo: '#'+id} );

}

function actionBlock(id) {
  bkid = id.replace("bk_","");
  sessionStorage.setItem("block", bkid);
  document.getElementById("blockTitle").innerHTML = bkid;

  bkNode = bkMap[bkid];

  var select = document.getElementById("locoBlockSelect");
  while(select.options.length > 0) {
    select.remove(0);
  }

  option = document.createElement( 'option' );
  option.value = "";
  option.innerHTML = "";
  select.add( option );

  var selected = false;
  
  for (var i in lcMap){
    var lc = lcMap[i];
    option = document.createElement( 'option' );
    option.value = lc.getAttribute('id');
    option.innerHTML = lc.getAttribute('id');
    select.add( option );
    if( bkNode.getAttribute('locid') == lc.getAttribute('id')) {
      select.selectedIndex = i+1;
      option.selected = 'selected';
      select.value = lc.getAttribute('id'); 
      selected = true;
      locoBlockSelect = lc.getAttribute('id');
    }
  }
  
  if( !selected ) {
    select.selectedIndex = 0;
    option.selected = 'selected';
    select.value = ""; 
    locoBlockSelect = 'none';
  }
  
  sessionStorage.setItem("locoBlockSelect", locoBlockSelect);

  $('#locoBlockSelect').selectmenu("refresh");

  var xPos = parseInt(bkNode.getAttribute('x')) * 32;
  var yPos = parseInt(bkNode.getAttribute('y')) * 32;
  //$( "#popupBlock" ).popup( "open" ).position({x: xPos, y: yPos, positionTo: window});
  $( "#popupBlock" ).popup( "open", {positionTo: '#'+id} );

}

function onBlockStart() {
  $( "#popupBlock" ).popup( "close" );
  locoBlockSelect = sessionStorage.getItem("locoBlockSelect");
  if( locoBlockSelect != "none" ) {
    var cmd = "<lc id=\""+locoBlockSelect+"\" cmd=\"go\"/>";
    worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
  }
}

function onBlockStop() {
  $( "#popupBlock" ).popup( "close" );
  locoBlockSelect = sessionStorage.getItem("locoBlockSelect");
  if( locoBlockSelect != "none" ) {
    var cmd = "<lc id=\""+locoBlockSelect+"\" cmd=\"stop\"/>";
    worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
  }
}

function onBlockOpen() {
  $( "#popupBlock" ).popup( "close" );
  bkid = sessionStorage.getItem("block");
  var cmd = "<bk id=\""+bkid+"\" state=\"open\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function onBlockClose() {
  $( "#popupBlock" ).popup( "close" );
  bkid = sessionStorage.getItem("block");
  var cmd = "<bk id=\""+bkid+"\" state=\"closed\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function onLocoInBlock(lcid) {
  bkid = sessionStorage.getItem("block");
  var cmd = "";
  if( lcid.length > 0 )
    cmd = "<lc id=\""+lcid+"\" cmd=\"block\" blockid=\""+bkid+"\"/>";
  else
    cmd = "<bk id=\""+bkid+"\" cmd=\"loc\" locid=\"\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}


function actionStageBlock(id)
{
  sbid = id.replace("sb_","");
  sessionStorage.setItem("stageblock", sbid);
  document.getElementById("stageblockTitle").innerHTML = sbid;
  $( "#popupStageBlock" ).popup( "open", {positionTo: '#'+id} );
}

function onStageCompress() {
  $( "#popupStageBlock" ).popup( "close" );
  sbid = sessionStorage.getItem("stageblock");
  var cmd = "<sb id=\""+sbid+"\" cmd=\"compress\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}


function onStageOpen() {
  $( "#popupStageBlock" ).popup( "close" );
  sbid = sessionStorage.getItem("stageblock");
  var cmd = "<sb id=\""+sbid+"\" state=\"open\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}


function onStageClose() {
  $( "#popupStageBlock" ).popup( "close" );
  sbid = sessionStorage.getItem("stageblock");
  var cmd = "<sb id=\""+sbid+"\" state=\"closed\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}


function onTurntableNext() {
  $( "#popupTurntable" ).popup( "close" );
  ttid = sessionStorage.getItem("turntable");
  var cmd = "<tt id=\""+ttid+"\" cmd=\"next\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}


function onTurntablePrevious() {
  $( "#popupTurntable" ).popup( "close" );
  ttid = sessionStorage.getItem("turntable");
  var cmd = "<tt id=\""+ttid+"\" cmd=\"previous\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}


function onFiddleYardNext() {
  $( "#popupFiddleYard" ).popup( "close" );
  fyid = sessionStorage.getItem("fiddleyard");
  var cmd = "<seltab id=\""+fyid+"\" cmd=\"next\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}


function onFiddleYardPrevious() {
  $( "#popupFiddleYard" ).popup( "close" );
  fyid = sessionStorage.getItem("fiddleyard");
  var cmd = "<seltab id=\""+fyid+"\" cmd=\"previous\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function onOptionDebug() {
  var optionDebug = document.getElementById("optionDebug");
  localStorage.setItem("debug", optionDebug.checked ? "true":"false");
  trace("option debug = "+ optionDebug.checked );
}

function onOptionSimSensors() {
  var optionSimSensors = document.getElementById("optionSimSensors");
  localStorage.setItem("simsensors", optionSimSensors.checked ? "true":"false");
  trace("option simsensors = "+ optionSimSensors.checked );
}

function initThrottleStatus() {
  trace("init throttle status: " + locoSelected );
  var lc = lcMap[locoSelected]
  if( lc == undefined )
    lc = carMap[locoSelected];
  if( lc != undefined ) {
    var locoStatus = document.getElementById("locoStatus");
    var locoDescription = document.getElementById("locoDescription");

    var modeLabel = "";
    var modeColor = "#FFAAAA";
    var mode = lc.getAttribute('mode');
    if( mode == "auto" ) {
      modeLabel = "A";
      modeColor = "#AAFFAA";
    }
    else if( mode == "idle" )
      modeLabel = "O";
    else if( mode == "wait" )
      modeLabel = "W";
    else if( mode == "halfauto" ) {
      modeLabel = "H";
      modeColor = "#AAFFAA";
    }
    
    var destblockid = lc.getAttribute('destblockid');
    var blockid     = lc.getAttribute('blockid');

    var fromTo = "";
    if( blockid != undefined && blockid.length > 0 && destblockid != undefined && destblockid.length > 0 )
      fromTo = " " + blockid + " >> " + destblockid;

    locoStatus.style.backgroundColor = modeColor;
    locoStatus.innerHTML = lc.getAttribute('id') + " [" + mode + "]" + fromTo;
    locoDescription.innerHTML = lc.getAttribute('desc');
    trace("init throttle status: " + lc.getAttribute('id') + " [" + mode + "]" + fromTo);
  }
}


function initThrottle() {
  trace("locoSelect: " + locoSelected );
  var lc = lcMap[locoSelected]
  if( lc == undefined )
    lc = carMap[locoSelected];
  if( lc != undefined ) {
    var img = document.getElementById("locoImage");
    var src = lc.getAttribute('image');
    if( src == undefined || src.length == 0 )
      img.src = "noimg.png";
    else
      img.src = lc.getAttribute('image');
    trace("new image: " + img.src);

    document.getElementById("speedSlider").value = lc.getAttribute('V');
    $("#speedSlider").slider("refresh");
    updateDir();
    initThrottleStatus();
  }
}


/* Initial functions */
$(document).on("pagecreate",function(){

  // jQuery events go here...
  $("#speedSlider").on( "slidestop", function( event, ui ) {
  value = this.value;
  speedUpdate(value);} );
  
  $('#locoBlockSelect').change(function() {
    $( "#popupBlock" ).popup( "close" );
    locoBlockSelect = this.value;
    trace("locoBlockSelect: " + locoBlockSelect);
    sessionStorage.setItem("locoBlockSelect", locoBlockSelect);
    onLocoInBlock(locoBlockSelect);
  } );
  
  /*
  $('#locoSelect').change(function() {
    locoSelected = this.value;
    localStorage.setItem("locoSelected", locoSelected);
    initThrottle();
    updateDir();
    updateFunctionLabels();
  } );
  */
  
  $('#languageSelect').change(function() {
    trace("languageSelect: " + this.value );
    localStorage.lang = this.value;

    if( this.value == "de" )
      langDE();
    else if( this.value == "en" )
      langEN();
    else if( this.value == "nl" )
      langNL();
  } );

});

$(document).ready(function(){
  trace("document ready");
  //$('.ui-slider-handle').height(50)
  //$('.ui-slider').input("none");
  //$('.ui-slider-track').marginLeft("15px");

  var lang = localStorage.lang;
  var sel = document.getElementById('languageSelect');
  
  if( lang == "de" ) {
    langDE();
    sel.selectedIndex = 0;
  }
  else if( lang == "en" ) {
    langEN();
    sel.selectedIndex = 1;
  }
  else if( lang == "nl" ) {
    langNL();
    sel.selectedIndex = 2;
  }
  $('#languageSelect').selectmenu("refresh");
})


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
   console.log("exception: " + e.stack);
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
  trace("sensor event: " + fb.getAttribute('id') + " " + fb.getAttribute('state'));
  var div = document.getElementById("fb_"+fb.getAttribute('id'));
  if( div != null ) {
    fbNode = fbMap[fb.getAttribute('id')];
    fbNode.setAttribute('state', fb.getAttribute('state'));
    div.style.backgroundImage = getSensorImage(fbNode);
    forceRedraw(div);
  }
  else {
    trace("sensor: " + fb.getAttribute('id') + " not found");
  }
}


function getTextFormat(tx) {
  var text = tx.getAttribute('text');
  var prefix = "";
  var suffix = "";
  var bold = tx.getAttribute('bold');
  var underlined = tx.getAttribute('underlined');
  var italic = tx.getAttribute('italic');

  if( bold != undefined && bold == "true") {
    prefix += "<b>";
    suffix += "</b>";
  }
  if( underlined != undefined && underlined == "true") {
    prefix += "<u>";
    suffix += "</u>";
  }
  if( italic != undefined && italic == "true") {
    prefix += "<i>";
    suffix += "</i>";
  }
  return (prefix+text+suffix);
}

function handleText(tx) {
  trace("text event: " + tx.getAttribute('id') + " " + tx.getAttribute('text'));
  var div = document.getElementById("tx_"+tx.getAttribute('id'));
  if( div != null ) {
    var text = tx.getAttribute('text');
    var txNode = txMap[tx.getAttribute('id')];
    
    var ori = getOri(txNode);
    txNode.setAttribute('text', text);

    if( text != undefined && text.indexOf(".png") != -1 ) {
      if( ori == "north" || ori == "south" )
        div.innerHTML = "<div class='imageV'><img height='"+div.style.width+"' src='"+text+"'/></div>";
      else
        div.innerHTML = "<div><img width='"+div.style.width+"' src='"+text+"'/></div>";
    }
    else {  
      var pointsize = tx.getAttribute('pointsize');
      if( pointsize == undefined || pointsize == "0")
        pointsize = "10";
      
      if( ori == "north" || ori == "south" )
        div.innerHTML = "<div class='itemtextV' style='font-size:"+pointsize+"px;'>"+getTextFormat(tx)+"</div>";
      else
        div.innerHTML = "<div style='font-size:"+pointsize+"px; horizontal-align:left;'>" +getTextFormat(tx)+ "</div>";
    }

    forceRedraw(div);
  }
}

function handleOutput(co) {
  trace("output event: " + co.getAttribute('id') + " " + co.getAttribute('state'));
  var div = document.getElementById("co_"+co.getAttribute('id'));
  if( div != null ) {
    coNode = coMap[co.getAttribute('id')];
    coNode.setAttribute('state', co.getAttribute('state'));
    div.style.backgroundImage = getOutputImage(coNode);
    forceRedraw(div);
  }
  else {
    trace("output: " + co.getAttribute('id') + " not found");
  }
}


function handleSwitch(sw) {
  trace("switch event: " + sw.getAttribute('id') + " " + sw.getAttribute('state'));
  var div = document.getElementById("sw_"+sw.getAttribute('id'));
  if( div != null ) {
    swNode = swMap[sw.getAttribute('id')];
    swNode.setAttribute('state', sw.getAttribute('state'));
    div.style.backgroundImage = getSwitchImage(swNode, div);
    forceRedraw(div);
  }
  else {
    trace("switch: " + sw.getAttribute('id') + " not found");
  }
}


function handleSignal(sg) {
  trace("signal event: " + sg.getAttribute('id') + " " + sg.getAttribute('state'));
  var div = document.getElementById("sg_"+sg.getAttribute('id'));
  if( div != null ) {
    sgNode = sgMap[sg.getAttribute('id')];
    sgNode.setAttribute('state', sg.getAttribute('state'));
    sgNode.setAttribute('aspect', sg.getAttribute('aspect'));
    div.style.backgroundImage = getSignalImage(sgNode, div);
    forceRedraw(div);
  }
  else {
    trace("signal: " + sg.getAttribute('id') + " not found");
  }
}

function findBlock4Loco(lcid) {
  for (var key in bkMap) {
    var bk = bkMap[key];
    if( lcid == bk.getAttribute('locid'))
      return bk;
  }
}

function handleLoco(lc) {
  var lcNode = lcMap[lc.getAttribute('id')]
  if( lc == undefined)
    lc = carMap[fn.getAttribute('id')];

  
  lcNode.setAttribute('mode', lc.getAttribute('mode'));
  lcNode.setAttribute('modereason', lc.getAttribute('modereason'));
  lcNode.setAttribute('V', lc.getAttribute('V'));
  lcNode.setAttribute('destblockid', lc.getAttribute('destblockid'));
  lcNode.setAttribute('blockid', lc.getAttribute('blockid'));

  var bk = findBlock4Loco(lc.getAttribute('id'));
  if( bk != undefined )
    updateBlockstate(bk.getAttribute('statesignal'), lc.getAttribute('id'));
  
  if( lc.getAttribute('id') == locoSelected )
    initThrottleStatus();
}


function handleFunction(fn) {
  trace("function event: " + fn.getAttribute('id') + " changed=" + fn.getAttribute('fnchanged'));
  var lc = lcMap[fn.getAttribute('id')];
  if( lc == undefined)
    lc = carMap[fn.getAttribute('id')];
  
  if( lc == undefined)
    return;
  
  var fnchanged = parseInt(fn.getAttribute('fnchanged'));
  var fx = parseInt(lc.getAttribute('fx'));
  var mask = 1 << fnchanged; 
  var on = fn.getAttribute("f"+fnchanged);
  
  if( on == "true")
    fx = fx | mask;
  else
    fx = fx & ~mask;
  lc.setAttribute('fx', ""+fx)
  
  if( fn.getAttribute('id') == locoSelected ) {
    if(fnchanged > 14)
      fnchanged -= 14;
    var div = document.getElementById("F"+fnchanged);
    if( on == "true")
      div.style.backgroundColor = "#FF8888";
    else
      div.style.backgroundColor = ''
  }
}

function updateBlockstate( sgid, lcid ) {
  sg = sgMap[sgid];
  lc = lcMap[lcid];
  if( sg == undefined )
    return;

  var div = document.getElementById("sg_"+sgid);
  var label = "-";
  if( lc != undefined ) {
    var mode = lc.getAttribute('mode');
    if( mode == "auto" )
      label = "A";
    else if( mode == "idle" )
      label = "O";
    else if( mode == "wait" )
      label = "W";
    else if( mode == "halfauto" )
      label = "H";
  }
  div.innerHTML = "<label class='itemtext'>"+label+"</label>";
  forceRedraw(div);
}

function handleBlock(bk) {
  trace("block event: " + bk.getAttribute('id') + " " + bk.getAttribute('state'));
  var div = document.getElementById("bk_"+bk.getAttribute('id'));
  if( div != null ) {
    bkNode = bkMap[bk.getAttribute('id')];
    bkNode.setAttribute('state', bk.getAttribute('state'));
    bkNode.setAttribute('locid', bk.getAttribute('locid'));
    bkNode.setAttribute('reserved', bk.getAttribute('reserved'));
    bkNode.setAttribute('entering', bk.getAttribute('entering'));
    
    var ori   = getOri(bkNode);
    var label = bk.getAttribute('locid');
    if( label == undefined || label.length == 0 )
      label = bk.getAttribute('id');
    if( ori == "north" || ori == "south" )
      div.innerHTML      = "<div class='itemtextV' style='font-size:"+blockPointsize+"px;'>"+label+"</div>";
    else
      div.innerHTML      = "<label class='itemtext' style='font-size:"+blockPointsize+"px;'>"+label+"</label>";

    div.style.backgroundImage = getBlockImage(bkNode, div);
    forceRedraw(div);
  }
  else {
    trace("block: " + bk.getAttribute('id') + " not found");
  }
  
  updateBlockstate( bk.getAttribute('statesignal'), bk.getAttribute('locid'));
}


function handleTurntable(tt) {
  trace("turntable event: " + tt.getAttribute('id') + " " + tt.getAttribute('state'));
  var div = document.getElementById("tt_"+tt.getAttribute('id'));
  if( div != null ) {
    ttNode = ttMap[tt.getAttribute('id')];
    ttNode.setAttribute('bridgepos', tt.getAttribute('bridgepos'));
    ttNode.setAttribute('state', tt.getAttribute('state'));
    ttNode.setAttribute('state1', tt.getAttribute('state1'));
    ttNode.setAttribute('state2', tt.getAttribute('state2'));
    
    div.innerHTML = getTurntableImage(ttNode, div);
    forceRedraw(div);
  }
  else {
    trace("turntable: " + tt.getAttribute('id') + " not found");
  }
  
}


function handleFiddleYard(fy) {
  trace("fiddleyard event: " + fy.getAttribute('id') + " " + fy.getAttribute('state'));
  var div = document.getElementById("fy_"+fy.getAttribute('id'));
  if( div != null ) {
    fyNode = fyMap[fy.getAttribute('id')];
    fyNode.setAttribute('locid', fy.getAttribute('locid'));
    fyNode.setAttribute('state', fy.getAttribute('state'));
    
    div.innerHTML = getFiddleYardImage(fyNode, div);
    forceRedraw(div);
  }
  else {
    trace("fiddleyard: " + fy.getAttribute('id') + " not found");
  }
  
}


function handleStageBlock(sb) {
  trace("staging block event: " + sb.getAttribute('id') + " " + sb.getAttribute('state'));
  var div = document.getElementById("sb_"+sb.getAttribute('id'));
  if( div != null ) {
    sbNode = sbMap[sb.getAttribute('id')];
    sbNode.setAttribute('state', sb.getAttribute('state'));
    sbNode.setAttribute('locid', sb.getAttribute('locid'));
    sbNode.setAttribute('reserved', sb.getAttribute('reserved'));
    sbNode.setAttribute('entering', sb.getAttribute('entering'));
    
    var lcCount = 0;
    sectionlist = sb.getElementsByTagName("section");
    if( sectionlist.length > 0 ) {
      trace("updating " + sectionlist.length + " staging block sections");
      for (var n = 0; n < sectionlist.length; n++) {
        var lcid = sectionlist[n].getAttribute('lcid');
        if( lcid != undefined && lcid.length > 0 ) {
          lcCount++;
        }
      }
    }

    var ori   = getOri(sb);
    var label = sb.getAttribute('locid');
    if( label == undefined || label.length == 0 )
      label = sb.getAttribute('id') + "[" + lcCount + "]";
    if( ori == "north" || ori == "south" )
      div.innerHTML      = "<div class='itemtextV' style='font-size:"+blockPointsize+"px;'>"+label+"</div>";
    else
      div.innerHTML      = "<label class='itemtext' style='font-size:"+blockPointsize+"px;'>"+label+"</label>";

    div.style.backgroundImage = getStageBlockImage(sbNode, div);
    forceRedraw(div);

  }
  else {
    trace("staging block: " + sb.getAttribute('id') + " not found");
  }
}

function handleRoute(st) {
  var stid = st.getAttribute('id');
  stNode = stMap[stid];
  if( stNode != undefined ) {
    var locked = st.getAttribute('status');
    if( locked != undefined ) {
      stNode.setAttribute('status', locked);
  
      for (var key in tkMap) {
        var tk = tkMap[key];
        var routeids = tk.getAttribute('routeids');
        if( routeids == undefined )
          continue;
        if( routeids.indexOf(stid) != -1 ) {
          tk.setAttribute('route', locked)
          handleTrack(tk);
        }
      }
      for (var key in fbMap) {
        var fb = fbMap[key];
        var routeids = fb.getAttribute('routeids');
        if( routeids == undefined )
          continue;
        if( routeids.indexOf(stid) != -1 ) {
          fb.setAttribute('route', locked)
          handleSensor(fb);
        }
      }
      for (var key in sgMap) {
        var sg = sgMap[key];
        var routeids = sg.getAttribute('routeids');
        if( routeids == undefined )
          continue;
        if( routeids.indexOf(stid) != -1 ) {
          sg.setAttribute('route', locked)
          handleSignal(sg);
        }
      }
    }

  }
}


function handleTrack(tk) {
  var div = document.getElementById("tk_"+tk.getAttribute('id'));
  if( div == null )
    return;
  div.style.backgroundImage = getTrackImage(tk);
  forceRedraw(div);
}


function handleState(state) {
  power = state.getAttribute('power');
  trace("power: " + power );
  if( power == "true" )
    document.getElementById("headerPower").style.backgroundColor= "#FF8888";
  else 
    document.getElementById("headerPower").style.backgroundColor= '';
}

function handleAuto(auto) {
  autoMode = auto.getAttribute('cmd');
  trace("auto: " + autoMode );
  if( autoMode == "on" )
    document.getElementById("headerAuto").style.backgroundColor= "#FF8888";
  else 
    document.getElementById("headerAuto").style.backgroundColor= '';
}

function handleSystem(sys) {
  //<sys cmd="shutdown" informall="true"/>
  var cmd = sys.getAttribute('cmd');
  if( cmd == "shutdown" ) {
    trace("server shutdown");
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
  trace("evaluate: " + evtName);
  if( evtName == "fb" )
    handleSensor(root);
  else if( evtName == "bk" )
    handleBlock(root);
  else if( evtName == "state" )
    handleState(root);
  else if( evtName == "auto" )
    handleAuto(root);
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
  else if( evtName == "tx" )
    handleText(root);
  else if( evtName == "sb" )
    handleStageBlock(root);
  else if( evtName == "tt" )
    handleTurntable(root);
  else if( evtName == "seltab" )
    handleFiddleYard(root);
  else if( evtName == "st" )
    handleRoute(root);
}

function processResponse() {
  trace("readyState="+req.readyState+" status="+req.status);
  if (req.readyState == 4 && (req.status == 0 || req.status == 200)) {
    // only if "OK"
    $.mobile.loading("show");
    try {
      xmlDoc = req.responseXML;
      if( xmlDoc != null ) {
        var category = localStorage.getItem("category");
        if(category == undefined || category.length == 0) {
          localStorage.setItem("category", "engine");
        }

        planlist = xmlDoc.getElementsByTagName("plan")
        if( planlist.length > 0 ) {
          var h = document.getElementById("title");
          donkey = planlist[0].getAttribute('donkey');
          title = planlist[0].getAttribute('title');
          rocrailversion = planlist[0].getAttribute('rocrailversion');
          rocrailpwd = planlist[0].getAttribute('rocrailpwd');
          
          var serverInfo = document.getElementById("serverInfo");
          serverInfo.innerHTML = "Server version: " + rocrailversion + "<br>" + "Server path: " + rocrailpwd;
          
          
          trace( "processing plan: " + title + " key=" + donkey );
          h.innerHTML = title;
          processPlan();
          planloaded = true;
          locoSelected = localStorage.getItem("locoSelected");
          trace("selected loco = " + locoSelected);

          if( donkey == 'true') {
            document.getElementById("donkey").style.display = 'none'
          }
          else {
            trace( "5 minutes before shutdown..." );
            var shutdownTimer = setInterval(function () {doShutdown()}, (5 * 60 * 1000) );
            function doShutdown() {
              trace("no key; shutdown...");
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
          trace( "processing event" );
          processUpdate(req);
        }
      }
      else {
        trace( "??? xmlDoc=" + xmlDoc);
      }

      if( planloaded ) {
        worker = new Worker("rocwebworker.js");
        worker.onmessage = function (e) {
          var result = JSON.parse(e.data);
          if(result.type == 'debug') {
            trace(result.msg);
          } 
          else if(result.type == 'alert') {
            alert(result.msg);
          } 
          else if(result.type == 'response') {
            trace("response: "+result.answer);
            if( !didShowDonkey && donkey == 'false' ) {
              openDonkey();
              didShowDonkey = true;
            }
            /* ToDo: Evaluate server event. */
            evaluateEvent(result.answer);
          }
          else if(result.type == 'command') {
            trace("command: "+result.msg);
            /* ToDo: Send to server. */
          }
          else {
            trace("data: " + e.data);
          }
        }
      }
     
    }
    catch(e) {
      console.log("exception: " + e.stack);
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
  var route = fb.getAttribute('route');
  var accnr = parseInt(fb.getAttribute('accnr'));
  var ori   = getOri(fb);
  var suffix = (route=="1")?"-route":"";

  if( "true" == fb.getAttribute('state') )
    if( accnr > 0 )
      return "url('accessory-"+accnr+"-on"+suffix+"."+ori+".svg')";
    else if( curve == "true" )
      return "url('curve-sensor-on"+suffix+"."+ori+".svg')";
    else
      return "url('sensor-on"+suffix+"."+ori+".svg')";
  else
    if( accnr > 0 )
      return "url('accessory-"+accnr+"-off"+suffix+"."+ori+".svg')";
    else if( curve == "true" )
      return "url('curve-sensor-off"+suffix+"."+ori+".svg')";
    else
      return "url('sensor-off"+suffix+"."+ori+".svg')";
}

function getOutputImage(co) {
  var ori   = getOri(co);
  var svg   = co.getAttribute('svgtype');
  var state = co.getAttribute('state');
  var suffix = '';
  if( state == undefined )
    state = "off";
  if( svg == undefined )
    svg = "0";
  return "url('button-"+svg+"-"+state+"."+ ori + ".svg')";
}

function getSignalImage(sg) {
  var ori     = getOri(sg);
  var state   = sg.getAttribute('state');
  var signal  = sg.getAttribute('signal');
  var type    = sg.getAttribute('type');
  var aspects = sg.getAttribute('aspects');
  var pattern = parseInt( sg.getAttribute('usepatterns') );
  var route   = sg.getAttribute('route');
  var suffix  = (route=="1")?"-route":"";

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
  
  if( type != "semaphore" )
    type = "signal";
  
  if( aspects == "2" )
    aspects = "-2";
  else
    aspects = "";
  
  trace("signal image: usepatterns="+pattern+" nr="+nr+" greennr="+greennr+" rednr="+rednr+" yellownr="+yellownr+" whitenr="+whitenr+" state="+state);
  var aspect  = "r";
  if( state == "red"    ) aspect = "r";
  if( state == "green"  ) aspect = "g";
  if( state == "yellow" ) aspect = "y";
  if( state == "white"  ) aspect = "w";
  
  if( signal == "blockstate" ) {
    return "url('blockstate"+"."+ ori + ".svg')";
  }
  if( signal == "distant" ) {
    return "url('"+type+"distant"+aspects+"-"+aspect+suffix+"."+ ori + ".svg')";
  }
  if( signal == "shunting" ) {
    return "url('"+type+"shunting-2-"+aspect+suffix+"."+ ori + ".svg')";
  }

  return "url('"+type+"main"+aspects+"-"+aspect+suffix+"."+ ori + ".svg')";
}


function getFiddleYardImage(fy, div) {
  var nrtracks = 0;
  if( fy.getAttribute('nrtracks') == undefined )
    nrtracks = 12;
  else
    nrtracks = parseInt(fy.getAttribute('nrtracks'));
  var ori      = getOri(fy);
  var width    = (nrtracks * 32) - 4;
  var symwidth = nrtracks * 32;
  var state    = fy.getAttribute('state');
  var rotate   = 0;

  var fill = "255,255,255";
  if( state == "closed" )
    fill = "100,100,100";
  
  trace("fy width="+width+" nrtracks="+nrtracks);

  if( ori == "north" || ori == "south" ) {
    div.style.width    = ""+(32)+"px";
    div.style.height   = ""+symwidth+"px";
    rotate = 90;
  }
  else {
    div.style.width    = ""+symwidth+"px";
    div.style.height   = ""+(32)+"px";
  }
  
  var label = fy.getAttribute('locid');
  if( label == undefined || label.length == 0 )
    label = fy.getAttribute('id');
  else
    fill = "255,100,100";

  var transform = "rotate("+rotate+", "+(32/2)+", "+(32/2)+")";


  var svg = 
    "<svg xmlns='http://www.w3.org/2000/svg' width='"+symwidth+"' height='"+(div.style.height)+"'>" +
    "  <g transform='"+transform+"'>" +
    "   <rect x='2' y='2' rx='8' ry='8' width='"+width+"' height='28' fill='rgb("+fill+")' stroke-width='2' stroke='rgb(0,0,0)'/>" +
    "   <text x='8' y='24' fill='black' font-size='"+blockPointsize+"px'>"+label+"</text>" +
    "  </g>" +
    "</svg>";
  
  return svg;
}


function getTraverserImage(tt, div) {
  var ori        = getOri(tt);
  var bridgepos  = parseInt(tt.getAttribute('bridgepos'));
  var yoff       = (bridgepos % 24) * 32;
  var rotate     = 0;
  var width      = 128;
  var height     = 256;
  var pathTransform = "";

  if( ori == "north" || ori == "south" ) {
    div.style.width    = "256px";
    div.style.height   = "128px";
    width  = 256;
    height = 128;
    rotate = 90;
    var pathTransform = " transform='translate(0, -128)'";
  }
  else {
    div.style.width    = "128px";
    div.style.height   = "256px";
  }
  
  var transform = "rotate("+rotate+", "+64+", "+64+")";


  var svg = 
    "<svg xmlns='http://www.w3.org/2000/svg' width='"+width+"' height='"+height+"'>" +
    "  <g transform='"+transform+"'>" +
    "    <path stroke='rgb(0,0,0)' fill='white' d='M 0,3 L 127,3 L 127,252 L 0,252 z ' "+pathTransform+"/>" +
    "    <path stroke='rgb(0,0,0)' fill='white' d='M 6,6 L 121,6 L 121,249 L 6,249 z ' "+pathTransform+"/>" +
    "    <path stroke='rgb(160,160,160)' fill='white' d='M 11,6 L 12,6 L 12,249 L 11,249 z ' "+pathTransform+"/>" +
    "    <path stroke='rgb(160,160,160)' fill='white' d='M 115,6 L 116,6 L 116,249 L 115,249 z ' "+pathTransform+"/>";
  
  // draw the bridge
  svg +=
    "<path stroke='rgb(0,0,0)' fill='gray' d='M 7,"+(7+yoff)+" L 17,"+(7+yoff)+" L 17,"+(24+yoff)+" L 7,"+(24+yoff)+" z ' "+pathTransform+"/>" +
    "<path stroke='rgb(0,0,0)' fill='gray' d='M 110,"+(7+yoff)+" L 120,"+(7+yoff)+" L 120,"+(24+yoff)+" L 110,"+(24+yoff)+" z ' "+pathTransform+"/>" +
    "<path stroke='rgb(0,0,0)' fill='none' d='M 0,"+(10+yoff)+" L 127,"+(10+yoff)+" L 127,"+(21+yoff)+" L 0,"+(21+yoff)+" z ' "+pathTransform+"/>" +
    "<path stroke='gray' fill='white' d='M 1,"+(11+yoff)+" L 126,"+(11+yoff)+" L 126,"+(20+yoff)+" L 1,"+(20+yoff)+" z ' "+pathTransform+"/>";

  
  svg +=
    "  </g>" +
    "</svg>";
  
  return svg;
}


function getTurntableImage(tt, div) {
  var traverser  = tt.getAttribute('traverser');
  if( traverser == "true" ) {
    return getTraverserImage(tt, div);
  }
  
  var symbolsize = parseInt(tt.getAttribute('symbolsize'));
  var bridgepos  = parseInt(tt.getAttribute('bridgepos'));
  if( symbolsize < 1 )
    symbolsize = 1;
  if( symbolsize > 13 )
    symbolsize = 13;
  
  var size   = symbolsize * 32;
  var center = (symbolsize * 32) / 2;
  var radius1 = center - 2;
  var radius2 = center / 2;
  var radius3 = radius2 - 6;
  var bridgeX = (size/2) - radius3; 
  var bridgeY = (size/2) - 5;
  var bridgeCX = radius3 * 2; 
  var bridgeCY = 10; 
  // M 2,2 L 126,2 L 126,62 L 2,62 z 
  var bridge = "M "+bridgeX+","+bridgeY+" L "+(bridgeX+bridgeCX)+","+bridgeY+" L "+
               (bridgeX+bridgeCX)+","+(bridgeY+bridgeCY)+" L"+bridgeX+","+(bridgeY+bridgeCY)+" z";
  var rotate = (360 / 48) * (48 - bridgepos);
  var transform = "rotate("+rotate+", "+center+", "+center+")";
  var svg = 
    "<svg xmlns='http://www.w3.org/2000/svg' width='"+size+"' height='"+size+"'>" +
    "  <g>" +
    "   <circle cx='"+center+"' cy='"+center+"' r='"+radius1+"' fill='rgb(255,255,255)' stroke-width='1' stroke='rgb(180,180,180)'/>";
  // draw tracks...
  var trackPath = "M "+center+","+center+" L "+(size-2)+","+center; 
  trackList = tt.getElementsByTagName("track");
  if( trackList.length > 0 ) {
    for( var i = 0; i < trackList.length; i++ ) {
      var track = trackList[i];
      var nr = parseInt(track.getAttribute('nr'));
      var trackRotate = (360 / 48) * (48 - nr);
      var trackTransform = "rotate("+trackRotate+", "+center+", "+center+")";
      var path = "";
      if( bridgepos == nr )
        path = "<path stroke-width='7' stroke='rgb(255,0,0)' fill='rgb(255,0,0)' d='"+trackPath+"' transform='"+trackTransform+"' />";
      else
        path = "<path stroke-width='7' stroke='rgb(100,100,100)' fill='rgb(100,100,100)' d='"+trackPath+"' transform='"+trackTransform+"' />";
      svg += path;
    }
  }

  svg +=   
    "   <circle cx='"+center+"' cy='"+center+"' r='"+radius2+"' fill='rgb(255,255,255)' stroke-width='2' stroke='rgb(0,0,0)'/>" +
    "   <circle cx='"+center+"' cy='"+center+"' r='"+radius3+"' fill='rgb(255,255,255)' stroke-width='2' stroke='rgb(0,0,0)'/>" +
    "   <path stroke-width='2' stroke='rgb(0,0,0)' fill='rgb(100,255,100)' d='"+bridge+"' transform='"+transform+"' />" +
    "  </g>" +
    "</svg>";
  return svg;
}

function getTrackImage(tk) {
  var ori   = getOri(tk);
  var type  = tk.getAttribute('type');
  var route = tk.getAttribute('route');
  var suffix = (route=="1")?"-route":"";
  if (type == "curve" )
    return "url('curve"+suffix+"."+ori+".svg')";
  else if( type == "buffer" || type == "connector" || type == "dcurve" || type == "dir" || type == "dirall" ) {
    return "url('"+type+suffix+"."+ori+".svg')";
  }
  else {
    return "url('straight"+suffix+"."+ ori + ".svg')";
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
  var suffix = "-route";

  trace("switch type: " + type + " accnr="+accnr);
  if( accnr != undefined && parseInt(accnr) > 1 )
    type = "accessory";
  
  if (type=="right") {
    if (state=="straight")
      return "url('turnoutright"+suffix+"."+ori+".svg')";
    else
      return "url('turnoutright-t"+suffix+"."+ori+".svg')";
  }
  else if (type=="left") {
    if (state=="straight")
      return "url('turnoutleft"+suffix+"."+ori+".svg')";
    else
      return "url('turnoutleft-t"+suffix+"."+ori+".svg')";
  }
  else if (type=="twoway") {
    if (state=="straight")
      return "url('twoway-tr"+suffix+"."+ori+".svg')";
    else
      return "url('twoway-tl"+suffix+"."+ori+".svg')";
  }
  else if (type=="threeway") {
    if (state=="left")
      return "url('threeway-tl"+suffix+"."+ori+".svg')";
    else if (state=="right")
      return "url('threeway-tr"+suffix+"."+ori+".svg')";
    else
      return "url('threeway"+suffix+"."+ori+".svg')";
  }
  else if (type=="crossing") {
    if( rectc == "true") {
      if (state=="straight")
        return "url('crossing"+suffix+"."+ori+".svg')";
      else
        return "url('crossing-t"+suffix+"."+ori+".svg')";
    }
    else {
      if( ori == "west" || ori == "east") {
        div.style.width    = "64px";
        div.style.height   = "32px";
      }
      else {
        div.style.width    = "32px";
        div.style.height   = "64px";
      }
      var direction = (dir == "true" ? "left":"right");
      if (state=="turnout")
        return "url('crossing"+direction+"-t"+suffix+"."+ori+".svg')";
      else
        return "url('crossing"+direction+suffix+"."+ori+".svg')";
    }
  }
  else if (type=="ccrossing") {
    if( ori == "west" || ori == "east") {
      div.style.width    = "64px";
      div.style.height   = "32px";
    }
    else {
      div.style.width    = "32px";
      div.style.height   = "64px";
    }
    return "url('ccrossing"+suffix+"."+ori+".svg')";
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
      return "url('dcrossing"+direction+"-tl"+suffix+"."+ori+".svg')";
    else if (state=="right")
      return "url('dcrossing"+direction+"-tr"+suffix+"."+ori+".svg')";
    else if (state=="turnout")
      return "url('dcrossing"+direction+"-t"+suffix+"."+ori+".svg')";
    else
      return "url('dcrossing"+direction+""+suffix+"."+ori+".svg')";
  }
  else if (type=="decoupler") {
    if (state=="straight")
      return "url('decoupler-on"+"."+ori+".svg')";
    else
      return "url('decoupler"+"."+ori+".svg')";
  }
  else if (type=="accessory") {
    if( accnr == "40" ) {
      if( ori == "west" || ori == "east") {
        div.style.width    = "128px";
        div.style.height   = "64px";
      }
      else {
        div.style.width    = "64px";
        div.style.height   = "128px";
      }
    }
    else if( accnr == "51" ) {
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
    else if( accnr == "53" ) {
      div.style.width    = "64px";
      div.style.height   = "64px";
    }
    else if( accnr == "54" ) {
      if( ori == "west" || ori == "east") {
        div.style.width    = "96px";
        div.style.height   = "64px";
      }
      else {
        div.style.width    = "64px";
        div.style.height   = "96px";
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
  var id    = bk.getAttribute('id');
  var label = bk.getAttribute('locid');
  var small = bk.getAttribute('smallsymbol');
  var suffix = "";

  if( ori == "north" || ori == "south" ) {
    div.style.width    = "32px";
    if( "true" == small ) {
      suffix = "-s";
      div.style.height   = "64px";
    }
    else
      div.style.height   = "128px";
  }
  else {
    if( "true" == small ) {
      suffix = "-s";
      div.style.width   = "64px";
    }
    else
      div.style.width    = "128px";
    div.style.height   = "32px";
  }

  if( "true" == bk.getAttribute('entering') )
    return "url('block-ent"+suffix+"."+ori+".svg')";
  else if( "closed" == bk.getAttribute('state') )
    return "url('block-closed"+suffix+"."+ori+".svg')";
  if( "true" == bk.getAttribute('reserved') )
    return "url('block-res"+suffix+"."+ori+".svg')";
  else if( label != undefined && label != "null" && label.length > 0 ) {
    trace("block " + id + " is locked by ["+label+"]");
    return "url('block-occ"+suffix+"."+ori+".svg')";
  }
  else
    return "url('block"+suffix+"."+ori+".svg')";
  
}

function getStageBlockImage(sb, div) {
  var ori   = getOri(sb);
  var label = sb.getAttribute('locid');
  var suffix = "";

  if( ori == "north" || ori == "south" ) {
    div.style.width    = "32px";
    div.style.height   = "128px";
  }
  else {
    div.style.width    = "128px";
    div.style.height   = "32px";
  }

  if( "true" == sb.getAttribute('entering') )
    return "url('stage-ent"+"."+ori+".svg')";
  else if( "closed" == sb.getAttribute('state') )
    return "url('stage-closed"+"."+ori+".svg')";
  if( "true" == sb.getAttribute('reserved') )
    return "url('stage-res"+"."+ori+".svg')";
  else if( label != undefined && label.length > 0 )
    return "url('stage-occ"+"."+ori+".svg')";
  else
    return "url('stage"+"."+ori+".svg')";
  
}

function addCatToList(div, lcCat) {
  var newdiv = document.createElement('div');
  newdiv.setAttribute('data-role', "collapsible");
  newdiv.setAttribute('class', "ui-collapsible ui-collapsible-inset ui-corner-all ui-collapsible-themed-content ui-collapsible-collapsed ui-first-child");
  var h2 = document.createElement('h2');
  h2.innerHTML = lcCat;
  newdiv.appendChild(h2);
  var ul = document.createElement('ul');
  ul.setAttribute('id', lcCat);
  ul.setAttribute('data-role', "listview");
  ul.setAttribute('class', "ui-listview");
  newdiv.appendChild(ul);
  div.appendChild(newdiv);
  cat = ul;
  lcCatMap[lcCat] = cat;
  return cat;
}

function addMobileToList(cat, id, image, dir, addr) {
  if( dir == undefined || dir.length == 0 )
    dir = "true";
  var li = document.createElement('li');
  li.setAttribute('onclick', "onLocoSelected('"+id+"')");
  li.setAttribute('class', "ui-first-child ui-last-child");
  var img = "<img src='noimg.png' width='120'/>";
  if( image != undefined && image.length > 0 )
    img = "<img src='"+image+"' width='120'/>";
  var dirImg = "<img src='fwd.png'/>";
  if( dir == "false" )
    dirImg = "<img src='rev.png'/>";
  var row = "<table><tr><td>"+img+"<td>"+dirImg+"<td>"+id+"<br>"+addr+"</table>";
  li.innerHTML = "<a href='#' class='ui-btn ui-btn-icon-right ui-icon-carat-r'>"+row+"</a>";
  cat.appendChild(li);
  $('div[data-role=collapsible]').collapsible({refresh:true});  
}

function addCarToList(car) {
  var addr = car.getAttribute('addr');
  if( addr == "0" )
    return;
  var category = localStorage.getItem("category");
  var div = document.getElementById("locoSelectList");
  
  var lcCat = "car";
  if( category != "engine" ) {
    lcCat = getMobileCategory(car);
  }

  var cat = lcCatMap[lcCat];
  if( cat == undefined ) {
    cat = addCatToList(div, lcCat);
  }
  addMobileToList(cat, car.getAttribute('id'), car.getAttribute('image'), car.getAttribute('dir'), car.getAttribute('addr'));
}

function getMobileCategory(lc) {
  var category = localStorage.getItem("category");
  var lcCat = lc.getAttribute('era');
  
  if( category == "roadname" ) {
    lcCat = lc.getAttribute('roadname');
    if( lcCat == undefined || lcCat.length == 0 )
      lcCat = "General";
  }
  else if( category == "era" ) {
    lcCat = lc.getAttribute('era');
    if( lcCat == undefined || lcCat.length == 0 )
      lcCat = "I";
    else {
      if(lcCat == "0") lcCat = "I";
      else if(lcCat == "1") lcCat = "II";
      else if(lcCat == "2") lcCat = "III";
      else if(lcCat == "3") lcCat = "IV";
      else if(lcCat == "4") lcCat = "V";
      else if(lcCat == "5") lcCat = "VI";
      else if(lcCat == "6") lcCat = "VII";
    }
  }
  return lcCat;
}

function addLocoToList(lc) {
  var category = localStorage.getItem("category");

  var div = document.getElementById("locoSelectList");
  var lcCat = "diesel";
  if( category == "engine" ) {
    lcCat = lc.getAttribute('engine');
    if( lcCat == undefined || lcCat.length == 0 )
      lcCat = "diesel";
  }
  else {
    lcCat = getMobileCategory(lc);
  }
  var cat = lcCatMap[lcCat];
  if( cat == undefined ) {
    cat = addCatToList(div, lcCat);
  }
  addMobileToList(cat, lc.getAttribute('id'), lc.getAttribute('image'), lc.getAttribute('dir'), lc.getAttribute('addr'));
}

/* Process the plan.xml */
function processPlan() {
//only if req shows "loaded"
   try {
     xmlDoc = req.responseXML;
     
     zlevellist = xmlDoc.getElementsByTagName("zlevel");
     if( zlevellist.length > 0 ) {
       var title = "";
       trace("processing " + zlevellist.length + " zlevels");
       for (var i = 0; i < zlevellist.length; i++) {
         zlevelMap[z] = zlevellist[i];
         var z = zlevellist[i].getAttribute('z');
         if( z == undefined )
           z = "0";
         trace('zlevel: ' + z + " title: " + zlevellist[i].getAttribute('title'));

         var newdiv = document.createElement('div');
         newdiv.setAttribute('id', "level_" + z);
         newdiv.setAttribute('overflow-x', "auto");
         newdiv.setAttribute('overflow-y', "auto");
         //newdiv.style.backgroundColor = "#FF9999";

         zlevelDivMap[z] = newdiv;
         zlevelDivList[i] = newdiv;

         if( zlevelSelected == 'none' ) {
           zlevelSelected = newdiv;
           zlevelIdx = i;
           title = zlevellist[i].getAttribute('title');
         }
         else {
           trace("disable level " + z);
           newdiv.style.display = 'none'
         }
         document.body.appendChild(newdiv);
       }
       var h = document.getElementById("title");
       h.innerHTML = title;
     }
     
     
     lclist = xmlDoc.getElementsByTagName("lc");
     if( lclist.length > 0 )
       trace("processing " + lclist.length + " locos");

     for (var i = 0; i < lclist.length; i++) {
       trace('loco: ' + lclist[i].getAttribute('id') );
       lcMap[lclist[i].getAttribute('id')] = lclist[i];
       addLocoToList(lclist[i]);
     }
     
     carlist = xmlDoc.getElementsByTagName("car");
     if( carlist.length > 0 )
       trace("processing " + carlist.length + " cars");

     for (var i = 0; i < carlist.length; i++) {
       trace('car: ' + carlist[i].getAttribute('id') );
       carMap[carlist[i].getAttribute('id')] = carlist[i];
       addCarToList(carlist[i]);
     }
     
     
     
     colist = xmlDoc.getElementsByTagName("co");
     if( colist.length > 0 )
       trace("processing " + colist.length + " outputs");

     for (var i = 0; i < colist.length; i++) {
       var z     = colist[i].getAttribute('z');
       var ori   = getOriNr(colist[i].getAttribute('ori'));
       var leveldiv = zlevelDivMap[z]; 
       trace('output: ' + colist[i].getAttribute('id') + "at level " + z);
       if( leveldiv == undefined ) {
         trace("Error: zlevel ["+z+"] does not exist!");
         continue;
       }
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
       trace("Output image="+newdiv.style.backgroundImage);

       leveldiv.appendChild(newdiv);
     }
     
     
     sglist = xmlDoc.getElementsByTagName("sg");
     if( sglist.length > 0 )
       trace("processing " + sglist.length + " signals");

     for (var i = 0; i < sglist.length; i++) {
       var z     = sglist[i].getAttribute('z');
       var ori   = getOriNr(sglist[i].getAttribute('ori'));
       var leveldiv = zlevelDivMap[z]; 
       trace('signal: ' + sglist[i].getAttribute('id') + "at level " + z);
       if( leveldiv == undefined ) {
         trace("Error: zlevel ["+z+"] does not exist!");
         continue;
       }
       sgMap[sglist[i].getAttribute('id')] = sglist[i];
       var newdiv = document.createElement('div');
       newdiv.setAttribute('id', "sg_"+sglist[i].getAttribute('id'));
       newdiv.setAttribute('onClick', "actionSignal(this.id)");
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       newdiv.style.width    = "32px";
       newdiv.style.height   = "32px";
       newdiv.style.lineHeight = "32px";
       newdiv.style.left     = "" + (parseInt(sglist[i].getAttribute('x')) * 32) + "px";
       newdiv.style.top      = "" + (parseInt(sglist[i].getAttribute('y')) * 32 + yoffset) + "px";
       newdiv.innerHTML      = "";
       newdiv.style.backgroundImage = getSignalImage(sglist[i]);
       trace("Signal image="+newdiv.style.backgroundImage);

       leveldiv.appendChild(newdiv);
     }
     
     
     tklist = xmlDoc.getElementsByTagName("tk");
     if( tklist.length > 0 )
       trace("processing " + tklist.length + " tracks");

     for (var i = 0; i < tklist.length; i++) {
       var z     = tklist[i].getAttribute('z');
       var ori   = getOriNr(tklist[i].getAttribute('ori'));
       var leveldiv = zlevelDivMap[z]; 
       trace('track: ' + tklist[i].getAttribute('id') + "at level " + z);
       if( leveldiv == undefined ) {
         trace("Error: zlevel ["+z+"] does not exist!");
         continue;
       }
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
       //trace("Track image="+newdiv.style.backgroundImage + "    " + getTrackImage(tklist[i]));
       trace("Track image="+newdiv.style.backgroundImage);

       leveldiv.appendChild(newdiv);
     }
     

     txlist = xmlDoc.getElementsByTagName("tx");
     if( txlist.length > 0 )
       trace("processing " + txlist.length + " texts");

     for (var i = 0; i < txlist.length; i++) {
       var z     = txlist[i].getAttribute('z');
       var ori   = getOri(txlist[i]);
       var text  = txlist[i].getAttribute('text');
       var leveldiv = zlevelDivMap[z]; 
       if( text == undefined )
         text = "";
       trace('text: ' + txlist[i].getAttribute('id') + "at level " + z + " text=["+text+"]");
       if( leveldiv == undefined ) {
         trace("Error: zlevel ["+z+"] does not exist!");
         continue;
       }
       txMap[txlist[i].getAttribute('id')] = txlist[i];
       var newdiv = document.createElement('div');
       newdiv.setAttribute('id', "tx_"+txlist[i].getAttribute('id'));
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       var transparent = txlist[i].getAttribute('transparent');
       if( transparent != undefined && transparent == "false" ) {
         var backred = txlist[i].getAttribute('backred'); 
         var backgreen = txlist[i].getAttribute('backgreen'); 
         var backblue = txlist[i].getAttribute('backblue');
         if( backred != undefined && backred != "-1" && backgreen != undefined && backgreen != "-1" && backblue != undefined && backblue != "-1" )
           newdiv.style.backgroundColor = "rgb("+backred+","+backgreen+","+backblue+")";
       }
       var red = txlist[i].getAttribute('red'); 
       var green = txlist[i].getAttribute('green'); 
       var blue = txlist[i].getAttribute('blue');
       if( red != undefined && green != undefined && blue != undefined )
         newdiv.style.color = "rgb("+red+","+green+","+blue+")";
       newdiv.style.textAlign = 'left';
       if( ori == "north" || ori == "south" ) {
         newdiv.style.width    = "" + (parseInt(txlist[i].getAttribute('cy')) * 32) + "px";
         newdiv.style.height   = ""  + (parseInt(txlist[i].getAttribute('cx')) * 32) + "px";
       }
       else {
         newdiv.style.width    = "" + (parseInt(txlist[i].getAttribute('cx')) * 32) + "px";
         newdiv.style.height   = ""  + (parseInt(txlist[i].getAttribute('cy')) * 32) + "px";
       }
       newdiv.style.left     = "" + (parseInt(txlist[i].getAttribute('x')) * 32) + "px";
       newdiv.style.top      = "" + (parseInt(txlist[i].getAttribute('y')) * 32 + yoffset) + "px";
       if( text != undefined && text.indexOf(".png") != -1 ) {
         //newdiv.style.backgroundImage = "url('"+text+"')";
         newdiv.style.backgroundSize = newdiv.style.width;
         if( ori == "north" || ori == "south" )
           newdiv.innerHTML = "<div class='imageV'><img height='"+newdiv.style.width+"' src='"+text+"'/></div>";
         else
           newdiv.innerHTML = "<div><img width='"+newdiv.style.width+"' src='"+text+"'/></div>";
       }
       else {  
         var pointsize = txlist[i].getAttribute('pointsize');
         if( pointsize == undefined || pointsize == "0")
           pointsize = "10";
         
         if( ori == "north" || ori == "south" )
           newdiv.innerHTML = "<div class='itemtextV' style='font-size:"+pointsize+"px;'>"+getTextFormat(txlist[i])+"</div>";
         else
           newdiv.innerHTML = "<div style='font-size:"+pointsize+"px; horizontal-align:left;'>" +getTextFormat(txlist[i])+ "</div>";
       }
       //newdiv.style.backgroundImage = getTextImage(txlist[i]);
       leveldiv.appendChild(newdiv);
     }
     

     swlist = xmlDoc.getElementsByTagName("sw");
     if( swlist.length > 0 )
       trace("processing " + swlist.length + " switches");

     for (var i = 0; i < swlist.length; i++) {
       var z     = swlist[i].getAttribute('z');
       var leveldiv = zlevelDivMap[z]; 
       trace('switch: ' + swlist[i].getAttribute('id') + "at level " + z);
       if( leveldiv == undefined ) {
         trace("Error: zlevel ["+z+"] does not exist!");
         continue;
       }
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
       trace("Switch image="+newdiv.style.backgroundImage);

       leveldiv.appendChild(newdiv);
     }
     
     
     fblist = xmlDoc.getElementsByTagName("fb");
     if( fblist.length > 0 )
       trace("processing " + fblist.length + " sensors");

     for (var i = 0; i < fblist.length; i++) {
       var z     = fblist[i].getAttribute('z');
       var curve = fblist[i].getAttribute('curve');
       var ori   = getOriNr(fblist[i].getAttribute('ori'));
       if( z == undefined )
         z = '0';
       if( curve != "true" )
         ori = (ori % 2 == 0) ? 2 : 1;

       var leveldiv = zlevelDivMap[z]; 
       trace('sensor: ' + fblist[i].getAttribute('id') + "at level " + z);
       if( leveldiv == undefined ) {
         trace("Error: zlevel ["+z+"] does not exist!");
         continue;
       }
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
       
       newdiv.style.backgroundImage = getSensorImage(fblist[i], 0);
       //trace("Sensor image="+newdiv.style.backgroundImage);

       leveldiv.appendChild(newdiv);
       //document.body.appendChild(newdiv);
     }
     
     bklist = xmlDoc.getElementsByTagName("bk");
     if( bklist.length > 0 )
       trace("processing " + bklist.length + " blocks");
     for (var i = 0; i < bklist.length; i++) {
       var z = bklist[i].getAttribute('z');
       if( z == undefined )
         z = '0';
       var ori      = getOri(bklist[i]);
       var small    = bklist[i].getAttribute('smallsymbol');
       var leveldiv = zlevelDivMap[z]; 
       trace('block: ' + bklist[i].getAttribute('id') + " at level " + z);
       if( leveldiv == undefined ) {
         trace("Error: zlevel ["+z+"] does not exist!");
         continue;
       }
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
       newdiv.style.backgroundImage = getBlockImage(bklist[i], newdiv);
       newdiv.style.lineHeight = newdiv.style.height;

       var label = bklist[i].getAttribute('locid');
       if( label == undefined || label.length == 0 )
         label = bklist[i].getAttribute('id');
       label = label.split(' ').join('.');
       label = label.split('-').join('.');
       if( ori == "north" || ori == "south" ) {
         var labdiv = document.createElement('div');
         labdiv.setAttribute('class', "itemtextV");
         labdiv.style.width    = "128px";
         labdiv.style.height   = "32px";
         labdiv.style.fontSize = ""+blockPointsize+"px";
         labdiv.style.position = "absolute";
         labdiv.style.left     = "0px";
         labdiv.style.top      = "48px";
         labdiv.innerHTML      = label;
         newdiv.appendChild(labdiv);
       }
       else {
         newdiv.innerHTML      = "<label class='itemtext' style='font-size:"+blockPointsize+"px;'>"+label+"</label>";
       }
       
       leveldiv.appendChild(newdiv);
     }
     

     ttlist = xmlDoc.getElementsByTagName("tt");
     if( ttlist.length > 0 )
       trace("processing " + ttlist.length + " turntables");
     for (var i = 0; i < ttlist.length; i++) {
       var z = ttlist[i].getAttribute('z');
       if( z == undefined )
         z = '0';
       var ori      = getOri(ttlist[i]);
       var leveldiv = zlevelDivMap[z]; 
       trace('turntable: ' + ttlist[i].getAttribute('id') + " at level " + z);
       if( leveldiv == undefined ) {
         trace("Error: zlevel ["+z+"] does not exist!");
         continue;
       }
       var traverser = ttlist[i].getAttribute('traverser');
       var symbolsize = parseInt(ttlist[i].getAttribute('symbolsize'));
       if( symbolsize < 2 )
         symbolsize = 5;
       ttMap[ttlist[i].getAttribute('id')] = ttlist[i];
       var newdiv = document.createElement('div');
       newdiv.setAttribute('id', "tt_"+ttlist[i].getAttribute('id'));
       newdiv.setAttribute('onClick', "actionTurntable(this.id)");
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       newdiv.style.width    = ""+(symbolsize*32)+"px";
       newdiv.style.height   = ""+(symbolsize*32)+"px";
       newdiv.style.left     = "" + (parseInt(ttlist[i].getAttribute('x')) * 32) + "px";
       newdiv.style.top      = "" + (parseInt(ttlist[i].getAttribute('y')) * 32 + yoffset) + "px";
       newdiv.innerHTML  = getTurntableImage(ttlist[i], newdiv); 
       leveldiv.appendChild(newdiv);
     }
     
     
     stlist = xmlDoc.getElementsByTagName("st");
     if( stlist.length > 0 )
       trace("processing " + stlist.length + " routes");
     for (var i = 0; i < stlist.length; i++) {
       stMap[stlist[i].getAttribute('id')] = stlist[i];
     }
     
     
     fylist = xmlDoc.getElementsByTagName("seltab");
     if( fylist.length > 0 )
       trace("processing " + fylist.length + " fiddle yards");
     for (var i = 0; i < fylist.length; i++) {
       var z = fylist[i].getAttribute('z');
       if( z == undefined )
         z = '0';
       var ori      = getOri(fylist[i]);
       var leveldiv = zlevelDivMap[z]; 
       trace('fiddle yard: ' + fylist[i].getAttribute('id') + " at level " + z + " ori=" + ori);
       if( leveldiv == undefined ) {
         trace("Error: zlevel ["+z+"] does not exist!");
         continue;
       }
       fyMap[fylist[i].getAttribute('id')] = fylist[i];
       
       var newdiv = document.createElement('div');
       newdiv.setAttribute('id', "fy_"+fylist[i].getAttribute('id'));
       newdiv.setAttribute('onClick', "actionFiddleYard(this.id)");
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       newdiv.style.left     = "" + (parseInt(fylist[i].getAttribute('x')) * 32) + "px";
       newdiv.style.top      = "" + (parseInt(fylist[i].getAttribute('y')) * 32 + yoffset) + "px";
       newdiv.innerHTML  = getFiddleYardImage(fylist[i], newdiv); 
       
       leveldiv.appendChild(newdiv);
     }
     
     
     sblist = xmlDoc.getElementsByTagName("sb");
     if( sblist.length > 0 )
       trace("processing " + sblist.length + " staging blocks");
     for (var i = 0; i < sblist.length; i++) {
       var z = sblist[i].getAttribute('z');
       if( z == undefined )
         z = '0';
       var ori      = getOri(sblist[i]);
       var leveldiv = zlevelDivMap[z]; 
       trace('staging block: ' + sblist[i].getAttribute('id') + " at level " + z + " ori=" + ori);
       if( leveldiv == undefined ) {
         trace("Error: zlevel ["+z+"] does not exist!");
         continue;
       }
       sbMap[sblist[i].getAttribute('id')] = sblist[i];
       var newdiv = document.createElement('div');
       newdiv.setAttribute('id', "sb_"+sblist[i].getAttribute('id'));
       newdiv.setAttribute('onClick', "actionStageBlock(this.id)");
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       newdiv.style.width    = "128px";
       newdiv.style.height   = "32px";
       newdiv.style.left     = "" + (parseInt(sblist[i].getAttribute('x')) * 32) + "px";
       newdiv.style.top      = "" + (parseInt(sblist[i].getAttribute('y')) * 32 + yoffset) + "px";
       newdiv.style.backgroundImage = getStageBlockImage(sblist[i], newdiv);
       newdiv.style.lineHeight = newdiv.style.height;

       var lcCount = 0;
       sectionlist = sblist[i].getElementsByTagName("section");
       if( sectionlist.length > 0 ) {
         trace("processing " + sectionlist.length + " staging block sections");
         for (var n = 0; n < sectionlist.length; n++) {
           var lcid = sectionlist[n].getAttribute('lcid');
           if( lcid != undefined && lcid.length > 0 ) {
             lcCount++;
           }
         }
       }

       var label = sblist[i].getAttribute('locid');
       if( label == undefined || label.length == 0 )
         label = sblist[i].getAttribute('id') + "[" + lcCount + "]";
       if( ori == "north" || ori == "south" ) {
         newdiv.innerHTML      = "<div class='itemtextV' style='font-size:"+blockPointsize+"px;'>"+label+"</div>";
       }
       else {
         newdiv.innerHTML      = "<label class='itemtext' style='font-size:"+blockPointsize+"px;'>"+label+"</label>";
       }

       leveldiv.appendChild(newdiv);
     }
     
     
   }
   catch(e) {
     console.log("exception: " + e.stack);
   }

}


function Item(props) {
  this.props = props;
}
Item.prototype.getId = function() {
  console.log("item id is " + this.props.getAttribute('id'));
};