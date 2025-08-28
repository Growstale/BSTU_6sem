const WebSocket = require('ws');

const SERVER_URL = 'ws://localhost:4000';
const RECONNECT_INTERVAL = 5000; // Попытка переподключения каждые 5 секунд

let ws = null;

function connect() {
    console.log(`Attempting to connect to ${SERVER_URL}...`);
    ws = new WebSocket(SERVER_URL);

    ws.on('open', () => {
        console.log(`Connected successfully to ${SERVER_URL}`);
    });

    ws.on('message', (message) => {
        console.log(`Received from server: ${message.toString()}`);
    });

    ws.on('ping', (data) => {
        console.log(`Received ping from server`);
        // ws.pong() вызывается автоматически библиотекой 'ws'
    });

    ws.on('close', (code, reason) => {
        const reasonString = reason ? reason.toString() : 'N/A';
        console.log(`Disconnected from server. Code: ${code}, Reason: ${reasonString}`);
        console.log(`Will attempt to reconnect in ${RECONNECT_INTERVAL / 1000} seconds...`);
        ws = null;
        setTimeout(connect, RECONNECT_INTERVAL);
    });

    ws.on('error', (error) => {
        console.error(`WebSocket error: ${error.message}`);
        if (ws && ws.readyState !== WebSocket.CLOSED) {
             ws.close();
        } else {
            ws = null;
            console.log(`Connection failed. Retrying in ${RECONNECT_INTERVAL / 1000} seconds...`);
            setTimeout(connect, RECONNECT_INTERVAL);
        }
    });
}

// Запускаем первую попытку подключения
connect();

console.log('Client started. Press Ctrl+C to exit.');