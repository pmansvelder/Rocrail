
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
  $( "#popupMenu" ).popup( "open" );
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
});