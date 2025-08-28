const WebSocket = require('ws');

const SERVER_URL = 'ws://localhost:4000';

if (process.argv.length < 3) {
    console.error('Usage: node 11-04a.js <client_name>');
    process.exit(1);
}
const clientName = process.argv[2];

console.log(`Starting client with name: "${clientName}"`);

const ws = new WebSocket(SERVER_URL);

ws.on('open', () => {
    console.log(`Connected to server: ${SERVER_URL}`);

    const messageData = {
        client: clientName,
        timestamp: new Date().toISOString() 
    };
    const messageJson = JSON.stringify(messageData);

    console.log('Sending message:', messageJson);
    ws.send(messageJson, (err) => {
        if (err) {
            console.error('Error sending message:', err);
        }
    });

});

ws.on('message', (message) => {
    try {
        const receivedData = JSON.parse(message.toString());
        console.log('Received from server:', receivedData);
    } catch (e) {
        console.error('Received non-JSON message or invalid JSON from server:', message.toString());
    }
});

ws.on('close', (code, reason) => {
    const reasonString = reason ? reason.toString() : 'N/A';
    console.log(`Disconnected from server. Code: ${code}, Reason: ${reasonString}`);
});

ws.on('error', (error) => {
    console.error(`WebSocket error: ${error.message}`);
    if (error.code === 'ECONNREFUSED') {
        console.error(`Could not connect to ${SERVER_URL}. Is the server running?`);
    }
});

console.log('Client setup complete. Attempting to connect...');