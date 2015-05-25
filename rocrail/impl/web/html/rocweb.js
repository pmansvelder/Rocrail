/* Variables */
var req;
var yoffset = 48;
var scale = 1.0;
var blockPointsize = 12;
var planloaded = false;
var ws = null;
var initWS;
var worker;
var tapholdFkey = 0;
var zlevelDivMap = {};
var zlevelDivList = [];
var zlevelMap = {};
var zlevelList = [];
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
var scMap = {};
var tourMap = {};
var locationMap = {};
var operatorMap = {};
var carMap = {};
var lcCatMap = {};
var ModPlan = false;
var locoSelected = 'none';
var masterSelected = "";
var locoSelectedList = [];
var locoSelectedIdx = 0;
var locoBlockSelect = 'none';
var locoConsistSelect = 'none';
var locoConsistAction = 'show';
var locoConsistMembers = "";
var scheduleBlockSelect = 'none';
var blockBlockSelect = 'none';
var trainBlockSelect = 'none';
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
var prevPopup = "";
var guestProt = "P";
var guestSteps = "28";
var trackTTSelect = 'none';
var speedStepDelta = 3;
var redBackground = "#FFC8C8";


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
  document.getElementById("menuGuest").innerHTML = "Gast-Lok";
  //document.getElementById("menuTrains").innerHTML = "Züge";
  document.getElementById("menuDisconnect").innerHTML = "Server trennen";
  document.getElementById("systemTitle").innerHTML = "<b>System</b>";
  document.getElementById("systemInitField").innerHTML = "Feld initialisieren";
  document.getElementById("systemQuerySensors").innerHTML = "Tages-Anfang";
  document.getElementById("systemEmergencyBreak").innerHTML = "Nothalt";
  document.getElementById("optionsTitle").innerHTML = "<b>Optionen</b>";
  document.getElementById("labOptionDebug").innerHTML = "Debug";
  document.getElementById("labOptionSimSensors").innerHTML = "Rückmelder simulieren";
  document.getElementById("labOptionShowAllSchedules").innerHTML = "Zeige alle Fahrpläne";
  document.getElementById("labOptionModuleView").innerHTML = "Modul-Ansicht";
  document.getElementById("labOptionShowOcc").innerHTML = "Blockbelegung";
  document.getElementById("labOptionShowRoutes").innerHTML = "Zeige Fahrstraßen";
  document.getElementById("labOptionShowRoutesOnSwitches").innerHTML = "Zeige Fahrstraßen bei Weichen";
  document.getElementById("labOptionAllSpeedSteps").innerHTML = "Alle Geschwindigkeitsstufen verwenden";
  document.getElementById("labOptionSpeedButtons").innerHTML = "Geschwindigkeitstasten";
  document.getElementById("labLocoCatEngine").innerHTML = "Antriebsart";
  document.getElementById("labLocoCatEra").innerHTML = "Epoche";
  document.getElementById("labLocoCatRoadname").innerHTML = "Gesellschaft";
  document.getElementById("labBlockStart").innerHTML = "Zug starten";
  document.getElementById("labBlockStop").innerHTML = "Zug anhalten";
  document.getElementById("labBlockManual").innerHTML = "Halbautomatisch";
  document.getElementById("labBlockReset").innerHTML = "Zurücksetzen";
  document.getElementById("labBlockSwapPlacing").innerHTML = "Umdrehen";
  document.getElementById("labBlockSwapEnter").innerHTML = "Einfahrtseite drehen";
  document.getElementById("labBlockClose").innerHTML = "Schliessen";
  document.getElementById("labBlockOpen").innerHTML = "Öffnen";
  document.getElementById("labTTNext").innerHTML = "Nächstes Gleis";
  document.getElementById("labTTPrev").innerHTML = "Vorheriges Gleis";
  document.getElementById("labTTGoto").innerHTML = "Gehe zum Gleis";
  document.getElementById("labFYNext").innerHTML = "Nächstes Gleis";
  document.getElementById("labFYPrev").innerHTML = "Vorheriges Gleis";
  document.getElementById("labStageClose").innerHTML = "Schliessen";
  document.getElementById("labStageOpen").innerHTML = "Öffnen";
  document.getElementById("labStageCloseExit").innerHTML = "Ausfahrt schliessen";
  document.getElementById("labStageOpenExit").innerHTML = "Ausfahrt öffnen";
  document.getElementById("labStageCompress").innerHTML = "Komprimieren";
  document.getElementById("labConsistView").innerHTML = "Anzeigen";
  document.getElementById("labConsistAdd").innerHTML = "Hinzufügen";
  document.getElementById("labConsistDel").innerHTML = "Löschen";
  document.getElementById("titleGuestLoco").innerHTML = "<b>Gast-Lok</b>";
  document.getElementById("guestAddress").placeholder = "Adresse";
  document.getElementById("guestShortID").placeholder = "Kennung";
  document.getElementById("guestAdd").innerHTML = "Hinzufügen";
  document.getElementById("colorNone").innerHTML = "Durchsichtig";
  document.getElementById("colorGreen").innerHTML = "Grün";
  document.getElementById("colorGrey").innerHTML = "Grau";
  document.getElementById("colorWhite").innerHTML = "Weiß";
  document.getElementById("colorBlue").innerHTML = "Blau";
  document.getElementById("colorUser").innerHTML = "Eigene Farbe";
  document.getElementById("labUserColor").innerHTML = "Hintergrundfarbe";
  $('#colorSelect').selectmenu("refresh");
}

function langEN() {
  var donkey = "<br>Rocrail runs entirely on volunteer labor. However, Rocrail also needs contributions of money. Your continued support is vital for keeping Rocrail available. If you already did support you can ask a key to disable this on startup dialog: donate@rocrail.net<br>";
  donkey += "<div id=\"donkeyWarning\"><b>Rocweb will stop communicate in 5 minutes!</b></div>";
  document.getElementById("donkey").innerHTML = donkey;

  document.getElementById("menuInfo").innerHTML = "Information";
  document.getElementById("menuSystem").innerHTML = "System";
  document.getElementById("menuOptions").innerHTML = "Options";
  document.getElementById("menuGuest").innerHTML = "Guest loco";
  //document.getElementById("menuTrains").innerHTML = "Trains";
  document.getElementById("menuDisconnect").innerHTML = "Disconnect server";
  document.getElementById("systemTitle").innerHTML = "<b>System</b>";
  document.getElementById("systemInitField").innerHTML = "Init field";
  document.getElementById("systemQuerySensors").innerHTML = "Start of day";
  document.getElementById("systemEmergencyBreak").innerHTML = "Emergency break";
  document.getElementById("optionsTitle").innerHTML = "<b>Options</b>";
  document.getElementById("labOptionDebug").innerHTML = "Debug";
  document.getElementById("labOptionSimSensors").innerHTML = "Simulate sensors";
  document.getElementById("labOptionShowAllSchedules").innerHTML = "Show all schedules";
  document.getElementById("labOptionModuleView").innerHTML = "Module view";
  document.getElementById("labOptionShowOcc").innerHTML = "Block occupancy";
  document.getElementById("labOptionShowRoutes").innerHTML = "Show routes";
  document.getElementById("labOptionShowRoutesOnSwitches").innerHTML = "Show routes on switches";
  document.getElementById("labOptionAllSpeedSteps").innerHTML = "Use all speed steps";
  document.getElementById("labOptionSpeedButtons").innerHTML = "Speed buttons";
  document.getElementById("labLocoCatEngine").innerHTML = "Engine";
  document.getElementById("labLocoCatEra").innerHTML = "Era";
  document.getElementById("labLocoCatRoadname").innerHTML = "Roadname";
  document.getElementById("labBlockStart").innerHTML = "Start train";
  document.getElementById("labBlockStop").innerHTML = "Stop train";
  document.getElementById("labBlockManual").innerHTML = "Half automatic";
  document.getElementById("labBlockReset").innerHTML = "Soft reset";
  document.getElementById("labBlockSwapPlacing").innerHTML = "Swap placing";
  document.getElementById("labBlockSwapEnter").innerHTML = "Swap enter side";
  document.getElementById("labBlockClose").innerHTML = "Close";
  document.getElementById("labBlockOpen").innerHTML = "Open";
  document.getElementById("labTTNext").innerHTML = "Next track";
  document.getElementById("labTTPrev").innerHTML = "Previous track";
  document.getElementById("labTTGoto").innerHTML = "Goto track";
  document.getElementById("labFYNext").innerHTML = "Next track";
  document.getElementById("labFYPrev").innerHTML = "Previous track";
  document.getElementById("labStageClose").innerHTML = "Close";
  document.getElementById("labStageOpen").innerHTML = "Open";
  document.getElementById("labStageCloseExit").innerHTML = "Close exit";
  document.getElementById("labStageOpenExit").innerHTML = "Open exit";
  document.getElementById("labStageCompress").innerHTML = "Compress";
  document.getElementById("labConsistView").innerHTML = "Show";
  document.getElementById("labConsistAdd").innerHTML = "Add";
  document.getElementById("labConsistDel").innerHTML = "Delete";
  document.getElementById("titleGuestLoco").innerHTML = "<b>Guest loco</b>";
  document.getElementById("guestAddress").placeholder = "Address";
  document.getElementById("guestShortID").placeholder = "Short ID";
  document.getElementById("guestAdd").innerHTML = "Add";
  document.getElementById("colorNone").innerHTML = "Transparent";
  document.getElementById("colorGreen").innerHTML = "Green";
  document.getElementById("colorGrey").innerHTML = "Grey";
  document.getElementById("colorWhite").innerHTML = "White";
  document.getElementById("colorBlue").innerHTML = "Blue";
  document.getElementById("colorUser").innerHTML = "Own color";
  document.getElementById("labUserColor").innerHTML = "Background color";
  $('#colorSelect').selectmenu("refresh");
}

