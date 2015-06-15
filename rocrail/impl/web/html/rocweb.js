/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2015 Rob Versluis, Rocrail.net

*/
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
var locoSelectAction = 'none';
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
var clockTimer;
var Divider = 1;
var FGroup = 0;
var rocrailversion = '';
var rocrailpwd = '';
var prevPopup = "";
var guestProt = "P";
var guestSteps = "28";
var trackTTSelect = 'none';
var trackFYSelect = 'none';
var sliderDelta = 3;
var controlCode = "";
var slaveCode = "";
var redBackground = "#FFC8C8";
var greyBackground = "#E1E1E1";
var yellowBackground = "#FFFF00";
var greenBackground = "#C8FFC8";
var blueBackground = "#C8C8FF";

var throttleid = "rocweb";
var speedUpdateVal = 0;
var timelabel = "";
var Title = "Rocrail";
var Time = 0;
var parser = new window.DOMParser();


function trace(msg) {
  var debug = localStorage.getItem("debug");
  if( debug == undefined )
    debug = "false";
  if( debug == "true" )
    console.log(msg);
}

function sendCommand(cmd) {
  // TODO: insert control code.
  worker.postMessage(cmd);
}

/* Configuration */
function langDE() {
  var donkey = "<br>Rocrail wird vollständig auf freiwilliger Basis betrieben. Trotzdem benötigt Rocrail Geld. Ihre fortwährende finanzielle Unterstützung ist notwendig, um Rocrail verfügbar zu halten. Falls Sie bereits einen Beitrag geleistet haben, können Sie einen Schlüssel beantragen, um diesen Dialog zu unterdrücken: donate@rocrail.net<br>";
  donkey += "<div id=\"donkeyWarning\"><b>Rocweb beendet nach 5 Minuten die Kommunikation!</b></div>";
  document.getElementById("donkey").innerHTML = donkey;

  document.getElementById("menuInfo").innerHTML = "Informationen";
  document.getElementById("menuSystem").innerHTML = "System";
  document.getElementById("menuAuto").innerHTML = "Automat";
  document.getElementById("menuOptions").innerHTML = "Optionen";
  document.getElementById("menuGuest").innerHTML = "Gast-Lok";
  //document.getElementById("menuTrains").innerHTML = "Züge";
  //document.getElementById("menuDisconnect").innerHTML = "Server trennen";
  document.getElementById("menuHelp").innerHTML = "Hilfe";
  document.getElementById("systemTitle").innerHTML = "<b>System</b>";
  document.getElementById("autoTitle").innerHTML = "<b>Automat</b>";
  document.getElementById("systemInitField").innerHTML = "Feld initialisieren";
  document.getElementById("systemQuerySensors").innerHTML = "Tages-Anfang";
  document.getElementById("systemEmergencyBreak").innerHTML = "Nothalt";
  document.getElementById("systemStartAll").innerHTML = "Alle Loks starten";
  document.getElementById("systemStopAll").innerHTML = "Alle Loks anhalten";
  document.getElementById("systemSoftReset").innerHTML = "Zurücksetzen; außer Blockbelegung";
  document.getElementById("systemShutdown").innerHTML = "Server herunterfahren";
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
  document.getElementById("labOptionBinState").innerHTML = "BinState Bedienung";
  document.getElementById("labOptionShowBlockID").innerHTML = "Zeige Block Kennungen";
  document.getElementById("labOptionShowTrainID").innerHTML = "Zeige Zug Kennungen";
  document.getElementById("labOptionShowLocoImage").innerHTML = "Zeige Lok-Bild im Block";
  document.getElementById("labOptionAnalogClock").innerHTML = "Modelluhr";
  document.getElementById("labLocoCatEngine").innerHTML = "Antriebsart";
  document.getElementById("labLocoCatEra").innerHTML = "Epoche";
  document.getElementById("labLocoCatRoadname").innerHTML = "Gesellschaft";
  document.getElementById("labLocoSortID").innerHTML = "Kennung";
  document.getElementById("labLocoSortAddress").innerHTML = "Adresse";
  document.getElementById("labBlockStart").innerHTML = "Zug starten";
  document.getElementById("labBlockStop").innerHTML = "Zug anhalten";
  document.getElementById("labBlockManual").innerHTML = "Halbautomatisch";
  document.getElementById("labBlockReset").innerHTML = "Zurücksetzen";
  document.getElementById("labBlockSwapPlacing").innerHTML = "Umdrehen";
  document.getElementById("labBlockSwapEnter").innerHTML = "Einfahrtseite drehen";
  document.getElementById("labBlockClose").innerHTML = "Schliessen";
  document.getElementById("labBlockOpen").innerHTML = "Öffnen";
  document.getElementById("labBlockTrain").innerHTML = "Zug zuordnen";
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
  document.getElementById("labSliderDelta").innerHTML = "Regler-Delta";
  document.getElementById("labControlCode").innerHTML = "Zugriffscode";
  document.getElementById("labSlaveCode").innerHTML = "Eingeschränkter Zugangscode";
  document.getElementById("locoSelectTitle").innerHTML = "Lokomotiven";
  document.getElementById("helpTitle").innerHTML = "<b>Hilfe</b>";
  var help  = "<tr><th>Taste<th>Lange Klick-Funktion";
  help += "<tr><td>Lokbild<td>Öffnet das Block-Menü, wenn die ausgewählte Lok im Block ist.";
  help += "<tr><td>RE<td>Gleisspannung AUS";
  help += "<tr><td>FG<td>Nothalt";
  help += "<tr><td>F1-F4<td>Fahrregler-Auswahl";
  help += "<tr><td>F5<td>Einen Wagen zum Zug hinzufügen";
  help += "<tr><td>F6<td>Einen Wagen an der augenblicklichen Position abstellen";
  help += "<tr><td>F7<td>Einen Wagen des Zuges beladen";
  help += "<tr><td>F8<td>Einen Wagen des Zuges entladen";
  help += "<tr><td>F9<td>Mehrfachtraktions-Lok hinzufügen";
  help += "<tr><td>F10<td>Mehrfachtraktions-Lok entfernen";
  help += "<tr><td>F11<td>Zeige komplette Mehrfachtraktion inkl. Haupt-Lok";
  help += "<tr><td>F13<td>Manueller Modus EIN/AUS";
  help += "<tr><td>F14<td>Rangieren EIN/AUS";
  help += "<tr><td>Licht<td>Bereitstellen für Regler"; 
  help += "<tr><td>Richtung<td>Stopp (V=0)"; 
  document.getElementById("helpTable").innerHTML = help;
  document.getElementById("helpLink").href = "http://www.rocweb.net/doku.php?id=rocweb:rocweb-de";
  document.getElementById("clockTitle").innerHTML = "<b>Modelluhr</b>";
  document.getElementById("labClockFreeze").innerHTML = "Anhalten";
  document.getElementById("labClockResume").innerHTML = "Fortsetzen";
  
  $('#colorSelect').selectmenu("refresh");
}

function langEN() {
  var donkey = "<br>Rocrail runs entirely on volunteer labor. However, Rocrail also needs contributions of money. Your continued support is vital for keeping Rocrail available. If you already did support you can ask a key to disable this on startup dialog: donate@rocrail.net<br>";
  donkey += "<div id=\"donkeyWarning\"><b>Rocweb will stop communicate in 5 minutes!</b></div>";
  document.getElementById("donkey").innerHTML = donkey;

  document.getElementById("menuInfo").innerHTML = "Information";
  document.getElementById("menuSystem").innerHTML = "System";
  document.getElementById("menuAuto").innerHTML = "Automatic";
  document.getElementById("menuOptions").innerHTML = "Options";
  document.getElementById("menuGuest").innerHTML = "Guest loco";
  //document.getElementById("menuTrains").innerHTML = "Trains";
  //document.getElementById("menuDisconnect").innerHTML = "Disconnect server";
  document.getElementById("menuHelp").innerHTML = "Help";
  document.getElementById("systemTitle").innerHTML = "<b>System</b>";
  document.getElementById("autoTitle").innerHTML = "<b>Automatic</b>";
  document.getElementById("systemInitField").innerHTML = "Init field";
  document.getElementById("systemQuerySensors").innerHTML = "Start of day";
  document.getElementById("systemEmergencyBreak").innerHTML = "Emergency break";
  document.getElementById("systemStartAll").innerHTML = "Start all locs";
  document.getElementById("systemStopAll").innerHTML = "Stop all locs";
  document.getElementById("systemSoftReset").innerHTML = "Soft reset";
  document.getElementById("systemShutdown").innerHTML = "Shutdown server";
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
  document.getElementById("labOptionBinState").innerHTML = "BinState control";
  document.getElementById("labOptionShowBlockID").innerHTML = "Show block IDs";
  document.getElementById("labOptionShowTrainID").innerHTML = "Show train ID";
  document.getElementById("labOptionShowLocoImage").innerHTML = "Show loco image in block";
  document.getElementById("labOptionAnalogClock").innerHTML = "Fast clock";
  document.getElementById("labLocoCatEngine").innerHTML = "Engine";
  document.getElementById("labLocoCatEra").innerHTML = "Era";
  document.getElementById("labLocoCatRoadname").innerHTML = "Roadname";
  document.getElementById("labLocoSortID").innerHTML = "ID";
  document.getElementById("labLocoSortAddress").innerHTML = "Address";
  document.getElementById("labBlockStart").innerHTML = "Start train";
  document.getElementById("labBlockStop").innerHTML = "Stop train";
  document.getElementById("labBlockManual").innerHTML = "Half automatic";
  document.getElementById("labBlockReset").innerHTML = "Soft reset, keep block occupancy";
  document.getElementById("labBlockSwapPlacing").innerHTML = "Swap placing";
  document.getElementById("labBlockSwapEnter").innerHTML = "Swap enter side";
  document.getElementById("labBlockClose").innerHTML = "Close";
  document.getElementById("labBlockOpen").innerHTML = "Open";
  document.getElementById("labBlockTrain").innerHTML = "Assign train";
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
  document.getElementById("labSliderDelta").innerHTML = "Slider delta";
  document.getElementById("labControlCode").innerHTML = "Control code";
  document.getElementById("labSlaveCode").innerHTML = "Slave code";
  document.getElementById("locoSelectTitle").innerHTML = "Locomotives";
  document.getElementById("helpTitle").innerHTML = "<b>Help</b>";
  var help  = "<tr><th>Button<th>Long click function";
  help += "<tr><td>Loco image<td>Opens the block menu if the selected loco is in a block";
  help += "<tr><td>RE<td>Power OFF";
  help += "<tr><td>FG<td>Emergency Break";
  help += "<tr><td>F1-F4<td>Throttle selection";
  help += "<tr><td>F5<td>Add a car to the train";
  help += "<tr><td>F6<td>Leave a car from the train";
  help += "<tr><td>F7<td>Load a car of the train";
  help += "<tr><td>F8<td>Empty a car of the train";
  help += "<tr><td>F9<td>Add a consist loco";
  help += "<tr><td>F10<td>Remove a consist loco";
  help += "<tr><td>F11<td>Show the complete consist including the master loco";
  help += "<tr><td>F13<td>Manual mode ON/OFF";
  help += "<tr><td>F14<td>Shunting ON/OFF";
  help += "<tr><td>Lights<td>Dispatch"; 
  help += "<tr><td>Direction<td>Stop (V=0)"; 
  document.getElementById("helpTable").innerHTML = help;
  document.getElementById("helpLink").href = "http://www.rocweb.net/doku.php?id=rocweb:rocweb-en";
  document.getElementById("clockTitle").innerHTML = "<b>Fastclock</b>";
  document.getElementById("labClockFreeze").innerHTML = "Freeze";
  document.getElementById("labClockResume").innerHTML = "Resume";

  $('#colorSelect').selectmenu("refresh");
}

