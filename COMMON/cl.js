

var net = require('net');

var client = new net.Socket();
client.connect(666, '127.0.0.1', function() {
	console.log('Connected');
	client.write('Hello, server! Love, Client.');
});

client.on('data', function(data) {
	console.log('Received: ' + data);
});

client.on('close', function() {
	console.log('Connection closed');
});

client.on('error', function() {
	console.log('error');
});




//
process.stdin.resume();
process.stdin.setEncoding('utf8');

  process.stdin.on('data', function (text) {
	  client.write(text);
  });