function langNL() {
  var donkey = "<br>Rocrail wordt uitsluitend door vrijwilligers ontwikkeld. Om Rocrail “in de lucht te houden” en steeds weer aan te kunnen passen aan nieuwe modelspoor hardware is er jaarlijks een bescheiden financieel budget nodig. Vrijwillige bijdragen zijn daarvoor noodzakelijk. Als je al een financiële ondersteuning gegeven hebt kun je op donate@rocrail.net een sleutel aanvragen om dit venster te onderdrukken.<br>";
  donkey += "<div id=\"donkeyWarning\"><b>Rocweb stopt met communicatie na 5 minuten!</b></div>";
  document.getElementById("donkey").innerHTML = donkey;
  
  document.getElementById("menuInfo").innerHTML = "Informatie";
  document.getElementById("menuSystem").innerHTML = "Systeem";
  document.getElementById("menuOptions").innerHTML = "Opties";
  document.getElementById("menuGuest").innerHTML = "Gast locomotief";
  //document.getElementById("menuTrains").innerHTML = "Treinstellen";
  document.getElementById("menuDisconnect").innerHTML = "Server loskoppelen";
  document.getElementById("systemTitle").innerHTML = "<b>Systeem</b>";
  document.getElementById("systemInitField").innerHTML = "Init veld";
  document.getElementById("systemQuerySensors").innerHTML = "Begin van de dag";
  document.getElementById("systemEmergencyBreak").innerHTML = "Noodstop";
  document.getElementById("optionsTitle").innerHTML = "<b>Opties</b>";
  document.getElementById("labOptionDebug").innerHTML = "Debug";
  document.getElementById("labOptionSimSensors").innerHTML = "Melders simuleren";
  document.getElementById("labOptionShowAllSchedules").innerHTML = "Laat alle dienstroosters zien";
  document.getElementById("labOptionModuleView").innerHTML = "Module weergave";
  document.getElementById("labOptionShowOcc").innerHTML = "Blok bezetting";
  document.getElementById("labOptionShowRoutes").innerHTML = "Toon rijwegen";
  document.getElementById("labOptionShowRoutesOnSwitches").innerHTML = "Toon rijwegen over wissels";
  document.getElementById("labOptionAllSpeedSteps").innerHTML = "Gebruik alle snelheid stappen";
  document.getElementById("labOptionSpeedButtons").innerHTML = "Snelheid knoppen";
  document.getElementById("labLocoCatEngine").innerHTML = "Aandrijving";
  document.getElementById("labLocoCatEra").innerHTML = "Periode";
  document.getElementById("labLocoCatRoadname").innerHTML = "Maatschappij";
  document.getElementById("labBlockStart").innerHTML = "Start trein";
  document.getElementById("labBlockStop").innerHTML = "Stop trein";
  document.getElementById("labBlockManual").innerHTML = "Half automaat";
  document.getElementById("labBlockReset").innerHTML = "Reset";
  document.getElementById("labBlockSwapPlacing").innerHTML = "Omkeren";
  document.getElementById("labBlockSwapEnter").innerHTML = "Aankomst omkeren";
  document.getElementById("labBlockClose").innerHTML = "Sluiten";
  document.getElementById("labBlockOpen").innerHTML = "Openen";
  document.getElementById("labTTNext").innerHTML = "Volgende spoor";
  document.getElementById("labTTPrev").innerHTML = "Vorige spoor";
  document.getElementById("labTTGoto").innerHTML = "Ga naar spoor";
  document.getElementById("labFYNext").innerHTML = "Volgende spoor";
  document.getElementById("labFYPrev").innerHTML = "Vorige spoor";
  document.getElementById("labStageClose").innerHTML = "Sluiten";
  document.getElementById("labStageOpen").innerHTML = "Openen";
  document.getElementById("labStageCloseExit").innerHTML = "Sluit uitgang";
  document.getElementById("labStageOpenExit").innerHTML = "Open uitgang";
  document.getElementById("labStageCompress").innerHTML = "Comprimeer";
  document.getElementById("labConsistView").innerHTML = "Tonen";
  document.getElementById("labConsistAdd").innerHTML = "Toevoegen";
  document.getElementById("labConsistDel").innerHTML = "Verwijderen";
  document.getElementById("titleGuestLoco").innerHTML = "<b>Gast locomotief</b>";
  document.getElementById("guestAddress").placeholder = "Adres";
  document.getElementById("guestShortID").placeholder = "Korte ID";
  document.getElementById("guestAdd").innerHTML = "Toevoegen";
  document.getElementById("colorNone").innerHTML = "Transparant";
  document.getElementById("colorGreen").innerHTML = "Groen";
  document.getElementById("colorGrey").innerHTML = "Grijs";
  document.getElementById("colorWhite").innerHTML = "Wit";
  document.getElementById("colorBlue").innerHTML = "Blauw";
  document.getElementById("colorUser").innerHTML = "Eigen kleur";
  document.getElementById("labUserColor").innerHTML = "Achtergrond kleur";
  $('#colorSelect').selectmenu("refresh");
}


/* Info Dialog */
function openInfo()
{
  //trace("close menu");
  //$( "#popupMenu" ).popup( "close" );
  
  trace("open info");
  //$('#popupMenu').on("popupafterclose", function(){$( "#popupInfo" ).popup( "open" )});
  $('#popupMenu').on("panelclose", function(){
    $('#popupMenu').unbind( "panelclose" );
    $( "#popupInfo" ).popup( "open" );
    });
}

function onLocoImage() {
  if( tapholdFkey == 1 ) {
    tapholdFkey = 0;
    return;
  }
  initLocoList();

  trace("close throttle");
  prevPopup = "popupThrottle;"
  $( "#popupThrottle" ).popup( "close" );
  trace("open loco select");
  $('#popupThrottle').on("popupafterclose", function(){
    $('#popupThrottle').unbind( "popupafterclose" );
    $( "#popupLocoSelect" ).popup( "open" );
    });
}

function onBlockLocoImage() {
  if( tapholdFkey == 1 ) {
    tapholdFkey = 0;
    return;
  }
  initLocoList();
  trace("close block popup");
  prevPopup = "popupBlock";
  $( "#popupBlock" ).popup( "close" );
  trace("open loco select");
  $('#popupBlock').on("popupafterclose", function(){
    $('#popupBlock').unbind( "popupafterclose" );
    $( "#popupLocoSelect" ).popup( "open" );
    });
}

function onLocoSelected(sel) {
  trace("selected loco: " + sel);
  
  trace("close loco select");
  $( "#popupLocoSelect" ).popup( "close" );
  
  if( prevPopup == "popupBlock") {
    locoBlockSelect = sel;
    sessionStorage.setItem("locoBlockSelect", locoBlockSelect);
    bkid = sessionStorage.getItem("block");
    trace("locoBlockSelect: " + locoBlockSelect + " in block " + bkid);
    onLocoInBlock(locoBlockSelect);
    return;
  }
  else if( prevPopup == "popupConsist") {
    locoConsistSelect = sel;
    trace("locoConsistSelect: " + locoConsistSelect);
    $('#popupLocoSelect').on("popupafterclose", function(){
      $('#popupLocoSelect').unbind( "popupafterclose" );
      $( "#popupConsist" ).popup( "open" );
      });
    return;
  }
  else {
    locoSelected = sel;
    localStorage.setItem("locoSelected", locoSelected);
    locoSelectedList[locoSelectedIdx] = locoSelected;
    localStorage.setItem("locoSelected"+locoSelectedIdx, locoSelected);
    initThrottle();
    updateDir();
    updateFunctionLabels();
  }

  
  trace("open " + popupThrottle);
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
    F.innerHTML = "<label style='font-size:13px;font-weight:bold;'>"+"F" + (i + FGroup * 14) + "</label>"; 
    F.style.backgroundColor = '';
  }
  var eF = document.getElementById("F0");
  eF.innerHTML = "<label style='font-size:13px;font-weight:bold;'>"+"F0"+"</label>"; 
  eF.style.backgroundColor = '';
  eF = document.getElementById("ST");
  eF.innerHTML = "<label style='font-size:13px;font-weight:bold;'>"+"ST"+"</label>"; 
  eF.style.backgroundColor = '';
  eF = document.getElementById("RE");
  eF.innerHTML = "<label style='font-size:13px;font-weight:bold;'>"+"RE"+"</label>"; 
  eF.style.backgroundColor = '';
  eF = document.getElementById("FG");
  eF.innerHTML = "<label style='font-size:13px;font-weight:bold;'>"+"FG"+"</label>"; 
  eF.style.backgroundColor = '';

  lc = lcMap[locoSelected];
  if(lc == undefined)
    lc = carMap[locoSelected];
  if( lc != undefined ) {
    var fx = parseInt(lc.getAttribute('fx'));
    var fn = lc.getAttribute('fn');
    
    if( fn != undefined && fn == "true")
      lights.style.backgroundColor = redBackground;
      
    for(i = 1; i < 15; i++) {
      var F = document.getElementById("F"+i);
      var iFnShift = i;
      if( FGroup == 1 )
        iFnShift += 14;
      var mask = 1 << (iFnShift-1); 
      trace("i="+i+" iFnShift="+(iFnShift-1)+" mask="+mask.toString(16)+" fx="+fx.toString(16));
      if( fx & mask )
        F.style.backgroundColor = redBackground;
    }

    //var fundeflist = lc.childNodes;
    var fundeflist = lc.getElementsByTagName("fundef");
    trace("function defs " + fundeflist.length + " for " + lc.getAttribute('id'));

    if( fundeflist.length > 0 ) {
      for( n = 0; n < fundeflist.length; n++ ) {
        var fn = fundeflist[n].getAttribute('fn');
        var iFn = parseInt(fn);
        var iFnShift = iFn;
        trace("fundef " + fn + " text: " + fundeflist[n].getAttribute('text'));
        if( FGroup == 0 && iFn > 14 ) {
          continue;
        }
        if( FGroup == 1 && iFn < 15 ) {
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
        var mask = 1 << (iFnShift-1); 
        if( fx & mask )
          F.style.backgroundColor = redBackground;

      }
    }
  }

}