function langNL() {
  var donkey = "<br>Rocrail wordt uitsluitend door vrijwilligers ontwikkeld. Om Rocrail “in de lucht te houden” en steeds weer aan te kunnen passen aan nieuwe modelspoor hardware is er jaarlijks een bescheiden financieel budget nodig. Vrijwillige bijdragen zijn daarvoor noodzakelijk. Als je al een financiële ondersteuning gegeven hebt kun je op donate@rocrail.net een sleutel aanvragen om dit venster te onderdrukken.<br>";
  donkey += "<div id=\"donkeyWarning\"><b>Rocweb stopt met communicatie na 5 minuten!</b></div>";
  document.getElementById("donkey").innerHTML = donkey;
  
  document.getElementById("menuInfo").innerHTML = "Informatie";
  document.getElementById("menuSystem").innerHTML = "Systeem";
  document.getElementById("menuAuto").innerHTML = "Automaat";
  document.getElementById("menuOptions").innerHTML = "Opties";
  document.getElementById("menuGuest").innerHTML = "Gast locomotief";
  //document.getElementById("menuTrains").innerHTML = "Treinstellen";
  //document.getElementById("menuDisconnect").innerHTML = "Server loskoppelen";
  document.getElementById("menuHelp").innerHTML = "Hulp";
  document.getElementById("systemTitle").innerHTML = "<b>Systeem</b>";
  document.getElementById("autoTitle").innerHTML = "<b>Automat</b>";
  document.getElementById("systemInitField").innerHTML = "Init veld";
  document.getElementById("systemQuerySensors").innerHTML = "Begin van de dag";
  document.getElementById("systemEmergencyBreak").innerHTML = "Noodstop";
  document.getElementById("systemStartAll").innerHTML = "Start alle loc's";
  document.getElementById("systemStopAll").innerHTML = "Stop alle loc's";
  document.getElementById("systemSoftReset").innerHTML = "Reset behalve block bezetting";
  document.getElementById("systemShutdown").innerHTML = "Server beëindigen";
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
  document.getElementById("labOptionBinState").innerHTML = "BinState bediening";
  document.getElementById("labOptionShowBlockID").innerHTML = "Toon blok ID's";
  document.getElementById("labOptionShowTrainID").innerHTML = "Toon treinstel ID";
  document.getElementById("labOptionShowLocoImage").innerHTML = "Toon lok afbeelding in het blok";
  document.getElementById("labOptionAnalogClock").innerHTML = "Model klok";
  document.getElementById("labLocoCatEngine").innerHTML = "Aandrijving";
  document.getElementById("labLocoCatEra").innerHTML = "Periode";
  document.getElementById("labLocoCatRoadname").innerHTML = "Maatschappij";
  document.getElementById("labLocoSortID").innerHTML = "ID";
  document.getElementById("labLocoSortAddress").innerHTML = "Adres";
  document.getElementById("labBlockStart").innerHTML = "Start trein";
  document.getElementById("labBlockStop").innerHTML = "Stop trein";
  document.getElementById("labBlockManual").innerHTML = "Half automaat";
  document.getElementById("labBlockReset").innerHTML = "Reset";
  document.getElementById("labBlockSwapPlacing").innerHTML = "Omkeren";
  document.getElementById("labBlockSwapEnter").innerHTML = "Aankomst omkeren";
  document.getElementById("labBlockClose").innerHTML = "Sluiten";
  document.getElementById("labBlockOpen").innerHTML = "Openen";
  document.getElementById("labBlockTrain").innerHTML = "Treinstel toewijzen";
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
  document.getElementById("labSliderDelta").innerHTML = "Regelaar delta";
  document.getElementById("labControlCode").innerHTML = "Controle code";
  document.getElementById("labSlaveCode").innerHTML = "Beperkte code";
  document.getElementById("locoSelectTitle").innerHTML = "Locomotieven";
  document.getElementById("helpTitle").innerHTML = "<b>Help</b>";
  var help  = "<tr><th>Knop<th>Lange click functie";
  help += "<tr><td>Lok afbeelding<td>Opent het blok menu indien de lok in een blok staat";
  help += "<tr><td>RE<td>Rail spanning UIT";
  help += "<tr><td>FG<td>Noodstop";
  help += "<tr><td>F1-F4<td>Loc besturing keuze";
  help += "<tr><td>F5<td>Wagon aan de trein toevoegen";
  help += "<tr><td>F6<td>Wagon van de trein achterlaten";
  help += "<tr><td>F7<td>Wagon van de trein laden";
  help += "<tr><td>F8<td>Wagon van de trein legen";
  help += "<tr><td>F9<td>Multitractie loc toevoegen";
  help += "<tr><td>F10<td>Multitractie loc verwijderen";
  help += "<tr><td>F11<td>Toon de complete multitractie incl. hoofd loc";
  help += "<tr><td>F13<td>Handbedien modus AAN/UIT";
  help += "<tr><td>F14<td>Rangeren AAN/UIT";
  help += "<tr><td>Licht<td>Klaarzetten voor handregelaar"; 
  help += "<tr><td>Richting<td>Stoppen (V=0)"; 
  document.getElementById("helpTable").innerHTML = help;
  document.getElementById("helpLink").href = "http://www.rocweb.net/doku.php?id=rocweb:rocweb-en";
  document.getElementById("clockTitle").innerHTML = "<b>Model klok</b>";
  document.getElementById("labClockFreeze").innerHTML = "Stoppen";
  document.getElementById("labClockResume").innerHTML = "Starten";

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

function openClock(id)
{
  $( "#popupClock" ).popup( "open", {positionTo: '#'+id} );
}

function onLocoImage() {
  if( tapholdFkey == 1 ) {
    tapholdFkey = 0;
    return;
  }
  initLocoList("select");

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
  initLocoList("select");
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
  else if( locoSelectAction == "select" ) {
    locoSelected = sel;
    localStorage.setItem("locoSelected", locoSelected);
    locoSelectedList[locoSelectedIdx] = locoSelected;
    localStorage.setItem("locoSelected"+locoSelectedIdx, locoSelected);
  }
  else if( locoSelectAction == "trainaddcar" ) {
    var lc = lcMap[locoSelected];
    var cmd = "<operator controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+lc.getAttribute('train')+"\" cmd=\"addcar\" carids=\""+sel+"\" />";
    sendCommand(cmd);
  }
  else if( locoSelectAction == "trainleavecar" ) {
    var lc = lcMap[locoSelected];
    var cmd = "<operator controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+lc.getAttribute('train')+"\" cmd=\"removecar\" carids=\""+sel+"\" />";
    sendCommand(cmd);
  }
  else if( locoSelectAction == "trainemptycar" ) {
    var lc = lcMap[locoSelected];
    var cmd = "<operator controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+lc.getAttribute('train')+"\" cmd=\"emptycar\" carids=\""+sel+"\" />";
    sendCommand(cmd);
  }
  else if( locoSelectAction == "trainloadcar" ) {
    var lc = lcMap[locoSelected];
    var cmd = "<operator controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+lc.getAttribute('train')+"\" cmd=\"loadcar\" carids=\""+sel+"\" />";
    sendCommand(cmd);
  }
  else if( locoSelectAction == "consistadd" ) {
    var master = lcMap[locoSelected];
    var slaves = master.getAttribute('consist');
    var newslaves = "";
    if( newslaves.length > 0 )
      newslaves += ",";
    newslaves += sel;
    master.setAttribute('consist', newslaves);
    var cmd = "<model controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"modify\"><lc id=\""+master.getAttribute('id')+"\" consist=\""+newslaves+"\"/></model>";
    sendCommand(cmd);
  }
  else if( locoSelectAction == "consistdel" ) {
    var master = lcMap[locoSelected];
    var slaves = master.getAttribute('consist').split(",");
    var newslaves = "";
    for( var i = 0; i < slaves.length; i++ ) {
      if( sel == slaves[i] )
        continue;
      if( newslaves.length > 0 )
        newslaves += ",";
      newslaves += sel;
    }
    master.setAttribute('consist', newslaves);
    var cmd = "<model controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"modify\"><lc id=\""+master.getAttribute('id')+"\" consist=\""+newslaves+"\"/></model>";
    sendCommand(cmd);
  }
  else if( locoSelectAction == "consistshow" ) {
    // Nothing todo.
  }
  
  initThrottle();
  updateDir();
  updateFunctionLabels();
  
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
  
  var idx = 0;
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
    if( document.getElementById("title").innerHTML.indexOf(title) != -1 ) {
      levelSelect.selectedIndex = ""+idx;
      zlevelIdx = idx;
      trace(document.getElementById("title").innerHTML + " == " + title + " i=" +i + " idx="+idx);
    }
    else {
      trace(document.getElementById("title").innerHTML + " != " + title);
    }
    idx++;
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

function openAuto() {
  trace("close menu");
  //$( "#popupMenu" ).popup( "close" );
  $('#popupMenu').on("panelclose", function(){
    $('#popupMenu').unbind( "panelclose" );
    $( "#popupAuto" ).popup( "open" );
    });
}

function openHelp() {
  trace("close menu");
  //$( "#popupMenu" ).popup( "close" );
  $('#popupMenu').on("panelclose", function(){
    $('#popupMenu').unbind( "panelclose" );
    $( "#popupHelp" ).popup( "open" );
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
  var binstate = localStorage.getItem("binstate");
  $('#optionBinState').prop('checked', binstate=="true"?true:false).checkboxradio('refresh');
  var showblockid = localStorage.getItem("showblockid");
  $('#optionShowBlockID').prop('checked', showblockid=="true"?true:false).checkboxradio('refresh');
  var showtrainid = localStorage.getItem("showtrainid");
  $('#optionShowTrainID').prop('checked', showtrainid=="true"?true:false).checkboxradio('refresh');
  var showlocoimage = localStorage.getItem("showlocoimage");
  $('#optionShowLocoImage').prop('checked', showlocoimage=="true"?true:false).checkboxradio('refresh');
  var analogclock = localStorage.getItem("analogclock");
  if( analogclock == undefined )
    analogclock = "true";
  $('#optionAnalogClock').prop('checked', analogclock=="true"?true:false).checkboxradio('refresh');

  var category = localStorage.getItem("category");
  
  if( category == "era" )
    $("#locoCatEra").prop("checked", true).checkboxradio('refresh');
  else if( category == "roadname" )
    $("#locoCatRoadname").prop("checked", true).checkboxradio('refresh');
  else
    $("#locoCatEngine").prop("checked", true).checkboxradio('refresh');
  
  var locosort = localStorage.getItem("locosort");
  
  if( locosort == "address" )
    $("#locoSortAddress").prop("checked", true).checkboxradio('refresh');
  else
    $("#locoSortID").prop("checked", true).checkboxradio('refresh');
  
  var select = document.getElementById("languageSelect");

  var red   = localStorage.getItem("red");
  var green = localStorage.getItem("green");
  var blue  = localStorage.getItem("blue");
  document.getElementById("userColorRed").value = red;
  document.getElementById("userColorGreen").value = green;
  document.getElementById("userColorBlue").value = blue;

  var sliderdelta = parseInt(localStorage.getItem("sliderdelta"));
  if( sliderdelta == undefined )
    sliderdelta = 3;
  document.getElementById("sliderDelta").value = sliderdelta;

  var controlcode = localStorage.getItem("controlcode");
  if( controlcode == undefined )
    controlcode = "";
  document.getElementById("controlCode").value = controlcode;

  var slavecode = localStorage.getItem("slavecode");
  if( slavecode == undefined )
    slavecode = "";
  document.getElementById("slaveCode").value = slavecode;

  $('#popupMenu').on("panelclose", function(){
    $('#popupMenu').unbind( "panelclose" );
    var maxHeight = $(window).height() - 75;
    $('#divOptions').css('max-height', maxHeight + 'px');
    $('#divOptions').css('overflow-y', 'scroll'); 
    $( "#popupOptions" ).popup( "open" );
    });

}

function onLocoSort(sort) {
  trace("locosort="+sort);
  localStorage.setItem("locosort", sort);
  location.reload();
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
  $("#F5").bind("taphold", tapholdF5Handler);
  $("#F6").bind("taphold", tapholdF6Handler);
  $("#F7").bind("taphold", tapholdF7Handler);
  $("#F8").bind("taphold", tapholdF8Handler);
  $("#F13").bind("taphold", tapholdF13Handler);
  $("#F14").bind("taphold", tapholdF14Handler);
  $("#direction").bind("taphold", tapholdDirectionHandler);
  $("#locoImage").bind("taphold", tapholdLocoImageHandler);
  $("#locoImageBlock").bind("taphold", tapholdLocoImageBlockHandler);
  $("#F9").bind("taphold", tapholdF9Handler);
  $("#F10").bind("taphold", tapholdF10Handler);
  $("#F11").bind("taphold", tapholdF11Handler);
 
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
  function tapholdF5Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F5: Add car");
    onTrainCar("trainaddcar");
  }
  function tapholdF6Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F6: Leave car");
    onTrainCar("trainleavecar");
  }
  function tapholdF7Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F7: Load car");
    onTrainCar("trainloadcar");
  }
  function tapholdF8Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F8: Empty car");
    onTrainCar("trainemptycar");
  }
  function tapholdREHandler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold RE: power off");
    var cmd = "<sys controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" informall=\"true\" cmd=\"stop\"/>";
    sendCommand(cmd);
  }
  function tapholdFGHandler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold FG: emergancy break");
    var cmd = "<sys controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"ebreak\" informall=\"true\"/>";
    sendCommand(cmd);
  }
  function tapholdF0Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F0: dispatch");
    var cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoSelected+"\" cmd=\"dispatch\"/>";
    sendCommand(cmd);
  }
  function tapholdF13Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F13: manual mode");
    var lc = lcMap[locoSelected];
    var manualon = lc.getAttribute('manualon');
    var cmd = "";
    if( manualon == undefined || manualon == "false" ) {
      cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoSelected+"\" cmd=\"manualon\"/>";
      lc.setAttribute('manualon', "true");
    }
    else {
      cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoSelected+"\" cmd=\"manualoff\"/>";
      lc.setAttribute('manualon', "false");
    }
    sendCommand(cmd);
  }
  function tapholdF14Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F14: shunting");
    var shuntingon = lc.getAttribute('shuntingon');
    var cmd = "";
    if( shuntingon == undefined || shuntingon == "false" ) {
      cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoSelected+"\" cmd=\"shuntingon\"/>";
      lc.setAttribute('shuntingon', "true");
    }
    else {
      cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoSelected+"\" cmd=\"shuntingoff\"/>";
      lc.setAttribute('shuntingon', "false");
    }
    sendCommand(cmd);
  }

  function tapholdF9Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F9: consist add");
    onConsistAdd();
  }
  
  function tapholdF10Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F10: consist delete");
    onConsistDel();
  }
  
  function tapholdF11Handler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold F11: consist show");
    onConsistShow();
  }
  
  function tapholdDirectionHandler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold direction: ...");
    var lc = lcMap[locoSelected];
    if( lc == undefined )
      lc = carMap[locoSelected];
    if( lc == undefined )
      return;
    var cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoSelected+"\" V=\"0\" fn=\""+lc.getAttribute("fn")+"\" dir=\""+lc.getAttribute("dir")+"\"/>";
    sendCommand(cmd);
  }
  
  function tapholdLocoImageHandler(e) {
    e.preventDefault();
    tapholdFkey = 1;
    trace("taphold locoImage: ...");
    var bk = findBlock4Loco(locoSelected);
    if( bk != undefined ) {
      prevPopup = "popupThrottle;"
      actionBlock("bk_"+bk.getAttribute('id'), true);
    }
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
  speedUpdateVal = 0;
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
  var cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" throttleid=\""+throttleid+"\" cmd=\"release\" id=\""+locoSelected+"\"/>";
  sendCommand(cmd);
}

