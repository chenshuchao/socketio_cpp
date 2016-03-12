var socket = io('http://10.211.55.6:5011');
$('form').submit(function(){
  var m = $('#m').val();
  socket.emit('chat message', m);
  $('#m').val('');
  return false;
});
socket.on('chat message', function(msg){
  $('#messages').append($('<li>').text(msg));
});
