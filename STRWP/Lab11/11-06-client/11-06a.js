const WebSocket = require('ws');

const SERVER_URL = 'ws://localhost:4000';
const EVENT_TO_SUBSCRIBE = 'A';
const RECONNECT_INTERVAL = 5000;
const CLIENT_NAME = `[Client ${EVENT_TO_SUBSCRIBE}]`;

let ws = null;

function connect() {
    console.log(`${CLIENT_NAME} Attempting to connect to ${SERVER_URL}...`);
    ws = new WebSocket(SERVER_URL);

    ws.on('open', () => {
        console.log(`${CLIENT_NAME} Connected to server.`);
        const subscriptionMessage = JSON.stringify({
            type: 'subscribe',
            event: EVENT_TO_SUBSCRIBE
        });
        console.log(`${CLIENT_NAME} Sending subscription request: ${subscriptionMessage}`);
        ws.send(subscriptionMessage);
    });

    ws.on('message', (message) => {
        try {
            const data = JSON.parse(message.toString());

            if (data.type === 'event' && data.event === EVENT_TO_SUBSCRIBE) {
                console.log(`${CLIENT_NAME} ===> Received subscribed event: ${data.event} (Timestamp: ${data.timestamp}) <===`);
            } else if (data.type === 'ack') {
                console.log(`${CLIENT_NAME} Received acknowledgment: ${data.status} for event ${data.event}`);
            } else if (data.type === 'error') {
                console.error(`${CLIENT_NAME} Received error from server: ${data.message}`);
            }
        } catch (e) {
            console.error(`${CLIENT_NAME} Error parsing message or invalid JSON:`, message.toString());
        }
    });

    ws.on('close', (code, reason) => {
        const reasonString = reason ? reason.toString() : 'N/A';
        console.log(`${CLIENT_NAME} Disconnected. Code: ${code}, Reason: ${reasonString}`);
        console.log(`${CLIENT_NAME} Reconnecting in ${RECONNECT_INTERVAL / 1000} seconds...`);
        ws = null;
        setTimeout(connect, RECONNECT_INTERVAL);
    });

    ws.on('error', (error) => {
        console.error(`${CLIENT_NAME} WebSocket error: ${error.message}`);
        if (ws && ws.readyState !== WebSocket.CLOSED && ws.readyState !== WebSocket.CLOSING) {
            ws.close();
        } else {
            ws = null;
             console.log(`${CLIENT_NAME} Connection failed. Retrying in ${RECONNECT_INTERVAL / 1000} seconds...`);
            setTimeout(connect, RECONNECT_INTERVAL);
        }
    });
}

connect(); 
console.log(`${CLIENT_NAME} started. Subscribing to event '${EVENT_TO_SUBSCRIBE}'. Press Ctrl+C to exit.`);