function onST() {
  trace("start/stop loco " + locoSelected);
  var lc = lcMap[locoSelected];
  var mode = lc.getAttribute('mode');
  var cmd = "";
  if( mode != undefined && (mode == "auto" || mode == "halfauto") )
    cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoSelected+"\" throttleid=\""+throttleid+"\" cmd=\"stop\"/>";
  else
    cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoSelected+"\" throttleid=\""+throttleid+"\" cmd=\"go\"/>";
  sendCommand(cmd);
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
  var cmd = "<fn controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoSelected+"\" throttleid=\""+throttleid+"\" fnchanged=\""+nr+"\" group=\""+group+"\" f"+nr+"=\""+on+"\"/>"
  sendCommand(cmd);
}

function onBinState(bindata) {
  lc = lcMap[locoSelected];
  if(lc == undefined)
    lc = carMap[locoSelected];
  if( lc == undefined ) 
    return;
  
  var nr  = document.getElementById("binStateAddress").value;
  var sec = document.getElementById("secondaryAddress").value;
  var decaddr = lc.getAttribute("secaddr");
  if( sec == "off" )
    decaddr = lc.getAttribute("addr");
  var cmd = "<binstate controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoSelected+"\" throttleid=\""+throttleid+"\"  nr=\""+nr+"\" data=\""+bindata+"\" addr=\""+decaddr+"\"/>"
  sendCommand(cmd);
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
  var cmd = "<fn controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoSelected+"\" throttleid=\""+throttleid+"\" fnchanged=\""+0+"\" f"+0+"=\""+on+"\"/>"
  sendCommand(cmd);
}

function speedUpdate(value) {
  lc = lcMap[locoSelected];
  if( lc == undefined )
    lc = carMap[locoSelected];
  if( lc == undefined ) return;

  var V_max = parseInt(lc.getAttribute('V_max'));
  var sec = document.getElementById("secondaryAddress").value;
  var usesecaddr = (sec=="on") ? true:false;
  trace("Speed: " + value + " for loco " + locoSelected);
  lc.setAttribute('V', value);
  trace("value="+value+" V_max="+V_max);
  var cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" usesecaddr=\""+usesecaddr+"\" throttleid=\""+throttleid+"\" id=\""+locoSelected+"\" V=\""+value+"\" dir=\""+lc.getAttribute('dir')+"\"/>";
  updateDir();
  sendCommand(cmd);
}


/* System commands */
function actionPower() {
  var mOn = power == 'true' ? false:true;
  var cmd = "<sys controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" informall=\"true\" cmd=\""+(mOn?"go":"stop")+"\"/>";
  sendCommand(cmd);
}

function actionStartAll() {
  $( "#popupAuto" ).popup( "close" );
  var cmd = "<auto controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"start\"/>";
  sendCommand(cmd);
}

function actionStopAll() {
  $( "#popupAuto" ).popup( "close" );
  var cmd = "<auto controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"stop\"/>";
  sendCommand(cmd);
}

function actionSoftReset() {
  $( "#popupAuto" ).popup( "close" );
  var cmd = "<auto controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"softreset\"/>";
  sendCommand(cmd);
}

function actionShutdown() {
  $( "#popupSystem" ).popup( "close" );
  var cmd = "<sys controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"shutdown\"/>";
  sendCommand(cmd);
}

function actionLevelSelect(z) {
  if( ModPlan )
    return;
  trace("actionLevelSelect: "+ z);
  zlevelSelected.style.display = 'none';
  zleveldiv = zlevelDivMap[z];
  zlevelSelected = zleveldiv;
  zleveldiv.style.display = 'block';
    
  var zlevel = zlevelMap[z];
  Title = zlevel.getAttribute('title');
  var h = document.getElementById("title");
  h.innerHTML = timelabel + Title;
  
  var idx = 0;
  for (var i in zlevelMap){
    var zlevel = zlevelMap[i];
    if( zlevel.getAttribute('z') == z ) {
      zlevelIdx = idx;
      break;
    }
    idx++;
  }

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
  Title = zlevel.getAttribute('title');
  var h = document.getElementById("title");
  h.innerHTML = timelabel + Title;
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
  Title = zlevel.getAttribute('title');
  var h = document.getElementById("title");
  h.innerHTML = timelabel + Title;
}


/* Item commands */
function actionAuto(auto) {
  trace("auto action " + auto);
  if( autoMode == "on" )
    auto = "off";
  var cmd = "<auto controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\""+auto+"\"/>";
  sendCommand(cmd);
}

function actionEBreak() {
  trace("emergancy break");
  $( "#popupSystem" ).popup( "close" );
  var cmd = "<sys controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"ebreak\" informall=\"true\"/>";
  sendCommand(cmd);
}

function actionSystemInitfield() {
  $( "#popupSystem" ).popup( "close" );
  var cmd = "<model controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"initfield\" informall=\"true\"/>";
  sendCommand(cmd);
}

function actionSystemQuerySensors() {
  $( "#popupSystem" ).popup( "close" );
  var cmd = "<sys controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"sod\" informall=\"true\"/>";
  sendCommand(cmd);
}

function actionRoute(id) {
  var div = document.getElementById(id);
  div.style.backgroundColor = greenBackground;
  stid = id.replace("st_","");
  trace("route action on " + stid );
  st = stMap[stid];
  var cmd = "<st controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"test\" id=\""+stid+"\"/>";
  sendCommand(cmd);
}

function actionSensor(id)
{
  var simsensors = localStorage.getItem("simsensors");
  if( simsensors == undefined )
    simsensors = "false";
  if( simsensors == "false" )
    return;

  var div = document.getElementById(id);
  div.style.backgroundColor = "red";
  
  fbid = id.replace("fb_","");
  trace("sensor action on " + fbid );
  fb = fbMap[fbid];
  var cmd = "";
  if( "true" == fb.getAttribute('state') )
    cmd = "<fb controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" state=\"false\" id=\""+fbid+"\"/>";
  else
    cmd = "<fb controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" state=\"true\" id=\""+fbid+"\"/>";
  sendCommand(cmd);
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
  var cmd = "<model controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"modify\"><tx id=\""+txid+"\" text=\""+text+"\"/></model>";
  sendCommand(cmd);

}

function actionSwitch(id) {
  var div = document.getElementById(id);
  swid = id.replace("sw_","");
  sw = swMap[swid];
  if( parseInt(sw.getAttribute("addr1")) != 0 || parseInt(sw.getAttribute("port1")) != 0 ) 
    div.style.backgroundColor = "red";
  trace("switch action on " + swid + " state=" + sw.getAttribute('state'));
  var cmd = "<sw controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"flip\" id=\""+swid+"\" manualcmd=\"true\"/>";
  sendCommand(cmd);
}

