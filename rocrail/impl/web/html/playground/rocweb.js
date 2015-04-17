
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