function onFG() {
  if( tapholdFkey == 1 ) {
    tapholdFkey = 0;
    return;
  }
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
function initMenu()
{
  var levelSelect = document.getElementById("levelSelect");
  while(levelSelect.options.length > 0) {
    levelSelect.remove(0);
  }
  
  levelSelect.selectedIndex = 0;
  
  for (var i in zlevelMap){
    var zlevel = zlevelMap[i];
    var title  = zlevel.getAttribute('title');
    var z      = zlevel.getAttribute('z');
    if( z == undefined )
      z = "0";
    zoption = document.createElement( 'option' );
    zoption.value = z;
    zoption.innerHTML = title;
    levelSelect.add( zoption );
    if( document.getElementById("title").innerHTML == title )
      levelSelect.selectedIndex = i;
  }
  $('#levelSelect').selectmenu("refresh");

  
  var lang = localStorage.lang;
  var sel = document.getElementById('languageSelect');
  
  if( lang == "en" ) {
    sel.selectedIndex = 0;
  }
  else if( lang == "de" ) {
    sel.selectedIndex = 1;
  }
  else if( lang == "nl" ) {
    sel.selectedIndex = 2;
  }
  $('#languageSelect').selectmenu("refresh");
  
  var color = localStorage.color;
  sel = document.getElementById('colorSelect');
  trace("colorSelect = "+color);
  if( color == "none" ) {
    sel.selectedIndex = 0;
  }
  else if( color == "green" ) {
    sel.selectedIndex = 1;
  }
  else if( color == "grey" ) {
    sel.selectedIndex = 2;
  }
  else if( color == "white" ) {
    sel.selectedIndex = 3;
  }
  else if( color == "blue" ) {
    sel.selectedIndex = 4;
  }
  else if( color == "user" ) {
    sel.selectedIndex = 5;
  }
  $('#colorSelect').selectmenu("refresh");

  //$('#popupMenu').unbind("popupafterclose");
  //$( "#popupMenu" ).popup( "open" );
}


function openSystem() {
  trace("close menu");
  //$( "#popupMenu" ).popup( "close" );
  $('#popupMenu').on("panelclose", function(){
    $('#popupMenu').unbind( "panelclose" );
    $( "#popupSystem" ).popup( "open" );
    });
}

function openZoom(fromMenu) {
  if( fromMenu ) {
    //trace("close menu");
    //$( "#popupMenu" ).popup( "close" );
    //$('#popupMenu').on("popupafterclose", function(){$( "#popupZoom" ).popup( "open" )});
    $('#popupMenu').on("panelclose", function(){
      $('#popupMenu').unbind( "panelclose" );
      $( "#popupZoom" ).popup( "open" );
      });
  }
  else {
    $( "#popupZoom" ).popup( "open" );
  }
}

/* Guest Loco Dialog */
function openGuest() {
  document.getElementById("guestAddress").value = "";
  document.getElementById("guestShortID").value = "";

  guestProt = "N";
  guestSteps = "28";
  $("#guestProtDCC").prop("checked", true).checkboxradio('refresh');
  $("#guestStep28").prop("checked", true).checkboxradio('refresh');

  //trace("close menu");
  //$( "#popupMenu" ).popup( "close" );
  //$('#popupMenu').on("popupafterclose", function(){$( "#popupGuestLoco" ).popup( "open" )});
  $('#popupMenu').on("panelclose", function(){
    $('#popupMenu').unbind( "panelclose" );
    $( "#popupGuestLoco" ).popup( "open" );
    });

}

function openTrains() {
  // ToDo: Popup.
}

/* Options Dialog */
function openOptions() {
  //trace("close menu");
  //$( "#popupMenu" ).popup( "close" );
  
  trace("open info");
  var debug = localStorage.getItem("debug");
  $('#optionDebug').prop('checked', debug=="true"?true:false).checkboxradio('refresh');
  var simsensors = localStorage.getItem("simsensors");
  $('#optionSimSensors').prop('checked', simsensors=="true"?true:false).checkboxradio('refresh');
  var showallschedules = localStorage.getItem("showallschedules");
  $('#optionShowAllSchedules').prop('checked', showallschedules=="true"?true:false).checkboxradio('refresh');
  var moduleview = localStorage.getItem("moduleview");
  $('#optionModuleView').prop('checked', moduleview=="true"?true:false).checkboxradio('refresh');
  var showocc = localStorage.getItem("showocc");
  $('#optionShowOcc').prop('checked', showocc=="true"?true:false).checkboxradio('refresh');
  var showroutes = localStorage.getItem("showroutes");
  $('#optionShowRoutes').prop('checked', showroutes=="true"?true:false).checkboxradio('refresh');
  var showroutesonswitches = localStorage.getItem("showroutesonswitches");
  $('#optionShowRoutesOnSwitches').prop('checked', showroutesonswitches=="true"?true:false).checkboxradio('refresh');
  var allspeedsteps = localStorage.getItem("allspeedsteps");
  $('#optionAllSpeedSteps').prop('checked', allspeedsteps=="true"?true:false).checkboxradio('refresh');
  var speedbuttons = localStorage.getItem("speedbuttons");
  $('#optionSpeedButtons').prop('checked', speedbuttons=="true"?true:false).checkboxradio('refresh');

  var category = localStorage.getItem("category");
  
  if( category == "era" )
    $("#locoCatEra").prop("checked", true).checkboxradio('refresh');
  else if( category == "roadname" )
    $("#locoCatRoadname").prop("checked", true).checkboxradio('refresh');
  else
    $("#locoCatEngine").prop("checked", true).checkboxradio('refresh');
  
  var select = document.getElementById("languageSelect");

  var red   = localStorage.getItem("red");
  var green = localStorage.getItem("green");
  var blue  = localStorage.getItem("blue");
  document.getElementById("userColorRed").value = red;
  document.getElementById("userColorGreen").value = green;
  document.getElementById("userColorBlue").value = blue;


  //$('#popupMenu').on("popupafterclose", function(){$( "#popupOptions" ).popup( "open" )});
  $('#popupMenu').on("panelclose", function(){
    $('#popupMenu').unbind( "panelclose" );
    $( "#popupOptions" ).popup( "open" );
    });

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


function openConsist() {
  if( true ) {
    trace("consist is disabled");
    return;
  }
  trace("close throttle");
  $( "#popupThrottle" ).popup( "close" );
  
  initConsist();
  trace("open loco consist");
  $('#popupThrottle').on("popupafterclose", function(){
    $('#popupThrottle').unbind( "popupafterclose" );
    $( "#popupConsist" ).popup( "open" );
    });
  $('#popupConsist').on("popupafterclose", function(){
    $('#popupConsist').unbind( "popupafterclose" );
    $( "#popupThrottle" ).popup( "open" );
    });
  
}

function openLocoControl() {
  trace("close throttle");
  $( "#popupThrottle" ).popup( "close" );
  trace("open loco control");
  $('#popupThrottle').on("popupafterclose", function(){
    $('#popupThrottle').unbind( "popupafterclose" );
    $( "#popupLocoControl" ).popup( "open" );
    });
  $('#popupLocoControl').on("popupafterclose", function(){
    $('#popupLocoControl').unbind( "popupafterclose" );
    $( "#popupThrottle" ).popup( "open" );
    });
}

/* Throttle commands */
window.oncontextmenu = function(event) {
  event.preventDefault();
  event.stopPropagation();
  return false;
};
$(function(){
  $("#F1").bind("taphold", tapholdF1Handler);
  $("#F2").bind("taphold", tapholdF2Handler);
  $("#F3").bind("taphold", tapholdF3Handler);
  $("#F4").bind("taphold", tapholdF4Handler);
  $("#RE").bind("taphold", tapholdREHandler);
  $("#FG").bind("taphold", tapholdFGHandler);
  $("#F0").bind("taphold", tapholdF0Handler);
  $("#F13").bind("taphold", tapholdF13Handler);
  $("#F14").bind("taphold", tapholdF14Handler);
  $("#direction").bind("taphold", tapholdDirectionHandler);
  //$("#locoImage").bind("taphold", tapholdLocoImageHandler);
  $("#locoImageBlock").bind("taphold", tapholdLocoImageBlockHandler);
 
  function tapholdF1Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F1");
    locoSelectedIdx = 0;
    locoSelected = locoSelectedList[locoSelectedIdx];
    initThrottle();
    updateDir();
    updateFunctionLabels();
  }
  function tapholdF2Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F2");
    locoSelectedIdx = 1;
    locoSelected = locoSelectedList[locoSelectedIdx];
    initThrottle();
    updateDir();
    updateFunctionLabels();
  }
  function tapholdF3Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F3");
    locoSelectedIdx = 2;
    locoSelected = locoSelectedList[locoSelectedIdx];
    initThrottle();
    updateDir();
    updateFunctionLabels();
  }
  function tapholdF4Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F4");
    locoSelectedIdx = 3;
    locoSelected = locoSelectedList[locoSelectedIdx];
    initThrottle();
    updateDir();
    updateFunctionLabels();
  }
  function tapholdREHandler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold RE: power off");
    var cmd = "<sys informall=\"true\" cmd=\"stop\"/>";
    worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
  }
  function tapholdFGHandler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold FG: emergancy break");
    var cmd = "<sys cmd=\"ebreak\" informall=\"true\"/>";
    worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
  }
  function tapholdF0Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F0: dispatch");
    var cmd = "<lc id=\""+locoSelected+"\" cmd=\"dispatch\"/>";
    worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
  }
  function tapholdF13Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F13: manual mode");
    var lc = lcMap[locoSelected];
    var manualon = lc.getAttribute('manualon');
    var cmd = "";
    if( manualon == undefined || manualon == "false" ) {
      cmd = "<lc id=\""+locoSelected+"\" cmd=\"manualon\"/>";
      lc.setAttribute('manualon', "true");
    }
    else {
      cmd = "<lc id=\""+locoSelected+"\" cmd=\"manualoff\"/>";
      lc.setAttribute('manualon', "false");
    }
    worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
  }
  function tapholdF14Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F14: shunting");
    var shuntingon = lc.getAttribute('shuntingon');
    var cmd = "";
    if( shuntingon == undefined || shuntingon == "false" ) {
      cmd = "<lc id=\""+locoSelected+"\" cmd=\"shuntingon\"/>";
      lc.setAttribute('shuntingon', "true");
    }
    else {
      cmd = "<lc id=\""+locoSelected+"\" cmd=\"shuntingoff\"/>";
      lc.setAttribute('shuntingon', "false");
    }
    worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
  }
  function tapholdDirectionHandler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold direction: consist management");
    openConsist();
  }
  
  function tapholdLocoImageHandler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold locoImage: loco management");
    openLocoControl();
  }

  function tapholdLocoImageBlockHandler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold locoImageBlock: free block");
    onLocoResetInBlock();
  }

});