function actionOutput(id) {
  var div = document.getElementById(id);
  coid = id.replace("co_","");
  co = coMap[coid];
  if( parseInt(co.getAttribute("addr")) != 0 || parseInt(co.getAttribute("port")) != 0 ) 
    div.style.backgroundColor = "red";
  trace("output action on " + coid + " state=" + co.getAttribute('state'));
  var toggleswitch = co.getAttribute('toggleswitch');
  if( toggleswitch == undefined || toggleswitch == "true" ) {
    var cmd = "<co controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"flip\" id=\""+coid+"\"/>";
    sendCommand(cmd);
  }
}

function actionOutputDown(id) {
  coid = id.replace("co_","");
  co = coMap[coid];
  trace("output down action on " + coid + " state=" + co.getAttribute('state'));
  var toggleswitch = co.getAttribute('toggleswitch');
  if( toggleswitch == undefined || toggleswitch == "true" )
    return;
  var cmd = "<co controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"on\" id=\""+coid+"\"/>";
  sendCommand(cmd);
}

function actionOutputUp(id) {
  coid = id.replace("co_","");
  co = coMap[coid];
  trace("output up action on " + coid + " state=" + co.getAttribute('state'));
  var toggleswitch = co.getAttribute('toggleswitch');
  if( toggleswitch == undefined || toggleswitch == "true" )
    return;
  var cmd = "<co controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"off\" id=\""+coid+"\"/>";
  sendCommand(cmd);
}

