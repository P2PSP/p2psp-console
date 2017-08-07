const net = require('net');

if(process.argv.length != 5) {
	console.log('Usage: node vlc-sourceClient.js <splitterPort> <channelName> <vlcPort>');
	process.exit(1);
}

const splitterPort = parseInt(process.argv[2]);
const channelName = process.argv[3];
const vlcPort = parseInt(process.argv[4]);

const splitterClient = new net.Socket();
const vlcClient = new net.Socket();

splitterClient.connect(splitterPort, '127.0.0.1', () => {
    console.log('Connected to SPLITTER.');

	vlcClient.connect(vlcPort, '127.0.0.1', () => {
		console.log('Connected to VLC.');
		vlcClient.write('GET /' + channelName +' HTTP/1.1\r\n\r\n');
	});

	vlcClient.on('data', (data) => {
		splitterClient.write(data);
	});

	vlcClient.on('close', () => {
		console.log('VLC Connection closed.');
	});
});

splitterClient.on('close', () => {
	console.log('Splitter Connection closed.');
});