function onDirection() {
  if( tapholdFkey == 1 ) {
    tapholdFkey = 0;
    return;
  }
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

function onRE() {
  if( tapholdFkey == 1 ) {
    tapholdFkey = 0;
    return;
  }
  trace("release loco " + locoSelected);
  var cmd = "<lc throttleid=\"rocweb\" cmd=\"release\" id=\""+locoSelected+"\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function onST() {
  trace("start/stop loco " + locoSelected);
  var lc = lcMap[locoSelected];
  var mode = lc.getAttribute('mode');
  var cmd = "";
  if( mode != undefined && (mode == "auto" || mode == "halfauto") )
    cmd = "<lc id=\""+locoSelected+"\" cmd=\"stop\"/>";
  else
    cmd = "<lc id=\""+locoSelected+"\" cmd=\"go\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function onFunction(id, nr) {
  if( tapholdFkey == 1 ) {
    tapholdFkey = 0;
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
  var mask = 1 << (nr-1);
  var on = fx&mask?"false":"true";
  var cmd = "<fn id=\""+locoSelected+"\" fnchanged=\""+nr+"\" group=\""+group+"\" f"+nr+"=\""+on+"\"/>"
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function onLights() {
  if( tapholdFkey == 1 ) {
    tapholdFkey = 0;
    return;
  }
  lc = lcMap[locoSelected];
  if(lc == undefined)
    lc = carMap[locoSelected];
  if( lc == undefined ) return;
  var fn = lc.getAttribute('fn');
  var on = "true";
  if( fn != undefined && fn == "true" )
    on = "false";
  trace("lights was "+fn+" will be "+on);
  var cmd = "<fn id=\""+locoSelected+"\" fnchanged=\""+0+"\" f"+0+"=\""+on+"\"/>"
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function speedUpdate(value) {
  lc = lcMap[locoSelected];
  if( lc == undefined )
    lc = carMap[locoSelected];
  if( lc == undefined ) return;
  trace("Speed: " + value + " for loco " + locoSelected);
  var vVal = value * (parseFloat(lc.getAttribute('V_max')/100.0));
  var iVal = Math.floor(vVal);
  lc.setAttribute('V', iVal);
  trace("value="+value+" vVal="+vVal+" V_max="+parseInt(lc.getAttribute('V_max')));
  var cmd = "<lc throttleid=\"rocweb\" id=\""+locoSelected+"\" V=\""+iVal+"\" dir=\""+lc.getAttribute('dir')+"\"/>";
  updateDir();
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}


/* System commands */
function actionPower() {
  var mOn = power == 'true' ? false:true;
  var cmd = "<sys informall=\"true\" cmd=\""+(mOn?"go":"stop")+"\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}


function actionLevelSelect(z) {
  if( ModPlan )
    return;
  zlevelSelected.style.display = 'none';
  zleveldiv = zlevelDivMap[z];
  zlevelSelected = zleveldiv;
  zleveldiv.style.display = 'block';
    
  var zlevel = zlevelMap[z];
  var title = zlevel.getAttribute('title');
  var h = document.getElementById("title");
  h.innerHTML = title;
}

function actionLevelDown() {
  if( ModPlan )
    return;
  zlevelSelected.style.display = 'none'
  zlevelIdx++;
  var zleveldiv = zlevelDivList[zlevelIdx];
  if(zleveldiv == undefined ) {
    zleveldiv = zlevelDivList[0];
    zlevelIdx = 0;
  }
  zlevelSelected = zleveldiv;
  zleveldiv.style.display = 'block';
    
  var zlevel = zlevelList[zlevelIdx];
  var title = zlevel.getAttribute('title');
  var h = document.getElementById("title");
  h.innerHTML = title;
}

function actionLevelUp() {
  if( ModPlan )
    return;
  zlevelSelected.style.display = 'none'
  zlevelIdx--;
  var zleveldiv = zlevelDivList[zlevelIdx];
  if(zleveldiv == undefined ) {
    zleveldiv = zlevelDivList[zlevelDivList.length-1];
    zlevelIdx = zlevelDivList.length-1;
  }
  zlevelSelected = zleveldiv;
  zleveldiv.style.display = 'block';

  var zlevel = zlevelList[zlevelIdx];
  var title = zlevel.getAttribute('title');
  var h = document.getElementById("title");
  h.innerHTML = title;
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

function actionRoute(id) {
  stid = id.replace("st_","");
  trace("route action on " + stid );
  st = stMap[stid];
  var cmd = "<st cmd=\"test\" id=\""+stid+"\"/>";
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


function actionText(id) {
  txid = id.replace("tx_","");
  sessionStorage.setItem("text", txid);
  var tx = txMap[txid];
  document.getElementById("textTitle").innerHTML = "<b>" + getString("text") + ": " + txid + "</b>";
  document.getElementById("newText").placeholder = tx.getAttribute('text');
  $( "#popupText" ).popup( "open", {positionTo: '#'+id} );
}

function onChangeText() {
  $( "#popupText" ).popup( "close" );
  txid = sessionStorage.getItem("text");
  var text = document.getElementById("newText").value;
  var cmd = "<model cmd=\"modify\"><tx id=\""+txid+"\" text=\""+text+"\"/></model>";
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
  var toggleswitch = co.getAttribute('toggleswitch');
  if( toggleswitch == undefined || toggleswitch == "true" ) {
    var cmd = "<co cmd=\"flip\" id=\""+coid+"\"/>";
    worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
  }
}

function actionOutputDown(id) {
  coid = id.replace("co_","");
  co = coMap[coid];
  trace("output down action on " + coid + " state=" + co.getAttribute('state'));
  var toggleswitch = co.getAttribute('toggleswitch');
  if( toggleswitch == undefined || toggleswitch == "true" )
    return;
  var cmd = "<co cmd=\"on\" id=\""+coid+"\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function actionOutputUp(id) {
  coid = id.replace("co_","");
  co = coMap[coid];
  trace("output up action on " + coid + " state=" + co.getAttribute('state'));
  var toggleswitch = co.getAttribute('toggleswitch');
  if( toggleswitch == undefined || toggleswitch == "true" )
    return;
  var cmd = "<co cmd=\"off\" id=\""+coid+"\"/>";
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
  document.getElementById("turntableTitle").innerHTML = "<b>" + getString("turntable") + ": " + ttid + "</b>";
  ttNode = ttMap[ttid];

  var trackSelect = document.getElementById("trackTTSelect");
  while(trackSelect.options.length > 0) {
    trackSelect.remove(0);
  }

  track = document.createElement( 'option' );
  track.value = "";
  track.innerHTML = "";
  track.selected = 'selected';
  trackSelect.add( track );

  tracklist = ttNode.getElementsByTagName("track");
  for (var i = 0; i < tracklist.length; i++) {
    var nr   = tracklist[i].getAttribute('nr');
    var desc = tracklist[i].getAttribute('desc');
    track = document.createElement( 'option' );
    track.value = nr;
    track.innerHTML = nr+" "+desc;
    track.setAttribute('onclick', "onTTTrackSelected('"+nr+"')");
    trackSelect.add( track );
  }
  $('#trackTTSelect').selectmenu("refresh");

  $( "#popupTurntable" ).popup( "open", {positionTo: '#'+id} );

}

function actionFiddleYard(id) {
  fyid = id.replace("fy_","");
  sessionStorage.setItem("fiddleyard", fyid);
  document.getElementById("fiddleyardTitle").innerHTML = "<b>" + getString("fiddleyard") + ": " + fyid + "</b>";

  fyNode = fyMap[fyid];
  $( "#popupFiddleYard" ).popup( "open", {positionTo: '#'+id} );

}


function isBlockInSchedule(bkid, sc) {
  var scentrylist = sc.getElementsByTagName("scentry");
  trace("scentry " + scentrylist.length + " for " + sc.getAttribute('id'));
  // Check only the first entry:

  var showallschedules = localStorage.getItem("showallschedules");
  if( showallschedules == undefined )
    showallschedules = "false";

  var all = scentrylist.length;
  if(showallschedules == "false")
    all = 1;
  for( n = 0; n < all; n++ ) {
    var block    = scentrylist[n].getAttribute('block');
    var location = scentrylist[n].getAttribute('location');
    
    if( block != undefined && block == bkid )
      return true;
    
    if( location != undefined && location.length > 0 && isBlockInLocation(bkid, location) )
      return true;
  }
  return false;
}

function addSchedule(bkid, sc, scheduleSelect, scid) {
  trace("addSchedule: "+scid);
  if( isBlockInSchedule(bkid, sc) ) {
    var scoption = document.createElement( 'option' );
    scoption.value = scid;
    scoption.innerHTML = scid;
    scheduleSelect.add( scoption );
  }
}

function isBlockInLocation(bkid, locationid) {
  trace("isBlockInLocation: block="+bkid+" location="+locationid);
  var location = locationMap[locationid];
  if( location == undefined ) {
    trace("location ["+locationid+"] not found");
    return false;
  }

  var blocks = location.getAttribute('blocks').split(",");
  for( var i = 0; i < blocks.length; i++ ) {
    trace(bkid+" == "+blocks[i]);
    if( bkid == blocks[i] )
      return true;
  }
  
  return false;
}

function actionBlock(id) {
  bkid = id.replace("bk_","");
  sessionStorage.setItem("block", bkid);
  document.getElementById("blockTitle").innerHTML = "<b>" + getString("block") + ": " + bkid + "</b>";
  document.getElementById("labBlockLocoID").innerHTML = "";

  bkNode = bkMap[bkid];
  
  scheduleBlockSelect = 'none';
  blockBlockSelect    = 'none';
  trainBlockSelect    = 'none';

  // Schedules and tours
  var scheduleSelect = document.getElementById("scheduleBlockSelect");
  while(scheduleSelect.options.length > 0) {
    scheduleSelect.remove(0);
  }

  scoption = document.createElement( 'option' );
  scoption.value = "none";
  scoption.innerHTML = getString("schedule");
  scoption.selected = 'selected';
  scheduleSelect.add( scoption );
  
  // Train list
  var trainSelect = document.getElementById("trainBlockSelect");
  while(trainSelect.options.length > 0) {
    trainSelect.remove(0);
  }

  trainoption = document.createElement( 'option' );
  trainoption.value = "none";
  trainoption.innerHTML = getString("train");
  trainSelect.add( trainoption );
  
  // Add all blocks as destination:
  var blockSelect = document.getElementById("blockBlockSelect");
  while(blockSelect.options.length > 0) {
    blockSelect.remove(0);
  }

  bkoption = document.createElement( 'option' );
  bkoption.value = "none";
  bkoption.innerHTML = getString("block");
  bkoption.selected = 'selected';
  blockSelect.add( bkoption );

  for (var i in bkMap){
    var bk = bkMap[i];
    if( bk.getAttribute('id') != bkid ) {
      bkoption = document.createElement( 'option' );
      bkoption.value = bk.getAttribute('id');
      bkoption.innerHTML = bk.getAttribute('id');
      blockSelect.add( bkoption );
    }
  }
  $('#blockBlockSelect').selectmenu("refresh");

  
  var selected = false;

  var img = document.getElementById("locoImageBlock");
  img.src = "noimg.png";

  var lc = lcMap[bkNode.getAttribute('locid')];
  if( lc != undefined ) {
    locoBlockSelect = lc.getAttribute('id');
    document.getElementById("labBlockLocoID").innerHTML = locoBlockSelect;

    var src = lc.getAttribute('image');
    if( src == undefined || src.length == 0 )
      img.src = "noimg.png";
    else
      img.src = lc.getAttribute('image');

    // Fill up the schedule list.
    for (var i in scMap){
      var sc = scMap[i];
      trace("add schedule from list");
      addSchedule(bkid, sc, scheduleSelect, sc.getAttribute('id'));
    }
    for (var i in tourMap){
      var tour = tourMap[i];
      var schedulelist = tour.getAttribute("schedules").split(",");
      var sc = scMap[schedulelist[0]];
      if( sc != undefined ) {
        trace("add schedule from tour: " + schedulelist[0]);
        addSchedule(bkid, sc, scheduleSelect, tour.getAttribute('id'));
      }
    }
    scheduleSelect.selectedIndex = 0;
    //scheduleSelect.value = ""; 
    
    var idx = 0;
    for (var i in operatorMap){
      var operator = operatorMap[i];
      opoption = document.createElement( 'option' );
      opoption.value = operator.getAttribute('id');
      opoption.innerHTML = operator.getAttribute('id');
      trainSelect.add( opoption );
      idx++;
      if( lc.getAttribute('train') != undefined && lc.getAttribute('train') == operator.getAttribute('id') ) {
        trace("lc train=" + lc.getAttribute('train') + " idx="+idx);
        opoption.selected = 'selected';
        trainSelect.selectedIndex = idx;
      }
    }

  }
  
  sessionStorage.setItem("locoBlockSelect", locoBlockSelect);

  $('#scheduleBlockSelect').selectmenu("refresh");
  $('#trainBlockSelect').selectmenu("refresh");

  var xPos = parseInt(bkNode.getAttribute('x')) * 32;
  var yPos = parseInt(bkNode.getAttribute('y')) * 32;
  prevPopup = "popupBlock";
  //$( "#popupBlock" ).popup( "open" ).position({x: xPos, y: yPos, positionTo: window});
  $( "#popupBlock" ).popup( "open", {positionTo: '#'+id} );

}

function onBlockTrain() {
  $( "#popupBlock" ).popup( "close" );
  locoBlockSelect = sessionStorage.getItem("locoBlockSelect");
  if( locoBlockSelect != "none" ) {
    if( trainBlockSelect == "none" ) {
      // release train
      var cmd = "<lc id=\""+locoBlockSelect+"\" cmd=\"releasetrain\"/>";
      worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
    }
    else {
      // assign train
      var cmd = "<lc id=\""+locoBlockSelect+"\" cmd=\"assigntrain\" train=\""+trainBlockSelect+"\"/>";
      worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
    }
  }  
}

function onBlockStart(gomanual) {
  $( "#popupBlock" ).popup( "close" );
  locoBlockSelect = sessionStorage.getItem("locoBlockSelect");
  if( locoBlockSelect != "none" ) {
    if( scheduleBlockSelect != "none" && scheduleBlockSelect.length > 0 ) {
      var cmd = "<lc id=\""+locoBlockSelect+"\" cmd=\"useschedule\" scheduleid=\""+scheduleBlockSelect+"\"/>";
      worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
    }  
    else if( blockBlockSelect != "none" && blockBlockSelect.length > 0 ) {
      var cmd = "<lc id=\""+locoBlockSelect+"\" cmd=\"gotoblock\" blockid=\""+blockBlockSelect+"\"/>";
      worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
    }  
    var cmd = "<lc id=\""+locoBlockSelect+"\" cmd=\""+(gomanual?"gomanual":"go")+"\"/>";
    worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
  }
}

function onBlockReset(soft) {
  $( "#popupBlock" ).popup( "close" );
  locoBlockSelect = sessionStorage.getItem("locoBlockSelect");
  if( locoBlockSelect != "none" ) {
    var cmd = "<lc id=\""+locoBlockSelect+"\" cmd=\""+(soft?"softreset":"reset")+"\"/>";
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

function onBlockSwapPlacing() {
  $( "#popupBlock" ).popup( "close" );
  locoBlockSelect = sessionStorage.getItem("locoBlockSelect");
  if( locoBlockSelect != "none" ) {
    var cmd = "<lc id=\""+locoBlockSelect+"\" cmd=\"swap\"/>";
    worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
  }
}

function onBlockSwapEnter() {
  $( "#popupBlock" ).popup( "close" );
  locoBlockSelect = sessionStorage.getItem("locoBlockSelect");
  if( locoBlockSelect != "none" ) {
    var cmd = "<lc id=\""+locoBlockSelect+"\" cmd=\"blockside\"/>";
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

function onGuestProt(prot) {
  guestProt = prot;
}

function onGuestSteps(steps) {
  guestSteps = steps;
}

function onAddGuest() {
  $( "#popupGuestLoco" ).popup( "close" );
  var address = document.getElementById("guestAddress").value;
  var id = document.getElementById("guestShortID").value;
  if( address.length == 0 )
    return;
  trace("add guest: "+address+" id="+id);
  var cmd = "<lc id=\""+address+"\" shortid=\""+id+"\" spcnt=\""+guestSteps+"\" prot=\""+guestProt+"\" V=\"0\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function onUserColor() {
  var red   = document.getElementById("userColorRed").value;
  var green = document.getElementById("userColorGreen").value;
  var blue  = document.getElementById("userColorBlue").value;
  localStorage.setItem("red", red);
  localStorage.setItem("green", green);
  localStorage.setItem("blue", blue);
}

function onLocoResetInBlock() {
  $( "#popupBlock" ).popup( "close" );
  onLocoInBlock("");
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
  document.getElementById("stageblockTitle").innerHTML = "<b>" + getString("stageblock") + ": " + sbid + "</b>";
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


function onStageOpenExit() {
  $( "#popupStageBlock" ).popup( "close" );
  sbid = sessionStorage.getItem("stageblock");
  var cmd = "<sb id=\""+sbid+"\" exitstate=\"open\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}


function onStageCloseExit() {
  $( "#popupStageBlock" ).popup( "close" );
  sbid = sessionStorage.getItem("stageblock");
  var cmd = "<sb id=\""+sbid+"\" exitstate=\"closed\"/>";
  worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
}

function onTurntableGotoTrack() {
  $( "#popupTurntable" ).popup( "close" );
  ttid = sessionStorage.getItem("turntable");
  var cmd = "<tt id=\""+ttid+"\" cmd=\""+trackTTSelect+"\"/>";
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

function onOptionShowAllSchedules() {
  var optionShowAllSchedules = document.getElementById("optionShowAllSchedules");
  localStorage.setItem("showallschedules", optionShowAllSchedules.checked ? "true":"false");
  trace("option showallschedules = "+ optionShowAllSchedules.checked );
}

function onOptionModuleView() {
  var optionModuleView = document.getElementById("optionModuleView");
  localStorage.setItem("moduleview", optionModuleView.checked ? "true":"false");
  trace("option moduleview = "+ optionModuleView.checked );
}

function onOptionShowOcc() {
  var optionShowOcc = document.getElementById("optionShowOcc");
  localStorage.setItem("showocc", optionShowOcc.checked ? "true":"false");
  trace("option showocc = "+ optionShowOcc.checked );
}

function onOptionShowRoutes() {
  var optionShowRoutes = document.getElementById("optionShowRoutes");
  localStorage.setItem("showroutes", optionShowRoutes.checked ? "true":"false");
  trace("option showroutes = "+ optionShowRoutes.checked );
}

function onOptionShowRoutesOnSwitches() {
  var optionShowRoutesOnSwitches = document.getElementById("optionShowRoutesOnSwitches");
  localStorage.setItem("showroutesonswitches", optionShowRoutesOnSwitches.checked ? "true":"false");
  trace("option showroutesonswitches = "+ optionShowRoutesOnSwitches.checked );
}

function onOptionAllSpeedSteps() {
  var optionAllSpeedSteps = document.getElementById("optionAllSpeedSteps");
  localStorage.setItem("allspeedsteps", optionAllSpeedSteps.checked ? "true":"false");
  trace("option allspeedsteps = "+ optionAllSpeedSteps.checked );
}

function onOptionSpeedButtons() {
  var optionSpeedButtons = document.getElementById("optionSpeedButtons");
  localStorage.setItem("speedbuttons", optionSpeedButtons.checked ? "true":"false");
  trace("option speedbuttons = "+ optionSpeedButtons.checked );
  document.getElementById("speedSliderDiv").style.display = optionSpeedButtons.checked ? 'none':'block';
  document.getElementById("speedButtonsDiv").style.display = optionSpeedButtons.checked ? 'block':'none';
}

function initThrottleStatus() {
  trace("init throttle status: " + locoSelected );
  var lc = lcMap[locoSelected]
  if( lc == undefined )
    lc = carMap[locoSelected];
  if( lc != undefined ) {
    var locoStatus = document.getElementById("locoStatus");
    var locoDescription = document.getElementById("locoDescription");
    var locoConsist = document.getElementById("locoConsist");

    var modeLabel = "";
    var modeColor = redBackground;
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
    var consist = lc.getAttribute('consist');
    if( consist == undefined || consist.length == 0 ) {
      locoConsist.innerHTML = findMaster(lc.getAttribute('id'));
    }
    else 
      locoConsist.innerHTML = consist;
    trace("init throttle status: " + lc.getAttribute('id') + " [" + mode + "]" + fromTo);
  }
  else {
    var locoStatus = document.getElementById("locoStatus");
    var locoDescription = document.getElementById("locoDescription");
    var locoConsist = document.getElementById("locoConsist");
    locoStatus.style.backgroundColor = "transparent";
    locoStatus.innerHTML = "";
    locoDescription.innerHTML = "";
    locoConsist.innerHTML = "";
  }
}

function updateSpeed(lc) {
  var slider = document.getElementById("speedSlider");
  var V = parseInt(lc.getAttribute('V'));
  var vVal = V * (100/parseInt(lc.getAttribute('V_max')));
  trace("V="+V+" vVal="+vVal+" V_max="+lc.getAttribute('V_max'));
  slider.value = vVal;
  $("#speedSlider").slider("refresh");

}


function onMaster() {
  if( masterSelected.length > 0 ) {
    locoSelected = masterSelected;
    initThrottle();
    updateDir();
    updateFunctionLabels();
  }
}


function onConsistView() {
  locoConsistAction = 'show';
  var lc = lcMap[locoSelected];
  locoConsistMembers = lc.getAttribute('consist');
  $( "#popupConsist" ).popup( "close" );
  trace("open loco select");
  $('#popupConsist').on("popupafterclose", function(){
    $('#popupConsist').unbind( "popupafterclose" );
    $( "#popupConsist" ).popup( "open" );
    });
  $('#popupConsist').on("popupafterclose", function(){
    $('#popupConsist').unbind( "popupafterclose" );
    $( "#popupConsist" ).popup( "open" );
    });
}


function onConsistAdd() {
  
}


function onConsistDel() {
  
}


function initConsist() {
  trace("consist master loco: " + locoSelected );
  var img = document.getElementById("locoImageConsist");
  var title = document.getElementById("consistTitle");
  var lc = lcMap[locoSelected]
  if( lc == undefined ) {
    img.src = "noimg.png";
    title.innerHTML = getString("consist");
    return;
  }
  img.src = lc.getAttribute('image');
  title.innerHTML = getString("consist") +": "+locoSelected;
}


function initThrottle() {
  trace("locoSelect: " + locoSelected );
  masterSelected = "";
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

    updateSpeed(lc);
    updateDir();
    initThrottleStatus();
  }
  else {
    var img = document.getElementById("locoImage");
    img.src = "noimg.png";
    document.getElementById("speedSlider").value = "0";
    $("#speedSlider").slider("refresh");
    initThrottleStatus();
  }
}


var speedUpdateVal = 0;
function onVUp() {
  speedUpdateVal++;
  if(speedUpdateVal > 100)
    speedUpdateVal = 100;
  speedUpdate(speedUpdateVal);
}

function onVDown() {
  speedUpdateVal--;
  if(speedUpdateVal < 0)
    speedUpdateVal = 0;
  speedUpdate(speedUpdateVal);
}

/* Initial functions */
$(document).delegate(".ui-page", "pagebeforeshow", function () {
  var color = localStorage.getItem("color");
  if( color == undefined ) {
    color = "none";
    localStorage.setItem("color", color);
  }
  if( color == "green" )
    $(this).css('background', '#CCEECC');
  else if( color == "grey" )
    $(this).css('background', '#CCCCCC');
  else if( color == "white" )
    $(this).css('background', '#FFFFFF');
  else if( color == "blue" )
    $(this).css('background', '#CCCCDD');
  else if( color == "user" ) {
    var red   = localStorage.getItem("red");
    var green = localStorage.getItem("green");
    var blue  = localStorage.getItem("blue");
    if( red == undefined || green == undefined || blue == undefined ) {
      red = 0;
      green = 0;
      blue = 0;
    }
    $(this).css('background', "rgb("+red+","+green+","+blue+")");
  }
});

$(document).on("pagecreate",function(){

  // jQuery events go here...
  $("#speedSlider").on( "slidestop", function( event, ui ) {
    value = this.value;
    speedUpdate(value);
  } );
  
  $('#speedSlider').change(function(){
    if( localStorage.getItem("allspeedsteps") != "true" ) {
      return;
    }
    var value = parseInt($(this).val());
    if( value < speedUpdateVal - speedStepDelta || value > speedUpdateVal + speedStepDelta) {
      speedUpdateVal = value;
      speedUpdate(value);
    }
    })
  
  $('#locoBlockSelect').change(function() {
    $( "#popupBlock" ).popup( "close" );
    locoBlockSelect = this.value;
    trace("locoBlockSelect: " + locoBlockSelect);
    sessionStorage.setItem("locoBlockSelect", locoBlockSelect);
    (locoBlockSelect);
  } );
  
  $('#scheduleBlockSelect').change(function() {
    scheduleBlockSelect = this.value;
    trace("scheduleBlockSelect: " + scheduleBlockSelect);
    sessionStorage.setItem("scheduleBlockSelect", scheduleBlockSelect);
  } );
  
  $('#blockBlockSelect').change(function() {
    blockBlockSelect = this.value;
    trace("blockBlockSelect: " + blockBlockSelect);
    sessionStorage.setItem("blockBlockSelect", blockBlockSelect);
  } );
  
  $('#trainBlockSelect').change(function() {
    trainBlockSelect = this.value;
    trace("trainBlockSelect: " + trainBlockSelect);
    sessionStorage.setItem("trainBlockSelect", trainBlockSelect);
  } );
  
  $('#trackTTSelect').change(function() {
    trackTTSelect = this.value;
    trace("trackTTSelect: " + trackTTSelect);
    sessionStorage.setItem("trackTTSelect", trackTTSelect);
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

  $('#levelSelect').change(function() {
    trace("levelSelect: " + this.value );
    actionLevelSelect(this.value);
  } );

  $('#colorSelect').change(function() {
    trace("colorSelect: " + this.value );
    localStorage.color = this.value;
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
  }
  else if( lang == "nl" ) {
    langNL();
  }
  else {
    // default english
    langEN();
  }
  
})

function getString(s) {
  var lang = localStorage.lang;
  
  if( lang == "de" ) {
    if( s == "block" ) return "Block";
    if( s == "diesel" ) return "Diesel";
    if( s == "steam" ) return "Dampf";
    if( s == "electric" ) return "Elektrisch";
    if( s == "car" ) return "Wagen";
    if( s == "consist" ) return "Mehrfachtraktion";
    if( s == "version" ) return "Version";
    if( s == "workspace" ) return "Arbeitsbereich";
    if( s == "stageblock" ) return "Aufstell-Block";
    if( s == "fiddleyard" ) return "Fiddle Yard";
    if( s == "turntable" ) return "Drehscheibe";
    if( s == "text" ) return "Text";
    if( s == "schedule" ) return "Fahrplan";
    if( s == "train" ) return "Zug";
  }
  else if( lang == "en" ) {
    if( s == "block" ) return "Block";
    if( s == "diesel" ) return "Diesel";
    if( s == "steam" ) return "Steam";
    if( s == "electric" ) return "Electric";
    if( s == "car" ) return "Car";
    if( s == "consist" ) return "Consist";
    if( s == "version" ) return "Version";
    if( s == "workspace" ) return "Workspace";
    if( s == "stageblock" ) return "Staging block";
    if( s == "fiddleyard" ) return "Fiddle Yard";
    if( s == "turntable" ) return "Turntable";
    if( s == "text" ) return "Text";
    if( s == "schedule" ) return "Schedule";
    if( s == "train" ) return "Train";
  }
  else if( lang == "nl" ) {
    if( s == "block" ) return "Blok";
    if( s == "diesel" ) return "Diesel";
    if( s == "steam" ) return "Stoom";
    if( s == "electric" ) return "Elektrisch";
    if( s == "car" ) return "Wagon";
    if( s == "consist" ) return "Multitractie";
    if( s == "version" ) return "Versie";
    if( s == "workspace" ) return "Werkruimte";
    if( s == "stageblock" ) return "Opstel blok";
    if( s == "fiddleyard" ) return "Fiddle Yard";
    if( s == "turntable" ) return "Draaischijf";
    if( s == "text" ) return "Tekst";
    if( s == "schedule" ) return "Dienstrooster";
    if( s == "train" ) return "Treinstel";
  }

  return s;
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
   console.log("exception: " + e.stack);
 }
}


/* Utilities */
function xml2string(node) {
  if (typeof(XMLSerializer) !== 'undefined') {
     var serializer = new XMLSerializer();
     return serializer.serializeToString(node);
  } 
  else if (node.xml) {
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
    updateSensorOcc(fbNode);
  }
  else {
    trace("sensor: " + fb.getAttribute('id') + " not found");
  }
}


function getTextFormat(tx) {
  var text = tx.getAttribute('text');
  if( text == undefined )
    return "";
  
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
        div.innerHTML = "<div class='imageV'><img width='"+div.style.height+"' src='"+text+"'/></div>";
      else
        div.innerHTML = "<div><img height='"+div.style.height+"' src='"+text+"'/></div>";
    }
    else {  
      var pointsize = txNode.getAttribute('pointsize');
      if( pointsize == undefined || pointsize == "0")
        pointsize = "10";
      if( text == undefined )
        text = "";
      
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
  trace("switch event: " + sw.getAttribute('id') + " state=" + sw.getAttribute('state') + " set=" + sw.getAttribute('set'));
  var div = document.getElementById("sw_"+sw.getAttribute('id'));
  if( div != null ) {
    swNode = swMap[sw.getAttribute('id')];
    swNode.setAttribute('state', sw.getAttribute('state'));
    if( sw.getAttribute('set') != undefined )
      swNode.setAttribute('set', sw.getAttribute('set'));
    div.style.backgroundImage = getSwitchImage(swNode, div, true);
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

function findMaster(lcid) {
  for (var key in lcMap) {
    var lc = lcMap[key];
    var consist = lc.getAttribute('consist');
    if( consist != undefined && consist.indexOf(lcid) != -1 ) {
      masterSelected = lc.getAttribute('id');
      return "<b>" + lc.getAttribute('id') + "</b>=" + consist;
    }
  }
  return "";
}

function handleLoco(lc) {
  var lcNode = lcMap[lc.getAttribute('id')]
  if( lc == undefined)
    lc = carMap[fn.getAttribute('id')];

  
  lcNode.setAttribute('mode', lc.getAttribute('mode'));
  lcNode.setAttribute('modereason', lc.getAttribute('modereason'));
  lcNode.setAttribute('V', lc.getAttribute('V'));
  lcNode.setAttribute('dir', lc.getAttribute('dir'));
  lcNode.setAttribute('fn', lc.getAttribute('fn'));
  lcNode.setAttribute('destblockid', lc.getAttribute('destblockid'));
  lcNode.setAttribute('blockid', lc.getAttribute('blockid'));
  lcNode.setAttribute('blockenterside', lc.getAttribute('blockenterside'));
  lcNode.setAttribute('train', lc.getAttribute('train'));

  var bk = findBlock4Loco(lc.getAttribute('id'));
  if( bk != undefined ) {
    updateBlockstate(bk.getAttribute('id'), bk.getAttribute('statesignal'), lc.getAttribute('id'), "loco");
    if( lc.getAttribute('blockenterside') != undefined ) {
      var div = document.getElementById("bk_"+bk.getAttribute('id'));
      getBlockLabel(bk, div);
    }
  }
  
  if( lc.getAttribute('id') == locoSelected ) {
    initThrottleStatus();
    updateDir();
    if( localStorage.getItem("allspeedsteps") != "true" ) {
      updateSpeed(lcNode);
    }
  }
}


function handleFunction(fn) {
  trace("function event: " + fn.getAttribute('id') + " changed=" + fn.getAttribute('fnchanged'));
  var lc = lcMap[fn.getAttribute('id')];
  if( lc == undefined)
    lc = carMap[fn.getAttribute('id')];
  
  if( lc == undefined)
    return;
  
  var fnchanged = parseInt(fn.getAttribute('fnchanged'));
  var on = "false";
  
  if( fnchanged > 0 ) {
    var fx = parseInt(lc.getAttribute('fx'));
    var mask = 1 << (fnchanged-1); 
    on = fn.getAttribute("f"+fnchanged);
    
    if( on == "true")
      fx = fx | mask;
    else
      fx = fx & ~mask;
    lc.setAttribute('fx', ""+fx)
    trace("mask="+mask.toString(16)+" fnchanged="+fnchanged+" fx="+fx.toString(16));
  }
  else if( fnchanged == 0 ) {
    on = fn.getAttribute("f0");
    lc.setAttribute("fn", fn.getAttribute("f0"));
  }

  if( fn.getAttribute('id') == locoSelected ) {
    if(fnchanged > 14)
      fnchanged -= 14;
    var div = document.getElementById("F"+fnchanged);
    if( on == "true")
      div.style.backgroundColor = redBackground;
    else
      div.style.backgroundColor = ''
  }
}

function updateBlockstate( bkid, sgid, lcid, from ) {
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
  trace("blockstate: block="+bkid+" signal="+sgid+" loco="+lcid+" from="+from+" label="+label);
  div.innerHTML = "<label class='itemtext'>"+label+"</label>";
  forceRedraw(div);
}

function getBlockLabel(bk, div) {
  bkNode = bkMap[bk.getAttribute('id')];
  var ori   = getOri(bkNode);
  var label = bk.getAttribute('locid');
  if( label == undefined || label.length == 0 )
    label = bk.getAttribute('id');
  else {
    var lc = lcMap[label];
    if( lc != undefined ) {
      var rotate = lc.getAttribute('blockenterside');
      var train  = lc.getAttribute('train');
      
      if( train != undefined && train.length > 0 )
        label = label + "_" + train;
      
      if( rotate == undefined )
        rotate = "true";
      
      if( "true" != bk.getAttribute('reserved') ) {
        var newLabel = "";
        if( ori=="east" || ori=="west" ) {
          newLabel = "< " + label;
          if( (ori=="east" && rotate=="false" ) || (ori=="west" && rotate=="true" ) ) {
            newLabel = label + " >";
          }
        }
        else if( ori=="north" || ori=="south" ) {
          newLabel = "< " + label;
          if( (ori=="north" && rotate=="false" ) || (ori=="south" && rotate=="true" ) ) {
            newLabel = label + " >";
          }
        }
        label = newLabel;
      }
      
    }
  }
  
  
  if( ori == "north" || ori == "south" ) {
    // Work around for rotated labels:
    label = label.split(' ').join('.');
    label = label.split('-').join('.');
    
    var labdiv = document.createElement('div');
    labdiv.setAttribute('class', "itemtextV");
    if( bkNode.getAttribute('smallsymbol') == "true" ) {
      labdiv.style.width    = "64px";
      labdiv.style.top      = "16px";
    }
    else {
      labdiv.style.width    = "128px";
      labdiv.style.top      = "48px";
    }
    labdiv.style.height   = "32px";
    labdiv.style.fontSize = ""+blockPointsize+"px";
    labdiv.style.position = "absolute";
    labdiv.style.left     = "0px";
    labdiv.innerHTML      = label;
    div.innerHTML      = "";
    div.appendChild(labdiv);
  }
  else {
    div.innerHTML      = "<label class='itemtext' style='font-size:"+blockPointsize+"px;'>"+label+"</label>";
  }
}


function updateBlockOcc(bk) {
  if( localStorage.getItem("showocc") != "true" ) {
    return;
  }
  
  var occ = "false";
  if( bk.getAttribute('locid') != undefined && bk.getAttribute('locid').length > 0 )
    occ = "true";

  for (var key in tkMap) {
    var tk = tkMap[key];
    var blockid = tk.getAttribute('blockid');
    if( blockid == undefined )
      continue;
    if( blockid == bk.getAttribute('id') ) {
      tk.setAttribute('occ', occ)
      handleTrack(tk);
    }
  }
  for (var key in fbMap) {
    var fb = fbMap[key];
    var blockid = fb.getAttribute('blockid');
    if( blockid == undefined )
      continue;
    if( blockid == bk.getAttribute('id') ) {
      fb.setAttribute('occ', occ)
      handleSensor(fb);
    }
  }
  for (var key in sgMap) {
    var sg = sgMap[key];
    var blockid = sg.getAttribute('blockid');
    if( blockid == undefined )
      continue;
    if( blockid == bk.getAttribute('id') ) {
      sg.setAttribute('occ', occ)
      handleSignal(sg);
    }
  }
  for (var key in swMap) {
    var sw = swMap[key];
    var blockid = sw.getAttribute('blockid');
    if( blockid == undefined )
      continue;
    if( blockid == bk.getAttribute('id') ) {
      sw.setAttribute('occ', occ)
      handleSwitch(sw);
    }
  }
  
}


function updateSensorOcc(fb) {
  if( localStorage.getItem("showocc") != "true" ) {
    return;
  }
  
  var occ = "false";
  if( fb.getAttribute('state') != undefined && fb.getAttribute('state') == "true" )
    occ = "true";

  for (var key in tkMap) {
    var tk = tkMap[key];
    var blockid = tk.getAttribute('blockid');
    if( blockid == undefined )
      continue;
    if( blockid == fb.getAttribute('id') ) {
      tk.setAttribute('occ', occ)
      handleTrack(tk);
    }
  }
  for (var key in sgMap) {
    var sg = sgMap[key];
    var blockid = sg.getAttribute('blockid');
    if( blockid == undefined )
      continue;
    if( blockid == fb.getAttribute('id') ) {
      sg.setAttribute('occ', occ)
      handleSignal(sg);
    }
  }
  
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
    
    getBlockLabel(bk, div);
    div.style.backgroundImage = getBlockImage(bkNode, div);
    forceRedraw(div);
  }
  else {
    trace("block: " + bk.getAttribute('id') + " not found");
  }
  
  updateBlockstate( bkNode.getAttribute('id'), bkNode.getAttribute('statesignal'), bkNode.getAttribute('locid'), "block");
  
  updateBlockOcc(bkNode);

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

function getStageLabel(sb, div) {
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
  var exitstate = sb.getAttribute('exitstate');
  var labelsuffix = "";
  if( exitstate != undefined && exitstate == "closed" )
    labelsuffix = "<";
  var label = sb.getAttribute('locid');
  if( label == undefined || label.length == 0 )
    label = sb.getAttribute('id') + "[" + lcCount + "]";
  label += labelsuffix;
  if( ori == "north" || ori == "south" )
    div.innerHTML      = "<div class='itemtextV' style='font-size:"+blockPointsize+"px;'>"+label+"</div>";
  else
    div.innerHTML      = "<label class='itemtext' style='font-size:"+blockPointsize+"px;'>"+label+"</label>";
}

function handleStageBlock(sb) {
  trace("staging block event: " + sb.getAttribute('id') + " " + sb.getAttribute('state'));
  var div = document.getElementById("sb_"+sb.getAttribute('id'));
  if( div != null ) {
    sbNode = sbMap[sb.getAttribute('id')];
    sbNode.setAttribute('state', sb.getAttribute('state'));
    if( sb.getAttribute('locid') != undefined )
      sbNode.setAttribute('locid', sb.getAttribute('locid'));
    sbNode.setAttribute('reserved', sb.getAttribute('reserved'));
    sbNode.setAttribute('entering', sb.getAttribute('entering'));
    
    getStageLabel(sb, div);
    
    div.style.backgroundImage = getStageBlockImage(sbNode, div);
    forceRedraw(div);

  }
  else {
    trace("staging block: " + sb.getAttribute('id') + " not found");
  }
}

function handleModel(model) {
  var cmd = model.getAttribute('cmd');
  trace("model: "+cmd);
  if( cmd == "add" ) {
    lclist = model.getElementsByTagName("lc");
    for( var i = 0; i < lclist.length; i++ ) {
      var lc = lcMap[lclist[i].getAttribute('id')];
      if( lc != undefined ) {
        trace("loco "+lclist[i].getAttribute('id')+" already exist");
        continue;
      }
      trace('add loco: ' + lclist[i].getAttribute('id') );
      lcMap[lclist[i].getAttribute('id')] = lclist[i];
      addLocoToList(lclist[i]);
    }
  }
}

function handleRoute(st) {
  var stid = st.getAttribute('id');
  stNode = stMap[stid];
  if( stNode != undefined ) {
    var locked = st.getAttribute('status');
    if( locked != undefined ) {
      stNode.setAttribute('status', locked);
  
      if( localStorage.getItem("showroutes") == "true" ) {
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
      
      if( localStorage.getItem("showroutesonswitches") == "true" ) {
        for (var key in swMap) {
          var sw = swMap[key];
          var routeids = sw.getAttribute('routeids');
          if( routeids == undefined )
            continue;
          if( routeids.indexOf(stid) != -1 ) {
            sw.setAttribute('route', locked)
            handleSwitch(sw);
          }
        }
      }
    }

    var div = document.getElementById("st_"+st.getAttribute('id'));
    if( div != undefined ) {
      div.style.backgroundImage = getRouteImage(st);
      forceRedraw(div);
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
    document.getElementById("headerPower").style.backgroundColor= redBackground;
  else 
    document.getElementById("headerPower").style.backgroundColor= '';
}

function handleAuto(auto) {
  autoMode = auto.getAttribute('cmd');
  trace("auto: " + autoMode );
  if( autoMode == "on" )
    document.getElementById("headerAuto").style.backgroundColor= redBackground;
  else 
    document.getElementById("headerAuto").style.backgroundColor= '';
}

function handleSystem(sys) {
  //<sys cmd="shutdown" informall="true"/>
  var cmd = sys.getAttribute('cmd');
  if( cmd == "shutdown" ) {
    trace("server shutdown");
    Disconnect(false);
  }
}

function Disconnect(closemenu) {
  var cmd = "<sys shutdown=\"true\"/>";
  worker.postMessage(JSON.stringify({type:'shutdown', msg:cmd}));
  for( var i = 0; i < zlevelDivList.length; i++ )
    zlevelDivList[i].style.display = 'none';
  if(closemenu)
    $( "#popupMenu" ).popup( "close" );

}

function onZoom(zoomin) {
  trace("scale="+scale);
  if( zoomin && scale < 2.0 ) {
    scale += 0.1;
    if( scale > 2.0 )
      scale = 1.0 + 0.1;
    localStorage.setItem("scale", scale);
    for( var i = 0; i < zlevelDivList.length; i++ )
      $(zlevelDivList[i]).css({'-webkit-transform': 'scale(' + scale + ')'});
  }
  else if( !zoomin && scale > 0.5 ) {
    scale -= 0.1;
    localStorage.setItem("scale", scale);
    for( var i = 0; i < zlevelDivList.length; i++ )
      $(zlevelDivList[i]).css({'-webkit-transform': 'scale(' + scale + ')'});
  }
}

function onZoom100() {
  scale = 1.0;
  localStorage.setItem("scale", scale);
  for( var i = 0; i < zlevelDivList.length; i++ )
    $(zlevelDivList[i]).css({'-webkit-transform': 'scale(' + scale + ')'});
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
  else if( evtName == "model" )
    handleModel(root);
}

function processResponse() {
  trace("readyState="+req.readyState+" status="+req.status);
  if (req.readyState == 4 && (req.status == 0 || req.status == 200)) {
    // only if "OK"
    $.mobile.loading("show");
    try {
      xmlDoc = req.responseXML;
      if( xmlDoc != null ) {
        scale = parseFloat(localStorage.getItem("scale"));
        if( scale == undefined || isNaN(scale) || scale == "1")
          scale = 1.0;
        trace("scale="+scale);
        
        var category = localStorage.getItem("category");
        if(category == undefined || category.length == 0) {
          localStorage.setItem("category", "engine");
        }
        var showocc = localStorage.getItem("showocc");
        if(showocc == undefined || showocc.length == 0) {
          localStorage.setItem("showocc", "true");
        }


        planlist = xmlDoc.getElementsByTagName("plan")
        if(planlist.length == 0)
          planlist = xmlDoc.getElementsByTagName("modplan")
          
        if( planlist.length > 0 ) {
          var h = document.getElementById("title");
          donkey = planlist[0].getAttribute('donkey');
          title = planlist[0].getAttribute('title');
          trace("title: "+title);
          rocrailversion = planlist[0].getAttribute('rocrailversion');
          rocrailpwd = planlist[0].getAttribute('rocrailpwd');
          var modplan = planlist[0].getAttribute('modplan');
          
          if( modplan != undefined && modplan == "true") {
            if( localStorage.getItem("moduleview") == "true" ) {
              trace("activating module view");
              ModPlan = true;
              document.getElementById("headerUp").style.display = 'none';
              document.getElementById("headerDown").style.display = 'none';
            }
          }

          var serverInfo = document.getElementById("serverInfo");
          serverInfo.innerHTML = "<table><tr><td>"+getString("version")+":<td>" + rocrailversion + "<tr><td>" + getString("workspace")+":<td>" + rocrailpwd;
          
          
          trace( "processing plan: " + title + " key=" + donkey );
          h.innerHTML = title;
          processPlan();
          planloaded = true;
          locoSelected = localStorage.getItem("locoSelected");
          locoSelectedList[0] = localStorage.getItem("locoSelected0");
          locoSelectedList[1] = localStorage.getItem("locoSelected1");
          locoSelectedList[2] = localStorage.getItem("locoSelected2");
          locoSelectedList[3] = localStorage.getItem("locoSelected3");
          
          var speedbuttons = localStorage.getItem("speedbuttons");
          document.getElementById("speedButtonsDiv").style.display = 'none';
          if( speedbuttons != undefined && speedbuttons == "true" ) {
            document.getElementById("speedSliderDiv").style.display = 'none';
            document.getElementById("speedButtonsDiv").style.display = 'block';
          }
          
          trace("selected loco = " + locoSelected);

          if( donkey == 'true') {
            document.getElementById("donkey").style.display = 'none';
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
        
        var pingTimer = setInterval(function () {doPing()}, (60 * 1000) );
        function doPing() {
          trace("ping...");
          var cmd = "<sys cmd=\"getstate\"/>";
          worker.postMessage(JSON.stringify({type:'command', msg:cmd}));
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
  var occ   = fb.getAttribute('occ');
  var accnr = parseInt(fb.getAttribute('accnr'));
  var ori   = getOri(fb);
  var suffix = ""; 
  
  if( route=="1" )
    suffix = "-route";
  else if( occ == "true" )
    suffix = "-occ";

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

function getRouteImage(st) {
  var ori    = getOri(st);
  var status = st.getAttribute('status');
  var suffix = '';
  if( status == undefined )
    status = "0";
  trace("route status = "+status);
  if( status == "0")
    status = "free";
  else if( status == "1")
    status = "locked";
  else if( status == "2")
    status = "selected";
  else if( status == "3")
    status = "deselected";
  else if( status == "4")
    status = "closed";
  trace("route status = "+status);
  return "url('route-"+status+"."+ ori + ".svg')";
}

function getSignalImage(sg) {
  var ori     = getOri(sg);
  var state   = sg.getAttribute('state');
  var signal  = sg.getAttribute('signal');
  var type    = sg.getAttribute('type');
  var aspects = sg.getAttribute('aspects');
  var pattern = parseInt( sg.getAttribute('usepatterns') );
  var route   = sg.getAttribute('route');
  var occ     = sg.getAttribute('occ');
  var suffix = ""; 
  
  if( route=="1" )
    suffix = "-route";
  else if( occ == "true" )
    suffix = "-occ";

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
    if( state == "red"  )
      return "url('"+type+"shunting-2-"+"r"+suffix+"."+ ori + ".svg')";
    else
      return "url('"+type+"shunting-2-"+"w"+suffix+"."+ ori + ".svg')";
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
  var ori    = getOri(tk);
  var type   = tk.getAttribute('type');
  var route  = tk.getAttribute('route');
  var occ    = tk.getAttribute('occ');
  var suffix = ""; 
  
  if( route=="1" )
    suffix = "-route";
  else if( occ == "true" )
    suffix = "-occ";
  
  if( type == "curve" || type == "buffer" || type == "connector" || type == "dcurve" || type == "dir" || type == "dirall" ) {
    return "url('"+type+suffix+"."+ori+".svg')";
  }
  else if( type == "concurveleft" ) {
    return "url('"+"connector-curve-left"+suffix+"."+ori+".svg')";
  }
  else if( type == "concurveright" ) {
    return "url('"+"connector-curve-right"+suffix+"."+ori+".svg')";
  }
  else if (type == "tracknr" ) {
    var tknr = parseInt(tk.getAttribute('tknr'));
    return "url('track-"+tknr+suffix+"."+ ori + ".svg')";
  }
  else {
    return "url('straight"+suffix+"."+ ori + ".svg')";
  }
}

function getSwitchImage(sw, div, checkSet) {
  var ori    = getOri(sw);
  var type   = sw.getAttribute('type');
  var swtype = sw.getAttribute('swtype');
  var state  = sw.getAttribute('state');
  var set    = sw.getAttribute('set');
  var accnr  = sw.getAttribute('accnr');
  var dir    = sw.getAttribute('dir');
  var rectc  = sw.getAttribute('rectcrossing');
  var addr1  = sw.getAttribute('addr1');
  var port1  = sw.getAttribute('port1');
  var route  = sw.getAttribute('route');
  var occ    = sw.getAttribute('occ');
  var raster = false;
  var rasterStr = "";
  var suffix    = "-route";
  var nomotor   = false;
  
  if( localStorage.getItem("showroutesonswitches") == "true" ) {
    suffix = "";
  }
  
  if( route=="1" )
    suffix = "-route";
  else if( occ == "true" )
    suffix = "-occ";

  if( swtype != undefined && swtype == "raster" ) {
    var suffix = "";
    rasterStr  = "-r";
    raster = true;
  }

  if( ( (addr1 == undefined) || (addr1 == "0") ) && ( (port1 == undefined) || (port1 == "0") ) )
    nomotor = true;

  if( checkSet && set != undefined && set != null && set == "false" ) {
    trace("set="+set+" checkSet="+checkSet);
    div.style.backgroundColor = "red";
  }
  else 
    div.style.backgroundColor = "transparent";

  
  trace("switch type: " + type + " accnr="+accnr);
  if( accnr != undefined && parseInt(accnr) > 1 )
    type = "accessory";
  
  if (type=="right") {
    if (state=="straight")
      return "url('turnoutright"+rasterStr+suffix+"."+ori+".svg')";
    else
      return "url('turnoutright"+rasterStr+"-t"+suffix+"."+ori+".svg')";
  }
  else if (type=="left") {
    if (state=="straight")
      return "url('turnoutleft"+rasterStr+suffix+"."+ori+".svg')";
    else
      return "url('turnoutleft"+rasterStr+"-t"+suffix+"."+ori+".svg')";
  }
  else if (type=="twoway") {
    if (state=="straight")
      return "url('twoway"+rasterStr+"-tr"+suffix+"."+ori+".svg')";
    else
      return "url('twoway"+rasterStr+"-tl"+suffix+"."+ori+".svg')";
  }
  else if (type=="threeway") {
    if (state=="left")
      return "url('threeway"+rasterStr+"-tl"+suffix+"."+ori+".svg')";
    else if (state=="right")
      return "url('threeway"+rasterStr+"-tr"+suffix+"."+ori+".svg')";
    else
      return "url('threeway"+rasterStr+suffix+"."+ori+".svg')";
  }
  else if (type=="crossing") {
    var nullM = "";
    if( nomotor ) {
      if( rectc != "true") {
        nullM = "0m";
        if( rasterStr == "-r" )
          rasterStr += "-";
      }
      state = "straight";
      if( localStorage.getItem("showroutesonswitches") != "true" )
        suffix = "";
    }
    
    if( rectc == "true") {
      if (state=="straight")
        return "url('crossing"+rasterStr+nullM+suffix+"."+ori+".svg')";
      else
        return "url('crossing"+rasterStr+"-t"+suffix+"."+ori+".svg')";
    }
    else {
      if( ori == "west" || ori == "east") {
        div.style.width    = raster?"32px":"64px";
        div.style.height   = "32px";
      }
      else {
        div.style.width    = "32px";
        div.style.height   = raster?"32px":"64px";
      }
      var direction = (dir == "true" ? "left":"right");
      if (state=="turnout")
        return "url('crossing"+direction+rasterStr+"-t"+suffix+"."+ori+".svg')";
      else
        return "url('crossing"+direction+rasterStr+nullM+suffix+"."+ori+".svg')";
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
      div.style.width    = raster?"32px":"64px";
      div.style.height   = "32px";
    }
    else {
      div.style.width    = "32px";
      div.style.height   = raster?"32px":"64px";
    }
    var direction = (dir == "true" ? "left":"right");
    if (state=="left")
      return "url('dcrossing"+direction+rasterStr+"-tl"+suffix+"."+ori+".svg')";
    else if (state=="right")
      return "url('dcrossing"+direction+rasterStr+"-tr"+suffix+"."+ori+".svg')";
    else if (state=="turnout")
      return "url('dcrossing"+direction+rasterStr+"-t"+suffix+"."+ori+".svg')";
    else
      return "url('dcrossing"+direction+rasterStr+suffix+"."+ori+".svg')";
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
    else if( accnr == "41" ) {
      if( ori == "west" || ori == "east") {
        div.style.width    = "128px";
        div.style.height   = "32px";
      }
      else {
        div.style.width    = "32px";
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

  if( "closed" == sb.getAttribute('state') )
    return "url('stage-closed"+"."+ori+".svg')";
  else if( label != undefined && label.length > 0 ) {
    if( "true" == sb.getAttribute('reserved') )
      return "url('stage-res"+"."+ori+".svg')";
    else if( "true" == sb.getAttribute('entering') )
      return "url('stage-ent"+"."+ori+".svg')";
    else 
      return "url('stage-occ"+"."+ori+".svg')";
  }
  else
    return "url('stage"+"."+ori+".svg')";
  
}

function addCatToList(div, lcCat) {
  var newdiv = document.createElement('div');
  newdiv.setAttribute('data-role', "collapsible");
  newdiv.setAttribute('class', "ui-collapsible ui-collapsible-inset ui-corner-all ui-collapsible-themed-content ui-collapsible-collapsed ui-first-child");
  var h2 = document.createElement('h2');
  h2.innerHTML = getString(lcCat);
  newdiv.appendChild(h2);
  var ul = document.createElement('ul');
  ul.setAttribute('id', lcCat);
  ul.setAttribute('data-role', "listview");
  ul.setAttribute('class', "ui-listview");
  ul.setAttribute('style', "max-height: 200px; overflow: auto;");
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

function initLocoList() {
  trace("initLocoList...");
  var locoSelectList = document.getElementById("locoSelectList");
  locoSelectList.innerHTML = "";
  for (var key in lcCatMap) delete lcCatMap[key];
  lcCatMap.length = 0;
  for (var key in lcMap) {
    var lc = lcMap[key];
    addLocoToList(lc);
  }
  for (var key in carMap) {
    var car = carMap[key];
    addLocoToList(car);
  }
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

/* Process the plan.xml 
 * 
 * 
  <zlevel title="Module 1" modid="m6" modnr="6" z="0" modviewx="0" modviewy="0" modviewcx="13" modviewcy="4"/>
  <zlevel title="Module 1" modid="m5" modnr="5" z="1" modviewx="13" modviewy="1" modviewcx="9" modviewcy="3"/>
 * 
 * */

function setXY(item, zlevel, div) {
  var zX = 0;
  var zY = 0;
  if( ModPlan ) {
    zX = parseInt(zlevel.getAttribute('modviewx'));
    zY = parseInt(zlevel.getAttribute('modviewy'));
  }
  div.style.left = "" + ((parseInt(item.getAttribute('x')) + zX) * 32) + "px";
  div.style.top  = "" + ((parseInt(item.getAttribute('y')) + zY) * 32) + "px";
}


function processPlan() {
  
   try {
     xmlDoc = req.responseXML;
     
     zlevellist = xmlDoc.getElementsByTagName("zlevel");
     if( zlevellist.length > 0 ) {
       var title = "";
       trace("processing " + zlevellist.length + " zlevels");
       for (var i = 0; i < zlevellist.length; i++) {
         var z = zlevellist[i].getAttribute('z');
         if( z == undefined )
           z = "0";
         trace('zlevel: ' + z + " title: " + zlevellist[i].getAttribute('title')  + " " +xml2string(zlevellist[i]) );
         zlevelMap[z]  = zlevellist[i];
         zlevelList[i] = zlevellist[i];

         var newdiv = document.createElement('div');
         newdiv.setAttribute('id', "level_" + z);
         newdiv.setAttribute('overflow-x', "auto");
         newdiv.setAttribute('overflow-y', "auto");
         newdiv.style.position = "absolute";
         newdiv.style.left = "0px";
         newdiv.style.top = ""+yoffset+"px";

         //newdiv.style.background = '#ff9999';
         //newdiv.style.transform = 'scale(.5)';
         //$('#level_0').css({ transform: 'scale(.5)' });
         $(newdiv).css({'-webkit-transform': 'scale(' + scale + ')'});
         
         zlevelDivMap[z] = newdiv;
         zlevelDivList[i] = newdiv;

         if( zlevelSelected == 'none' ) {
           zlevelSelected = newdiv;
           zlevelIdx = i;
           title = zlevellist[i].getAttribute('title');
         }
         else if( !ModPlan ) {
           trace("disable level " + z);
           newdiv.style.display = 'none'
         }
         document.body.appendChild(newdiv);
       }
       if( !ModPlan ) {
         var h = document.getElementById("title");
         h.innerHTML = title;
       }
     }
     
     
     lclist = xmlDoc.getElementsByTagName("lc");
     if( lclist.length > 0 )
       trace("processing " + lclist.length + " locos");

     for (var i = 0; i < lclist.length; i++) {
       var show  = lclist[i].getAttribute('show');
       if( show != undefined && show == "false" )
         continue;

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
       newdiv.setAttribute('onmousedown', "actionOutputDown(this.id)");
       newdiv.setAttribute('onmouseup', "actionOutputUp(this.id)");
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       newdiv.style.width    = "32px";
       newdiv.style.height   = "32px";
       setXY(colist[i], zlevelMap[z], newdiv);
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
       setXY(sglist[i], zlevelMap[z], newdiv);
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
       newdiv.style.width    = ""+32+"px";
       newdiv.style.height   = ""+32+"px";
       setXY(tklist[i], zlevelMap[z], newdiv);
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
       var input = txlist[i].getAttribute('manualinput');
       if( input != undefined && input == "true")
         newdiv.setAttribute('onClick', "actionText(this.id)");
       
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
       setXY(txlist[i], zlevelMap[z], newdiv);

       if( text != undefined && text.indexOf(".png") != -1 ) {
         //newdiv.style.backgroundImage = "url('"+text+"')";
         newdiv.style.backgroundSize = newdiv.style.width;
         if( ori == "north" || ori == "south" )
           newdiv.innerHTML = "<div class='imageV'><img height='"+newdiv.style.width+"' src='"+text+"'/></div>";
         else
           newdiv.innerHTML = "<div><img height='"+newdiv.style.height+"' src='"+text+"'/></div>";
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
       var show  = swlist[i].getAttribute('show');
       if( show != undefined && show == "false" )
         continue;
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
       setXY(swlist[i], zlevelMap[z], newdiv);
       newdiv.innerHTML      = "";
       
       newdiv.style.backgroundImage = getSwitchImage(swlist[i], newdiv, false);
       trace("Switch image="+newdiv.style.backgroundImage);

       leveldiv.appendChild(newdiv);
     }
     
     
     fblist = xmlDoc.getElementsByTagName("fb");
     if( fblist.length > 0 )
       trace("processing " + fblist.length + " sensors");

     for (var i = 0; i < fblist.length; i++) {
       var show  = fblist[i].getAttribute('show');
       if( show != undefined && show == "false" )
         continue;
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
       setXY(fblist[i], zlevelMap[z], newdiv);
       newdiv.innerHTML      = "";
       
       newdiv.style.backgroundImage = getSensorImage(fblist[i], 0);
       //trace("Sensor image="+newdiv.style.backgroundImage);

       leveldiv.appendChild(newdiv);
       //document.body.appendChild(newdiv);
       updateSensorOcc(fblist[i]);
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
       setXY(bklist[i], zlevelMap[z], newdiv);
       newdiv.style.backgroundImage = getBlockImage(bklist[i], newdiv);
       newdiv.style.lineHeight = newdiv.style.height;

       getBlockLabel(bklist[i], newdiv);
       
       leveldiv.appendChild(newdiv);
       updateBlockOcc(bklist[i]);
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
       setXY(ttlist[i], zlevelMap[z], newdiv);
       newdiv.innerHTML  = getTurntableImage(ttlist[i], newdiv); 
       leveldiv.appendChild(newdiv);
     }
     
     
     sclist = xmlDoc.getElementsByTagName("sc");
     if( sclist.length > 0 )
       trace("processing " + sclist.length + " schedules");
     for (var i = 0; i < sclist.length; i++) {
       scMap[sclist[i].getAttribute('id')] = sclist[i];
     }
     
     
     tourlist = xmlDoc.getElementsByTagName("tour");
     if( tourlist.length > 0 )
       trace("processing " + tourlist.length + " tours");
     for (var i = 0; i < tourlist.length; i++) {
       tourMap[tourlist[i].getAttribute('id')] = tourlist[i];
     }
     
     
     locationlist = xmlDoc.getElementsByTagName("location");
     if( locationlist.length > 0 )
       trace("processing " + locationlist.length + " locations");
     for (var i = 0; i < locationlist.length; i++) {
       locationMap[locationlist[i].getAttribute('id')] = locationlist[i];
     }
     
     
     operatorlist = xmlDoc.getElementsByTagName("operator");
     if( operatorlist.length > 0 )
       trace("processing " + operatorlist.length + " operators");
     for (var i = 0; i < operatorlist.length; i++) {
       operatorMap[operatorlist[i].getAttribute('id')] = operatorlist[i];
     }
     
     
     stlist = xmlDoc.getElementsByTagName("st");
     if( stlist.length > 0 )
       trace("processing " + stlist.length + " routes");
     for (var i = 0; i < stlist.length; i++) {
       stMap[stlist[i].getAttribute('id')] = stlist[i];
       var show = stlist[i].getAttribute('show');
       if( show != undefined && show == "true" ) {
         var z     = stlist[i].getAttribute('z');
         var ori   = getOriNr(stlist[i].getAttribute('ori'));
         if( z == undefined )
           z = '0';
         var leveldiv = zlevelDivMap[z]; 
         trace('route: ' + stlist[i].getAttribute('id') + "at level " + z);
         if( leveldiv == undefined ) {
           trace("Error: zlevel ["+z+"] does not exist!");
           continue;
         }
         var newdiv = document.createElement('div');
         newdiv.setAttribute('id', "st_"+stlist[i].getAttribute('id'));
         newdiv.setAttribute('onClick', "actionRoute(this.id)");
         newdiv.setAttribute('class', "item");
         newdiv.style.position = "absolute";
         newdiv.style.width    = "32px";
         newdiv.style.height   = "32px";
         setXY(stlist[i], zlevelMap[z], newdiv);
         newdiv.innerHTML      = "";
         
         newdiv.style.backgroundImage = getRouteImage(stlist[i]);
         leveldiv.appendChild(newdiv);
         
       }
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
       setXY(fylist[i], zlevelMap[z], newdiv);
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
       setXY(sblist[i], zlevelMap[z], newdiv);
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

       getStageLabel(sblist[i], newdiv);

       leveldiv.appendChild(newdiv);
     }
     
     
   }
   catch(e) {
     console.log("exception: " + e.stack);
   }

}