function actionSignal(id) {
  var div = document.getElementById(id);
  sgid = id.replace("sg_","");
  sg = sgMap[sgid];
  if( parseInt(sg.getAttribute("addr")) != 0 || parseInt(sg.getAttribute("port1")) != 0 ) 
    div.style.backgroundColor = "red";
  trace("signal action on " + sgid + " state=" + sg.getAttribute('state'));
  var cmd = "<sg controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" cmd=\"flip\" id=\""+sgid+"\"/>";
  sendCommand(cmd);
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

  var trackSelect = document.getElementById("trackFYSelect");
  while(trackSelect.options.length > 0) {
    trackSelect.remove(0);
  }

  track = document.createElement( 'option' );
  track.value = "";
  track.innerHTML = "";
  track.selected = 'selected';
  trackSelect.add( track );

  tracklist = fyNode.getElementsByTagName("seltabpos");
  for (var i = 0; i < tracklist.length; i++) {
    var nr   = tracklist[i].getAttribute('nr');
    var desc = tracklist[i].getAttribute('desc');
    track = document.createElement( 'option' );
    track.value = nr;
    track.innerHTML = nr+" "+desc;
    track.setAttribute('onclick', "onFYTrackSelected('"+nr+"')");
    trackSelect.add( track );
  }
  $('#trackFYSelect').selectmenu("refresh");

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

function actionBlock(id, throttle) {
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
  if( throttle ) {
    trace("close throttle");
    prevPopup = "popupThrottle;"
    $( "#popupThrottle" ).popup( "close" );
    trace("open loco select");
    $('#popupThrottle').on("popupafterclose", function(){
      $('#popupThrottle').unbind( "popupafterclose" );
      var maxHeight = $(window).height() - 75;
      $('#divBlock').css('max-height', maxHeight + 'px');
      $('#divBlock').css('overflow-y', 'scroll'); 
      $( "#popupBlock" ).popup( "open" );
      });
    $('#popupBlock').on("popupafterclose", function(){
      $('#popupBlock').unbind( "popupafterclose" );
      $( "#popupThrottle" ).popup( "open" );
      });
  }
  else {
    var maxHeight = $(window).height() - 75;
    $('#divBlock').css('max-height', maxHeight + 'px');
    $('#divBlock').css('overflow-y', 'scroll'); 
    $( "#popupBlock" ).popup( "open", {positionTo: '#'+id} );
  }

}

function onBlockTrain() {
  $( "#popupBlock" ).popup( "close" );
  locoBlockSelect = sessionStorage.getItem("locoBlockSelect");
  if( locoBlockSelect != "none" ) {
    if( trainBlockSelect == "none" ) {
      // release train
      var cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoBlockSelect+"\" cmd=\"releasetrain\"/>";
      sendCommand(cmd);
    }
    else {
      // assign train
      var cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoBlockSelect+"\" cmd=\"assigntrain\" train=\""+trainBlockSelect+"\"/>";
      sendCommand(cmd);
    }
  }  
}

function onBlockStart(gomanual) {
  $( "#popupBlock" ).popup( "close" );
  locoBlockSelect = sessionStorage.getItem("locoBlockSelect");
  if( locoBlockSelect != "none" ) {
    if( scheduleBlockSelect != "none" && scheduleBlockSelect.length > 0 ) {
      var cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoBlockSelect+"\" cmd=\"useschedule\" scheduleid=\""+scheduleBlockSelect+"\"/>";
      sendCommand(cmd);
    }  
    else if( blockBlockSelect != "none" && blockBlockSelect.length > 0 ) {
      var cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoBlockSelect+"\" cmd=\"gotoblock\" blockid=\""+blockBlockSelect+"\"/>";
      sendCommand(cmd);
    }  
    var cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoBlockSelect+"\" cmd=\""+(gomanual?"gomanual":"go")+"\"/>";
    sendCommand(cmd);
  }
}

function onBlockReset(soft) {
  $( "#popupBlock" ).popup( "close" );
  locoBlockSelect = sessionStorage.getItem("locoBlockSelect");
  if( locoBlockSelect != "none" ) {
    var cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoBlockSelect+"\" cmd=\""+(soft?"softreset":"reset")+"\"/>";
    sendCommand(cmd);
  }
}

function onBlockStop() {
  $( "#popupBlock" ).popup( "close" );
  locoBlockSelect = sessionStorage.getItem("locoBlockSelect");
  if( locoBlockSelect != "none" ) {
    var cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoBlockSelect+"\" cmd=\"stop\"/>";
    sendCommand(cmd);
  }
}

function onBlockSwapPlacing() {
  $( "#popupBlock" ).popup( "close" );
  locoBlockSelect = sessionStorage.getItem("locoBlockSelect");
  if( locoBlockSelect != "none" ) {
    var cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoBlockSelect+"\" cmd=\"swap\"/>";
    sendCommand(cmd);
  }
}

function onBlockSwapEnter() {
  $( "#popupBlock" ).popup( "close" );
  locoBlockSelect = sessionStorage.getItem("locoBlockSelect");
  if( locoBlockSelect != "none" ) {
    var cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+locoBlockSelect+"\" cmd=\"blockside\"/>";
    sendCommand(cmd);
  }
}

function onBlockOpen() {
  $( "#popupBlock" ).popup( "close" );
  bkid = sessionStorage.getItem("block");
  var cmd = "<bk controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+bkid+"\" state=\"open\"/>";
  sendCommand(cmd);
}

function onBlockClose() {
  $( "#popupBlock" ).popup( "close" );
  bkid = sessionStorage.getItem("block");
  var cmd = "<bk controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+bkid+"\" state=\"closed\"/>";
  sendCommand(cmd);
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
  var cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+address+"\" shortid=\""+id+"\" spcnt=\""+guestSteps+"\" prot=\""+guestProt+"\" V=\"0\"/>";
  sendCommand(cmd);
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
    cmd = "<lc controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+lcid+"\" cmd=\"block\" blockid=\""+bkid+"\"/>";
  else
    cmd = "<bk controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+bkid+"\" cmd=\"loc\" locid=\"\"/>";
  sendCommand(cmd);
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
  var cmd = "<sb controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+sbid+"\" cmd=\"compress\"/>";
  sendCommand(cmd);
}


function onStageOpen() {
  $( "#popupStageBlock" ).popup( "close" );
  sbid = sessionStorage.getItem("stageblock");
  var cmd = "<sb controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+sbid+"\" state=\"open\"/>";
  sendCommand(cmd);
}


function onStageClose() {
  $( "#popupStageBlock" ).popup( "close" );
  sbid = sessionStorage.getItem("stageblock");
  var cmd = "<sb controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+sbid+"\" state=\"closed\"/>";
  sendCommand(cmd);
}


function onStageOpenExit() {
  $( "#popupStageBlock" ).popup( "close" );
  sbid = sessionStorage.getItem("stageblock");
  var cmd = "<sb controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+sbid+"\" exitstate=\"open\"/>";
  sendCommand(cmd);
}


function onStageCloseExit() {
  $( "#popupStageBlock" ).popup( "close" );
  sbid = sessionStorage.getItem("stageblock");
  var cmd = "<sb controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+sbid+"\" exitstate=\"closed\"/>";
  sendCommand(cmd);
}

function onTurntableGotoTrack() {
  $( "#popupTurntable" ).popup( "close" );
  ttid = sessionStorage.getItem("turntable");
  var cmd = "<tt controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+ttid+"\" cmd=\""+trackTTSelect+"\"/>";
  sendCommand(cmd);
}

function onTurntableNext() {
  $( "#popupTurntable" ).popup( "close" );
  ttid = sessionStorage.getItem("turntable");
  var cmd = "<tt controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+ttid+"\" cmd=\"next\"/>";
  sendCommand(cmd);
}


function onTurntablePrevious() {
  $( "#popupTurntable" ).popup( "close" );
  ttid = sessionStorage.getItem("turntable");
  var cmd = "<tt controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+ttid+"\" cmd=\"previous\"/>";
  sendCommand(cmd);
}


function onFiddleYardGotoTrack() {
  $( "#popupFiddleYard" ).popup( "close" );
  fyid = sessionStorage.getItem("fiddleyard");
  var cmd = "<seltab controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+fyid+"\" cmd=\""+trackFYSelect+"\"/>";
  sendCommand(cmd);
}

function onFiddleYardNext() {
  $( "#popupFiddleYard" ).popup( "close" );
  fyid = sessionStorage.getItem("fiddleyard");
  var cmd = "<seltab controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+fyid+"\" cmd=\"next\"/>";
  sendCommand(cmd);
}


function onFiddleYardPrevious() {
  $( "#popupFiddleYard" ).popup( "close" );
  fyid = sessionStorage.getItem("fiddleyard");
  var cmd = "<seltab controlcode=\""+controlCode+"\" slavecode=\""+slaveCode+"\" id=\""+fyid+"\" cmd=\"previous\"/>";
  sendCommand(cmd);
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
  var optionBinState = document.getElementById("optionBinState");
  localStorage.setItem("speedbuttons", optionSpeedButtons.checked ? "true":"false");
  localStorage.setItem("binstate", optionBinState.checked ? "true":"false");
  trace("option speedbuttons = "+ optionSpeedButtons.checked );
  document.getElementById("binStateDiv").style.display = optionBinState.checked ? 'block':'none';
  document.getElementById("speedSliderDiv").style.display = optionSpeedButtons.checked ? 'none':'block';
  document.getElementById("speedButtonsDiv").style.display = optionSpeedButtons.checked ? 'block':'none';
}

function onOptionShowBlockID() {
  var optionShowBlockID = document.getElementById("optionShowBlockID");
  localStorage.setItem("showblockid", optionShowBlockID.checked ? "true":"false");
  trace("option showblockid = "+ optionShowBlockID.checked );
}

function onOptionShowTrainID() {
  var optionShowTrainID = document.getElementById("optionShowTrainID");
  localStorage.setItem("showtrainid", optionShowTrainID.checked ? "true":"false");
  trace("option showtrainid = "+ optionShowTrainID.checked );
}

function onOptionShowLocoImage() {
  var optionShowLocoImage = document.getElementById("optionShowLocoImage");
  localStorage.setItem("showlocoimage", optionShowLocoImage.checked ? "true":"false");
  trace("option showlocoiamge = "+ optionShowLocoImage.checked );
}

function onOptionAnalogClock() {
  var optionAnalogClock = document.getElementById("optionAnalogClock");
  localStorage.setItem("analogclock", optionAnalogClock.checked ? "true":"false");
  trace("option analogclock = "+ optionAnalogClock.checked );
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

    var train = lc.getAttribute('train');
    if( train != undefined && train.length > 0 ) {
      train = "_" + train + "("+lc.getAttribute('trainlen')+","+lc.getAttribute('trainweight')+")";
    }
    else
      train = "";
    locoStatus.style.backgroundColor = modeColor;
    locoStatus.innerHTML = lc.getAttribute('id') + train + " [" + mode + "]" + fromTo;
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
  var V_max = parseInt(lc.getAttribute('V_max'));
  $("#speedSlider").prop("max", V_max).slider("refresh");
  slider.value = V;
  speedUpdateVal = V;
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


function onClock(resume) {
  $( "#popupClock" ).popup( "close" );
  var cmd = "<clock cmd=\""+(resume?"go":"freeze")+"\"/>";
  sendCommand(cmd);
}

function onTrainCar(action) {
  trace("train: "+action);
  
  var lc = lcMap[locoSelected];
  var train = lc.getAttribute('train');
  if( train == undefined || train.length == 0 ) {
    return;
  }

  initLocoList(action);

  trace("close throttle");
  prevPopup = "popupThrottle;"
  $( "#popupThrottle" ).popup( "close" );
  trace("open loco select");
  $('#popupThrottle').on("popupafterclose", function(){
    $('#popupThrottle').unbind( "popupafterclose" );
    $( "#popupLocoSelect" ).popup( "open" );
    });
}


function onConsistAdd() {
  trace("consist add");
  initLocoList("consistadd");

  trace("close throttle");
  prevPopup = "popupThrottle;"
  $( "#popupThrottle" ).popup( "close" );
  trace("open loco select");
  $('#popupThrottle').on("popupafterclose", function(){
    $('#popupThrottle').unbind( "popupafterclose" );
    $( "#popupLocoSelect" ).popup( "open" );
    });
}


function onConsistDel() {
  trace("consist delete");
  initLocoList("consistdel");

  trace("close throttle");
  prevPopup = "popupThrottle;"
  $( "#popupThrottle" ).popup( "close" );
  trace("open loco select");
  $('#popupThrottle').on("popupafterclose", function(){
    $('#popupThrottle').unbind( "popupafterclose" );
    $( "#popupLocoSelect" ).popup( "open" );
    });
}


function onConsistShow() {
  trace("consist show");
  initLocoList("consistshow");

  trace("close throttle");
  prevPopup = "popupThrottle;"
  $( "#popupThrottle" ).popup( "close" );
  trace("open loco select");
  $('#popupThrottle').on("popupafterclose", function(){
    $('#popupThrottle').unbind( "popupafterclose" );
    $( "#popupLocoSelect" ).popup( "open" );
    });
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


function onVUp() {
  var lc = lcMap[locoSelected];
  if( lc == undefined )
    lc = carMap[locoSelected];
  if( lc == undefined ) 
    return;
  var V_max = parseInt(lc.getAttribute('V_max'));
  speedUpdateVal += sliderDelta;
  if(speedUpdateVal > V_max)
    speedUpdateVal = V_max;
  speedUpdate(speedUpdateVal);
}

function onVDown() {
  speedUpdateVal -= sliderDelta;
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
    if( value < speedUpdateVal - sliderDelta || value > speedUpdateVal + sliderDelta) {
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
  
  $('#trackFYSelect').change(function() {
    trackFYSelect = this.value;
    trace("trackFYSelect: " + trackFYSelect);
    sessionStorage.setItem("trackFYSelect", trackFYSelect);
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
    location.reload();
  } );
  
  $('#sliderDelta').change(function() {
    var delta = document.getElementById("sliderDelta").value;
    trace("sliderDelta="+delta);
    localStorage.setItem("sliderdelta", delta);
    sliderDelta = parseInt(delta);
  });

  $('#controlCode').change(function() {
    var code = document.getElementById("controlCode").value;
    trace("controlCode="+code);
    localStorage.setItem("controlcode", code);
    controlCode = code;
  });

  $('#slaveCode').change(function() {
    var code = document.getElementById("slaveCode").value;
    trace("slaveCode="+code);
    localStorage.setItem("slavecode", code);
    slaveCode = code;
  });

});

$(document).ready(function(){
  trace("document ready");
  doLang();
})

function doLang() {
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
}


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
    if( s == "master" ) return "Hauptlok";
    if( s == "slaves" ) return "Mehrfachtraktion";
    if( s == "guests" ) return "Gäste";
    if( s == "trainaddcar" ) return "Wagen hinzufügen";
    if( s == "trainleavecar" ) return "Wagen entfernen";
    if( s == "trainloadcar" ) return "Wagen beladen";
    if( s == "trainemptycar" ) return "Wagen leeren";
    if( s == "fastclock" ) return "Modelluhr";
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
    if( s == "master" ) return "Master loco";
    if( s == "slaves" ) return "Slave locos";
    if( s == "guests" ) return "Guests";
    if( s == "trainaddcar" ) return "Add car";
    if( s == "trainleavecar" ) return "Leave car";
    if( s == "trainloadcar" ) return "Load car";
    if( s == "trainemptycar" ) return "Empty car";
    if( s == "fastclock" ) return "Fast clock";
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
    if( s == "master" ) return "Tractie hoofd loc";
    if( s == "slaves" ) return "Tractie locs";
    if( s == "guests" ) return "Gasten";
    if( s == "trainaddcar" ) return "Wagon toevoegen";
    if( s == "trainleavecar" ) return "Wagon achterlaten";
    if( s == "trainloadcar" ) return "Wagon beladen";
    if( s == "trainemptycar" ) return "Wagon legen";
    if( s == "fastclock" ) return "Model klok";
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
    div.style.backgroundColor = "";
    div.style.backgroundImage = getSensorImage(fbNode);
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

  text = text.replace("|","<br>");
  
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


function getTextImage(txNode, div) {
  var text = txNode.getAttribute('text');
  var ori  = getOri(txNode);

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
    var height = div.style.height;
    if( ori == "north" )
      div.innerHTML = "<div class='itemtextV' style='font-size:"+pointsize+"px;'>"+getTextFormat(txNode)+"</div>";
    else if( ori == "south" )
      div.innerHTML = "<div class='itemtextV' style='font-size:"+pointsize+"px;'>"+getTextFormat(txNode)+"</div>";
    else
      div.innerHTML = "<div style='font-size:"+pointsize+"px;'>" +getTextFormat(txNode)+ "</div>";
  }
  
}


function handleText(tx) {
  trace("text event: " + tx.getAttribute('id') + " " + tx.getAttribute('text'));
  var div = document.getElementById("tx_"+tx.getAttribute('id'));
  if( div != null ) {
    var txNode = txMap[tx.getAttribute('id')];
    txNode.setAttribute('text', tx.getAttribute('text'));
    getTextImage(txNode, div);
  }
}

function handleOutput(co) {
  trace("output event: " + co.getAttribute('id') + " " + co.getAttribute('state'));
  var div = document.getElementById("co_"+co.getAttribute('id'));
  if( div != null ) {
    coNode = coMap[co.getAttribute('id')];
    coNode.setAttribute('state', co.getAttribute('state'));
    div.style.backgroundColor = "";
    div.style.backgroundImage = getOutputImage(coNode);
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
    div.style.backgroundColor = "";
    div.style.backgroundImage = getSwitchImage(swNode, div, true);
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
    div.style.backgroundColor = "";
    div.style.backgroundImage = getSignalImage(sgNode, div);
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


  var newmode  = lc.getAttribute('mode');
  var prevmode = lcNode.getAttribute('mode');
  var modechanged = false;
  if( newmode != undefined && newmode != prevmode ) {
    lcNode.setAttribute('mode', newmode );
    modechanged = true;
  }
  
  if( lc.getAttribute('modereason') != undefined )
    lcNode.setAttribute('modereason', lc.getAttribute('modereason'));
  
  if( lc.getAttribute('V') != undefined )
    lcNode.setAttribute('V', lc.getAttribute('V'));
  if( lc.getAttribute('dir') != undefined )
    lcNode.setAttribute('dir', lc.getAttribute('dir'));
  if( lc.getAttribute('fn') != undefined )
    lcNode.setAttribute('fn', lc.getAttribute('fn'));
  
  var newdestblock  = lc.getAttribute('destblockid');
  var prevdestblock = lcNode.getAttribute('destblockid');
  var destblockchanged = false;
  if( newdestblock != undefined && newdestblock != prevdestblock ) {
    lcNode.setAttribute('destblockid', newdestblock);
    destblockchanged = true;
  }
  
  var newblock  = lc.getAttribute('blockid');
  var prevblock = lcNode.getAttribute('blockid');
  var blockchanged = false;
  if( newblock != undefined && newblock != prevblock ) {
    lcNode.setAttribute('blockid', newblock);
    blockchanged = true;
  }
  
  var newblockenterside  = lc.getAttribute('blockenterside');
  var prevblockenterside = lcNode.getAttribute('blockenterside');
  var blockentersidechanged = false;
  if( newblockenterside != undefined && newblockenterside != prevblockenterside ) {
    lcNode.setAttribute('blockenterside', newblockenterside);
    blockentersidechanged = true;
  }
  
  var newtrain  = lc.getAttribute('train');
  var prevtrain = lcNode.getAttribute('train');
  var trainchanged = false;
  if( newtrain != undefined && newtrain != prevtrain) {
    lcNode.setAttribute('train', newtrain);
    trainchanged = true;
  }
  if( lc.getAttribute('trainlen') != undefined )
    lcNode.setAttribute('trainlen', lc.getAttribute('trainlen'));
  if( lc.getAttribute('trainweight') != undefined )
    lcNode.setAttribute('trainweight', lc.getAttribute('trainweight'));

  var newplacing  = lc.getAttribute('placing');
  var prevplacing = lcNode.getAttribute('placing');
  var placingchanged = false;
  if( newplacing != undefined && newplacing != prevplacing) {
    lcNode.setAttribute('placing', newplacing);
    placingchanged = true;
  }
  
  if( lc.getAttribute('consist') != undefined )
    lcNode.setAttribute('consist', lc.getAttribute('consist'));

  if( modechanged || blockentersidechanged || trainchanged || placingchanged ) {
    var bk = findBlock4Loco(lc.getAttribute('id'));
    if( bk != undefined ) {
      if( modechanged )
        updateBlockstate(bk.getAttribute('id'), bk.getAttribute('statesignal'), lc.getAttribute('id'), "loco");
      if( blockentersidechanged || trainchanged ) {
        var div = document.getElementById("bk_"+bk.getAttribute('id'));
        getBlockLabel(bk, div);
      }
    }
  }

  if( lc.getAttribute('id') == locoSelected && lc.getAttribute('V') != undefined ) {
    var V = parseInt(lc.getAttribute('V'));
    speedUpdateVal = V;
  }
  
  if( ! $(popupThrottle).parent().hasClass('ui-popup-hidden') ) {
    if( lc.getAttribute('id') == locoSelected ) {
      initThrottleStatus();
      updateDir();
      if( localStorage.getItem("allspeedsteps") != "true" ) {
        updateSpeed(lcNode);
      }
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
  if( fn.getAttribute('fnchanged') == undefined)
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
      div.style.backgroundColor = "";
  }
}

function updateBlockstate( bkid, sgid, lcid, from ) {
  sg = sgMap[sgid];
  lc = lcMap[lcid];
  if( sg == undefined )
    return;

  var div = document.getElementById("sg_"+sgid);
  var label = "-";
  var color = greyBackground;
  if( lc != undefined ) {
    var mode = lc.getAttribute('mode');
    var modereason = lc.getAttribute('modereason');
    var nodest = false;
    if( modereason != undefined && "nodest" == modereason );
    nodest = true;

    if( mode == "auto" ) {
      label = "A";
      if( mode == "halfauto" )
        color = blueBackground;
      else
        color = greenBackground;
    }
    else if( mode == "idle" ) {
      label = "O";
    }
    else if( mode == "wait" ) {
      label = "W";
      if( nodest )
        color = yellowBackground;
      else if( mode == "halfauto" )
        color = blueBackground;
      else
        color = greenBackground;
    }
    else if( mode == "halfauto" ) {
      label = "H";
      color = blueBackground;
    }
  }
  trace("blockstate: block="+bkid+" signal="+sgid+" loco="+lcid+" from="+from+" label="+label);
  div.innerHTML = "<label class='itemtext' style='line-height:20px; background-color: "+color+";position:relative;left:5px;top:5px;width:21px;height:21px;'>"+label+"</label>";
}

function getBlockLabel(bk, div) {
  bkNode = bkMap[bk.getAttribute('id')];
  var ori   = getOri(bkNode);
  var small = bkNode.getAttribute('smallsymbol');
  var locid = bk.getAttribute('locid');
  var label = bk.getAttribute('locid');
  var isEntering = false;
  var isReserved = false;
  var occupied   = 0;
  
  if(locid != undefined && locid.length > 0) {
    if( "true" == bk.getAttribute('entering') )
     isEntering = true;
    if( "true" == bk.getAttribute('reserved') )
     isReserved = true;
    occupied = isReserved ? 2:1;
    occupied = isEntering ? 3:occupied;
  }
  
  if( localStorage.getItem("showblockid") == "true" && (occupied == 1 || occupied == 3) && "true" != small ) {
    var lc = lcMap[locid];
    if( localStorage.getItem("showlocoimage") == "true" && lc != undefined ) {
      var image = lc.getAttribute('image');
      label = bk.getAttribute('id') + "<img src='"+image+"' style='height:22px;vertical-align:middle'/>";
    }
    else {
      var placing = "";
      if( lc != undefined && lc.getAttribute('placing') != undefined && lc.getAttribute('placing') == "false" )
        placing = "&lt;";
      label = bk.getAttribute('id') + ":" + placing + bk.getAttribute('locid');
    }
  }
  else if( localStorage.getItem("showlocoimage") == "true" && (occupied == 1 || occupied == 3) ) {
    var lc = lcMap[locid];
    var image = lc.getAttribute('image');
    label = "<img src='"+image+"' style='height:22px;vertical-align:middle'/>";
  }
      
  if( label == undefined || label.length == 0 ) {
    label = bk.getAttribute('id');
  }
  else {
    trace("label: "+label);
    var lc = lcMap[locid];
    if( lc != undefined ) {
      var rotate  = lc.getAttribute('blockenterside');
      var train   = lc.getAttribute('train');
      var placing = "";
      
      if( lc.getAttribute('placing') != undefined && lc.getAttribute('placing') == "false" )
        placing = "-";
      
      if( localStorage.getItem("showtrainid") == "true" && train != undefined && train.length > 0 )
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
        else if( ori=="north" ) {
          newLabel = "< " + label;
          if( (ori=="north" && rotate=="true" ) ) {
            newLabel = label + " >";
          }
        }
        else if( ori=="south" ) {
          newLabel = "< " + label;
          if( (ori=="south" && rotate=="true" ) ) {
            newLabel = label + " >";
          }
        }
        label = newLabel;
      }
      
    }
  }
  
  
  if( ori == "north" || ori == "south" ) {
    // Work around for rotated labels:
    //label = label.split(' ').join('.');
    //label = label.split('-').join('.');
    
    var labdiv = document.createElement('div');
    if( ori == "north" ) {
      labdiv.setAttribute('class', "itemtextVnorth");
      if( bkNode.getAttribute('smallsymbol') == "true" )
        labdiv.style.left = "-32px";
      else
        labdiv.style.left = "-96px";
    }
    else {
      labdiv.setAttribute('class', "itemtextVsouth");
      labdiv.style.left = "0px";
    }
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
    labdiv.innerHTML      = label;
    trace("vert label: "+label);
    div.innerHTML      = "";
    div.appendChild(labdiv);
  }
  else {
    div.innerHTML = "<label class='itemtext' style='font-size:"+blockPointsize+"px;'>"+label+"</label>";
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
    if( fy.getAttribute('locid') != undefined )
      fyNode.setAttribute('locid', fy.getAttribute('locid'));
    if( fy.getAttribute('state') != undefined )
      fyNode.setAttribute('state', fy.getAttribute('state'));
    if( fy.getAttribute('pos') != undefined )
      fyNode.setAttribute('pos', fy.getAttribute('pos'));
    
    div.innerHTML = getFiddleYardImage(fyNode, div);
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

  }
  else {
    trace("staging block: " + sb.getAttribute('id') + " not found");
  }
}

function handleOperator(operator) {
  var cmd = operator.getAttribute('cmd');
  trace("operator: "+cmd);
  op = operatorMap[operator.getAttribute('id')];
  op.setAttribute('carids', operator.getAttribute('carids'));
}



function handleClock(clock) {
  var cmd = clock.getAttribute('cmd');
  trace("clock: "+cmd);
  if( cmd == "sync" ) {
    Time = parseInt(clock.getAttribute('time'));
    var d = new Date(Time*1000);
    var min = ""+d.getMinutes() ;
    if(min.length == 1) min = "0" + min;
    timelabel = ""+d.getHours()+":"+min+ " ";
    trace(timelabel);
    var zlevel = zlevelList[zlevelIdx];
    var title = zlevel.getAttribute('title');
    var h = document.getElementById("title");
    if( ModPlan )
      title = Title;
    h.innerHTML = timelabel + title;
    
    if( clock.getAttribute('divider') == undefined )
      Divider = 1;
    else
      Divider = parseInt(clock.getAttribute('divider'));
  }
}

function rotate(id, angle) {
  var element = document.getElementById(id);
  if (element) {
    element.setAttribute('transform', 'rotate(' + angle + ', 100, 100)');
    if (element.getAttribute('visibility') == 'hidden') {
      element.setAttribute('visibility', 'visible');
    }
  }
}

function doFastClock() {
  Time++;
  var now     = new Date(Time*1000);
  var hours   = now.getHours();
  var minutes = now.getMinutes();
  var seconds = now.getSeconds();
  rotate('hourHand',   hours * 30 + minutes * 0.5);
  rotate('minuteHand', minutes * 6);
  if( Divider < 5 )
    rotate('secondHand', 6 * seconds + 3 * (1 + Math.cos(Math.PI)));
  else {
    var element = document.getElementById('secondHand');
    element.setAttribute('visibility', 'hidden');
  }
  setTimeout( doFastClock, (1000/Divider) );
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
      if( lclist[i].getAttribute('generated') == "true" )
        addLocoToList(lclist[i], getString("guests"), false);
      else
        addLocoToList(lclist[i], null, false);
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
      div.style.backgroundColor = "";
      div.style.backgroundImage = getRouteImage(st);
    }

  }
}


function handleTrack(tk) {
  var div = document.getElementById("tk_"+tk.getAttribute('id'));
  if( div == null )
    return;
  div.style.backgroundImage = getTrackImage(tk);
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
    for( var i = 0; i < zlevelDivList.length; i++ ) {
      $(zlevelDivList[i]).css({'-webkit-transform': 'scale(' + scale + ')'});
      $(zlevelDivList[i]).css({'-moz-transform': 'scale(' + scale + ')'});
    }
  }
  else if( !zoomin && scale > 0.5 ) {
    scale -= 0.1;
    localStorage.setItem("scale", scale);
    for( var i = 0; i < zlevelDivList.length; i++ ) {
      $(zlevelDivList[i]).css({'-webkit-transform': 'scale(' + scale + ')'});
      $(zlevelDivList[i]).css({'-moz-transform': 'scale(' + scale + ')'});
    }
  }
}

function onZoom100() {
  scale = 1.0;
  localStorage.setItem("scale", scale);
  for( var i = 0; i < zlevelDivList.length; i++ ) {
    $(zlevelDivList[i]).css({'-webkit-transform': 'scale(' + scale + ')'});
    $(zlevelDivList[i]).css({'-moz-transform': 'scale(' + scale + ')'});
  }
}

function parseString(xmlStr) {
  var xmlDoc = parser.parseFromString(xmlStr, "text/xml");
  return xmlDoc.documentElement;
}

/* Processing events from server */
function evaluateEvent(xmlStr) {
  trace("evaluate: " + xmlStr);
  if( xmlStr.indexOf("<fb") == 0 )
    handleSensor(parseString(xmlStr));
  else if( xmlStr.indexOf("<bk") == 0 )
    handleBlock(parseString(xmlStr));
  else if( xmlStr.indexOf("<state") == 0 )
    handleState(parseString(xmlStr));
  else if( xmlStr.indexOf("<auto") == 0 )
    handleAuto(parseString(xmlStr));
  else if( xmlStr.indexOf("<sys") == 0 )
    handleSystem(parseString(xmlStr));
  else if( xmlStr.indexOf("<fn") == 0 )
    handleFunction(parseString(xmlStr));
  else if( xmlStr.indexOf("<lc") == 0 )
    handleLoco(parseString(xmlStr));
  else if( xmlStr.indexOf("<sw") == 0 )
    handleSwitch(parseString(xmlStr));
  else if( xmlStr.indexOf("<co") == 0 )
    handleOutput(parseString(xmlStr));
  else if( xmlStr.indexOf("<sg") == 0 )
    handleSignal(parseString(xmlStr));
  else if( xmlStr.indexOf("<tx") == 0 )
    handleText(parseString(xmlStr));
  else if( xmlStr.indexOf("<sb") == 0 )
    handleStageBlock(parseString(xmlStr));
  else if( xmlStr.indexOf("<tt") == 0 )
    handleTurntable(parseString(xmlStr));
  else if( xmlStr.indexOf("<seltab") == 0 )
    handleFiddleYard(parseString(xmlStr));
  else if( xmlStr.indexOf("<st") == 0 )
    handleRoute(parseString(xmlStr));
  else if( xmlStr.indexOf("<model") == 0 )
    handleModel(parseString(xmlStr));
  else if( xmlStr.indexOf("<clock") == 0 )
    handleClock(parseString(xmlStr));
  else if( xmlStr.indexOf("<operator") == 0 )
    handleOperator(parseString(xmlStr));
  else if( xmlStr.indexOf("<debug") == 0 )
    trace(xmlStr);
  else if( xmlStr.indexOf("<alert") == 0 ) {
    console.log(xmlStr);
    document.getElementById("headerMenu").style.backgroundColor= redBackground;
  }
  else if( xmlStr.indexOf("<fatal") == 0 )
    alert(xmlStr);
  else if( xmlStr.indexOf("<connected") == 0 ) {
    trace(xmlStr);
    document.getElementById("headerMenu").style.backgroundColor= '';
  }
  else 
    trace("unhandled XML: "+xmlStr); 
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
        var locosort = localStorage.getItem("locosort");
        if(locosort == undefined || locosort.length == 0) {
          localStorage.setItem("locosort", "id");
        }
        var showocc = localStorage.getItem("showocc");
        if(showocc == undefined || showocc.length == 0) {
          localStorage.setItem("showocc", "true");
        }
        var sliderdelta = localStorage.getItem("sliderdelta");
        if(sliderdelta == undefined || sliderdelta.length == 0) {
          localStorage.setItem("sliderdelta", "3");
        }
        else {
          sliderDelta = parseInt(sliderdelta); 
        }
        var controlcode = localStorage.getItem("controlcode");
        if(controlcode == undefined || controlcode.length == 0) {
          localStorage.setItem("controlcode", "");
        }
        else {
          controlCode = controlcode; 
        }
        var slavecode = localStorage.getItem("slavecode");
        if(slavecode == undefined || slavecode.length == 0) {
          localStorage.setItem("slavecode", "");
        }
        else {
          slaveCode = slavecode; 
        }
        
        planlist = xmlDoc.getElementsByTagName("plan")
        if(planlist.length == 0)
          planlist = xmlDoc.getElementsByTagName("modplan")
          
        if( planlist.length > 0 ) {
          var h = document.getElementById("title");
          donkey = planlist[0].getAttribute('donkey');
          Title = planlist[0].getAttribute('title');
          trace("title: "+Title);
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
          
          
          trace( "processing plan: " + Title + " key=" + donkey );
          h.innerHTML = Title;
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
          var binstate = localStorage.getItem("binstate");
          document.getElementById("binStateDiv").style.display = 'none';
          if( binstate != undefined && binstate == "true" )
            document.getElementById("binStateDiv").style.display = 'block';
          
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
              Disconnect(false)
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
          evaluateEvent(e.data);
          if( !didShowDonkey && donkey == 'false' ) {
            openDonkey();
            didShowDonkey = true;
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
  var aspect  = sg.getAttribute('aspect');
  var pattern = parseInt( sg.getAttribute('usepatterns') );
  var route   = sg.getAttribute('route');
  var occ     = sg.getAttribute('occ');
  var symbolprefix    = sg.getAttribute('symbolprefix');
  var usesymbolprefix = sg.getAttribute('usesymbolprefix');
  var suffix = ""; 
  
  // symbolprefix + 'signalaspect-' + aspect + '.svg'
  var iAspects = parseInt(aspects);
  var iAspect  = parseInt(aspect);
  if( iAspects > 4 && symbolprefix != undefined )
    usesymbolprefix = "true";
  
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
  if( usesymbolprefix == "true" ) {
    return "url('"+symbolprefix+"signalaspect-"+iAspect+"."+ ori + ".svg')";
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


function getClockImage(labelTxt) {
  // $(window).height()
  var width  = $(window).width();
  var height = $(window).height()-yoffset;
  if( width > height ) 
    width = height;  
  else if( width < height ) 
    height = width;  
  
  var label = "Here comes the fast clock..."+labelTxt;
  var svg = 
    "<svg id='fastClock' xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' xmlns:ev='http://www.w3.org/2001/xml-events'"+
    "  version='1.1' baseProfile='full' width='100%' height='100%' viewBox='0 0 200 200'> "+
    " <g id='hourHand' visibility='hidden'>" +
    "   <polygon points='94,46 100,40 106,46 106,118 94,118' style='fill:#222; stroke:none'/>" +
    " </g>" +
    " <g id='minuteHand' visibility='hidden'>" +
    "   <polygon points='95.5,11.5 100,7 104.5,11.5 104.5,122 95.5,122' style='fill:#222; stroke:none'/>" +
    " </g>" +
    " <g id='secondHand' visibility='hidden'>" +
    "   <polygon points='98.8,11 100,9.8 101.2,11 101.6,42 98.4,42' style='fill:#ad1a14; stroke:none'/>" +
    "   <polygon points='98.1,58 101.9,58 102.5,122 97.5,122' style='fill:#ad1a14; stroke:none'/>" +
    "   <circle cx='100' cy='50' r='8.5' style='fill:none; stroke:#ad1a14; stroke-width:6.5'/>" +
    " </g>" +
    " <g>" +
    "   <circle cx='100' cy='100' r='4' style='fill:white; stroke:#ad1a14; stroke-width:2'/>" +
    " </g>" +
    "</svg>";
  
  return svg;
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
    label = fy.getAttribute('id') + "[" + fy.getAttribute('pos') + "]";
  else {
    fill = "255,100,100";
    label = fy.getAttribute('id') + "[" + fy.getAttribute('pos') + "] " + label;
  }

  var transform = "rotate("+rotate+", "+(32/2)+", "+(32/2)+")";


  var svg = 
    "<svg xmlns='http://www.w3.org/2000/svg' width='"+symwidth+"' height='"+(div.style.height)+"'>" +
    "  <g transform='"+transform+"'>" +
    "   <rect x='2' y='2' rx='8' ry='8' width='"+width+"' height='28' fill='rgb("+fill+")' stroke-width='2' stroke='rgb(0,0,0)'/>" +
    "   <text x='8' y='22' fill='black' font-size='"+blockPointsize+"px'>"+label+"</text>" +
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
  else if( "ghost" == bk.getAttribute('state') )
    return "url('block-ghost"+suffix+"."+ori+".svg')";
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

function addCatToList(div, lcCat, opencat) {
  var newdiv = document.createElement('div');
  newdiv.setAttribute('data-role', "collapsible");
  newdiv.setAttribute('class', "ui-collapsible ui-collapsible-inset ui-corner-all ui-collapsible-themed-content ui-collapsible-collapsed ui-first-child");
  if( opencat ) {
    trace("first: "+lcCat);
    newdiv.setAttribute('data-collapsed', 'false' );
  }
  
  var h2 = document.createElement('h2');
  h2.innerHTML = getString(lcCat);
  newdiv.appendChild(h2);
  var ul = document.createElement('ul');
  ul.setAttribute('id', lcCat);
  ul.setAttribute('data-role', "listview");
  ul.setAttribute('class', "ui-listview");
  ul.setAttribute('style', "max-height: 300px; overflow: auto;");
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
  var img = "<img src='noimg.png' height='40'/>";
  if( image != undefined && image.length > 0 )
    img = "<img src='"+image+"' height='40'/>";
  var dirImg = "<img src='fwd.png'/>";
  if( dir == "false" )
    dirImg = "<img src='rev.png'/>";
  var row = "<table><tr><td style='width: 160px;'>"+img+"<td>"+dirImg+"<td>"+id+"<br>"+addr+"</table>";
  li.innerHTML = "<a href='#' class='ui-btn ui-btn-icon-right ui-icon-carat-r'>"+row+"</a>";
  cat.appendChild(li);
  $('div[data-role=collapsible]').collapsible({refresh:true});  
}

function addCarToList(car, opencat) {
  var addr = car.getAttribute('addr');
  if( addr == "0" )
    return;
  var category = localStorage.getItem("category");
  var div = document.getElementById("locoSelectList");
  
  var lcCat = "car";
  var cat = lcCatMap[lcCat];
  if( cat == undefined ) {
    cat = addCatToList(div, lcCat, false);
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

function isCarFree(carid) {
  for (var key in operatorMap) {
    var op = operatorMap[key];
    var carids = op.getAttribute('carids');
    if( carids.indexOf(carid) != -1 )
      return false;
  }
  return true;
}

function initLocoList(action) {
  trace("initLocoList: "+action);
  locoSelectAction = action;
  var locoSelectList = document.getElementById("locoSelectList");
  locoSelectList.innerHTML = "";
  for (var key in lcCatMap) delete lcCatMap[key];
  lcCatMap.length = 0;
  
  var firstCat = true;
  if( action == "select" ) {
    for (var key in lcMap) {
      var lc = lcMap[key];
      if( lc.getAttribute('generated') == "true" )
        addLocoToList(lc, getString("guests"), false);
      else {
        addLocoToList(lc, null, firstCat);
        firstCat = false;
      }
    }
    for (var key in carMap) {
      var car = carMap[key];
      addCarToList(car);
    }
  }

  else if( action == "trainaddcar" || action == "trainleavecar" || action == "trainloadcar" || action == "trainemptycar" ) {
    var lc = lcMap[locoSelected];
    var train = lc.getAttribute('train');
    var operator = operatorMap[train];
    if( operator != undefined ) {
      if( action == "trainaddcar" ) {
        var op = operatorMap[train];
        var carids = op.getAttribute('carids');
        for (var key in carMap) {
          var car = carMap[key];
          var carid = car.getAttribute('id');
          if( isCarFree(carid) ) {
            addLocoToList(car, getString(action), firstCat);
            firstCat = false;
          }
        }
      }
      else if( action == "trainleavecar" || action == "trainloadcar" || action == "trainemptycar" ) {
        var op = operatorMap[train];
        var carids = op.getAttribute('carids');
        for (var key in carMap) {
          var car = carMap[key];
          var carid = car.getAttribute('id');
          if( carids.indexOf(carid) != -1 ) {
            addLocoToList(car, getString(action), firstCat);
            firstCat = false;
          }
        }
      }
      
    }
  }  

  else if( action == "consistadd" || action == "consistdel" || action == "consistshow" ) {
    var master = lcMap[locoSelected];
    var masterid = master.getAttribute('id');
    if( action == "consistshow" ) {
      addLocoToList(master, getString("master"), true);
    }
    if(master != undefined) {
      var slaves = master.getAttribute('consist');
      for (var key in lcMap) {
        var lc = lcMap[key];
        var slaveid = lc.getAttribute('id');
        if( action == "consistadd" && slaveid != masterid && slaves.indexOf(slaveid) == -1 ) {
          addLocoToList(lc, null, firstCat);
          firstCat = false;
        }
        else if( action == "consistdel" && slaveid != masterid && slaves.indexOf(slaveid) != -1 ) {
          addLocoToList(lc, getString("slaves"), firstCat);
          firstCat = false;
        }
        else if( action == "consistshow" && slaveid != masterid && slaves.indexOf(slaveid) != -1 )
          addLocoToList(lc, getString("slaves"), false);
      }
    }
  }
  
}

function addLocoToList(lc, catname, opencat) {
  var category = localStorage.getItem("category");

  var div = document.getElementById("locoSelectList");
  var lcCat = "diesel";
  if( catname != null && catname.length > 0) {
    trace("catname="+catname);
    lcCat = catname;
  }
  else if( category == "engine" ) {
    lcCat = lc.getAttribute('engine');
    if( lcCat == undefined || lcCat.length == 0 )
      lcCat = "diesel";
  }
  else {
    lcCat = getMobileCategory(lc);
  }
  var cat = lcCatMap[lcCat];
  if( cat == undefined ) {
    cat = addCatToList(div, lcCat, opencat);
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

function sortById(a, b) {
  var attrname = "id";
  if (a.getAttribute(attrname) > b.getAttribute(attrname))
    return 1;
  if (a.getAttribute(attrname) < b.getAttribute(attrname))
    return -1;
  // a must be equal to b
  return 0;
}

function processPlan() {
  
   try {
     xmlDoc = req.responseXML;
     
     /* ----------------------------------------
      * ZLevels
      */
     zlevellist = xmlDoc.getElementsByTagName("zlevel");
     if( zlevellist.length > 0 ) {
       var title = "";
       var i = 0;
       trace("processing " + zlevellist.length + " zlevels");
       for(i = 0; i < zlevellist.length; i++) {
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

         $(newdiv).css({'-webkit-transform': 'scale(' + scale + ')'});
         $(newdiv).css({'-moz-transform': 'scale(' + scale + ')'});
         
         zlevelDivMap[z] = newdiv;
         zlevelDivList[i] = newdiv;

         if( zlevelSelected == 'none' ) {
           zlevelSelected = newdiv;
           zlevelIdx = i;
           title = zlevellist[i].getAttribute('title');
         }
         else if( !ModPlan ) {
           trace("disable level " + z);
           newdiv.style.display = 'none';
         }
         document.body.appendChild(newdiv);
       }
       
       var analogclock = localStorage.getItem("analogclock");
       if( analogclock == undefined || analogclock == "true" ) {
         var clocklevel = document.createElement('zlevel');
         clocklevel.setAttribute('id', getString("fastclock"));
         var clockZ = "1000";
         clocklevel.setAttribute('z', clockZ);
         clocklevel.setAttribute('title', getString("fastclock"));
         var clockdiv = document.createElement('div');
         clockdiv.setAttribute('id', "level_" + clockZ);
         clockdiv.setAttribute('overflow-x', "auto");
         clockdiv.setAttribute('overflow-y', "auto");
         clockdiv.setAttribute('onClick', "openClock(this.id)");
         clockdiv.style.position = "absolute";
         clockdiv.style.left = "0px";
         clockdiv.style.top = ""+yoffset+"px";
         clockdiv.style.width = "100%";
         clockdiv.style.height = "100%";
         zlevelMap[""+clockZ]  = clocklevel;
         zlevelList[i] = clocklevel;
         zlevelDivMap[""+clockZ]  = clockdiv;
         zlevelDivList[i] = clockdiv;
         clockdiv.style.display = 'none';
         clockdiv.innerHTML = getClockImage();
         clockdiv.style.backgroundImage = "url('clock.svg')";
         clockdiv.style.backgroundRepeat = "no-repeat";
         clockdiv.style.backgroundPosition = "center";
         //clockdiv.style.backgroundSize = "90% 90%";
         document.body.appendChild(clockdiv);
         Time = new Date();
         setTimeout( doFastClock, (1000/Divider) );
       }

       
       if( !ModPlan ) {
         var h = document.getElementById("title");
         h.innerHTML = title;
       }
     }
     
     
     /* ----------------------------------------
      * Locos
      */
     lclistRaw = xmlDoc.getElementsByTagName("lc");
     if( lclistRaw.length > 0 ) {
       var lclist = Array.prototype.slice.call(lclistRaw, 0);
       lclist.sort( function (a, b) {
         var locosort = localStorage.getItem("locosort");
         var attrname = "id";
         if( locosort == "address" ) {
           attrname = "addr";
           if(parseInt(a.getAttribute(attrname)) > parseInt(b.getAttribute(attrname)))
             return 1;
           if(parseInt(a.getAttribute(attrname)) < parseInt(b.getAttribute(attrname)))
             return -1;
         }
         else {
           if (a.getAttribute(attrname) > b.getAttribute(attrname))
             return 1;
           if (a.getAttribute(attrname) < b.getAttribute(attrname))
             return -1;
         }
         // a must be equal to b
         return 0;
       });
     
       trace("processing " + lclist.length + " locos");
     
       for (var i = 0; i < lclist.length; i++) {
         var show  = lclist[i].getAttribute('show');
         if( show != undefined && show == "false" )
           continue;
  
         trace('loco: ' + lclist[i].getAttribute('id') );
         lcMap[lclist[i].getAttribute('id')] = lclist[i];
         addLocoToList(lclist[i], null, false);
       }
     }

     
     /* ----------------------------------------
      * Cars
      */
     carlistRaw = xmlDoc.getElementsByTagName("car");
     if( carlistRaw.length > 0 ) {
       var carlist = Array.prototype.slice.call(carlistRaw, 0);
       lclist.sort( function (a, b) {
         return sortById(a, b);
       });
       trace("processing " + carlist.length + " cars");
     
       for (var i = 0; i < carlist.length; i++) {
         trace('car: ' + carlist[i].getAttribute('id') );
         carMap[carlist[i].getAttribute('id')] = carlist[i];
         addCarToList(carlist[i]);
       }
     }
     
     
     
     /* ----------------------------------------
      * Outputs
      */
     colist = xmlDoc.getElementsByTagName("co");
     if( colist.length > 0 )
       trace("processing " + colist.length + " outputs");

     for (var i = 0; i < colist.length; i++) {
       var show  = colist[i].getAttribute('show');
       if( show != undefined && show == "false" )
         continue;

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
       newdiv.setAttribute('title', colist[i].getAttribute('id'));
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
     
     
     /* ----------------------------------------
      * Signals
      */
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
       newdiv.setAttribute('title', sglist[i].getAttribute('id'));
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
     
     
     /* ----------------------------------------
      * Tracks
      */
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
     

     /* ----------------------------------------
      * Texts
      */
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
       newdiv.setAttribute('title', txlist[i].getAttribute('id'));
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       var input = txlist[i].getAttribute('manualinput');
       var clock = txlist[i].getAttribute('clock');
       if( input != undefined && input == "true")
         newdiv.setAttribute('onClick', "actionText(this.id)");
       else if( clock != undefined && clock == "true" )
         newdiv.setAttribute('onClick', "openClock(this.id)");
       
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
       newdiv.style.verticalAlign = 'middle';

       setXY(txlist[i], zlevelMap[z], newdiv);

       var border = txlist[i].getAttribute('border');
       var borderWidth = 0;
       if( border != undefined && border == "true" ) {
         newdiv.style.border = '1px solid black';
         borderWidth = 2;
         newdiv.style.left = "" + (parseInt(newdiv.style.left)+1);
         newdiv.style.top  = "" + (parseInt(newdiv.style.top)+1);
       }
       
       if( ori == "north" || ori == "south" ) {
         newdiv.style.width    = "" + (parseInt(txlist[i].getAttribute('cy')) * 32 - borderWidth) + "px";
         newdiv.style.height   = ""  + (parseInt(txlist[i].getAttribute('cx')) * 32 - borderWidth) + "px";
       }
       else {
         newdiv.style.width    = "" + (parseInt(txlist[i].getAttribute('cx')) * 32 - borderWidth) + "px";
         newdiv.style.height   = ""  + (parseInt(txlist[i].getAttribute('cy')) * 32 - borderWidth) + "px";
       }

       getTextImage(txlist[i], newdiv);
       leveldiv.appendChild(newdiv);
     }
     

     /* ----------------------------------------
      * Switches
      */
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
       newdiv.setAttribute('title', swlist[i].getAttribute('id'));
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
     
     
     /* ----------------------------------------
      * Sensors
      */
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
       newdiv.setAttribute('title', fblist[i].getAttribute('id'));
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
     
     
     /* ----------------------------------------
      * Blocks
      */
     bklistRaw = xmlDoc.getElementsByTagName("bk");
     if( bklistRaw.length > 0 ) {
       var bklist = Array.prototype.slice.call(bklistRaw, 0);
       bklist.sort( function (a, b) {
         return sortById(a, b);
       });
     
       trace("processing " + bklist.length + " blocks");
       for (var i = 0; i < bklist.length; i++) {
         var show  = bklist[i].getAttribute('show');
         if( show != undefined && show == "false" )
           continue;
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
         newdiv.setAttribute('title', bklist[i].getAttribute('id'));
         newdiv.setAttribute('onClick', "actionBlock(this.id, false)");
         newdiv.setAttribute('class', "item");
         newdiv.style.overflow = 'hidden';
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
     }
     

     /* ----------------------------------------
      * Turntables
      */
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
       newdiv.setAttribute('title', ttlist[i].getAttribute('id'));
       newdiv.setAttribute('onClick', "actionTurntable(this.id)");
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       newdiv.style.width    = ""+(symbolsize*32)+"px";
       newdiv.style.height   = ""+(symbolsize*32)+"px";
       setXY(ttlist[i], zlevelMap[z], newdiv);
       newdiv.innerHTML  = getTurntableImage(ttlist[i], newdiv); 
       leveldiv.appendChild(newdiv);
     }
     
     
     /* ----------------------------------------
      * Schedules
      */
     sclistRaw = xmlDoc.getElementsByTagName("sc");
     if( sclistRaw.length > 0 ) {
       var sclist = Array.prototype.slice.call(sclistRaw, 0);
       sclist.sort( function (a, b) {
         return sortById(a, b);
       });
     
       trace("processing " + sclist.length + " schedules");
       for (var i = 0; i < sclist.length; i++) {
         scMap[sclist[i].getAttribute('id')] = sclist[i];
       }
     }     
     

     /* ----------------------------------------
      * Tours
      */
     tourlistRaw = xmlDoc.getElementsByTagName("tour");
     if( tourlistRaw.length > 0 ) {
       var tourlist = Array.prototype.slice.call(tourlistRaw, 0);
       tourlist.sort( function (a, b) {
         return sortById(a, b);
       });
       trace("processing " + tourlist.length + " tours");
       for (var i = 0; i < tourlist.length; i++) {
         tourMap[tourlist[i].getAttribute('id')] = tourlist[i];
       }
     }     
     

     /* ----------------------------------------
      * Locations
      */
     locationlistRaw = xmlDoc.getElementsByTagName("location");
     if( locationlistRaw.length > 0 ) {
       var locationlist = Array.prototype.slice.call(locationlistRaw, 0);
       locationlist.sort( function (a, b) {
         return sortById(a, b);
       });
       trace("processing " + locationlist.length + " locations");
       for (var i = 0; i < locationlist.length; i++) {
         locationMap[locationlist[i].getAttribute('id')] = locationlist[i];
       }
     }
     
     
     /* ----------------------------------------
      * Operators (Trains)
      */
     operatorlistRaw = xmlDoc.getElementsByTagName("operator");
     if( operatorlistRaw.length > 0 ) {
       var operatorlist = Array.prototype.slice.call(operatorlistRaw, 0);
       operatorlist.sort( function (a, b) {
         return sortById(a, b);
       });
       trace("processing " + operatorlist.length + " operators");
       for (var i = 0; i < operatorlist.length; i++) {
         operatorMap[operatorlist[i].getAttribute('id')] = operatorlist[i];
       }
     }
     
     
     /* ----------------------------------------
      * Routes
      */
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
         newdiv.setAttribute('title', stlist[i].getAttribute('id'));
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
     
     
     /* ----------------------------------------
      * Selection tables (Fiddleyards)
      */
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
       newdiv.setAttribute('title', fylist[i].getAttribute('id'));
       newdiv.setAttribute('onClick', "actionFiddleYard(this.id)");
       newdiv.setAttribute('class', "item");
       newdiv.style.position = "absolute";
       setXY(fylist[i], zlevelMap[z], newdiv);
       newdiv.innerHTML  = getFiddleYardImage(fylist[i], newdiv); 
       
       leveldiv.appendChild(newdiv);
     }
     

     /* ----------------------------------------
      * Staging blocks
      */
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
       newdiv.setAttribute('title', sblist[i].getAttribute('id'));
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


