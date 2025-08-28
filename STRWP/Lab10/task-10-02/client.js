const WebSocket = require('ws'); 
const WS_SERVER_URL = 'ws://localhost:4000'; 

console.log(`[Client 10-02] Attempting to connect to WebSocket server at ${WS_SERVER_URL}`);

const ws = new WebSocket(WS_SERVER_URL);

let messageInterval = null;       // ID интервала для отправки сообщений
let clientMessageCounter = 0;     // Счетчик отправленных клиентом сообщений
let connectionTimeout = null;     // ID таймаута для авто-остановки
const CLIENT_ID = `10-02-client-${Math.random().toString(36).substring(2, 8)}`;


function logMessage(message) {
    console.log(`[${new Date().toLocaleTimeString()}] [${CLIENT_ID}] ${message}`);
}

ws.on('open', () => {
    logMessage('WebSocket connection established.');
    clientMessageCounter = 0; 

    messageInterval = setInterval(() => {
        clientMessageCounter++;
        const message = `10-02-client: ${clientMessageCounter}`; 
        logMessage(`Sending: ${message}`);
        if (ws.readyState === WebSocket.OPEN) { 
            ws.send(message);
        } else {
            logMessage('Cannot send message, WebSocket is not open.');
            cleanupResources(); 
        }
    }, 3000); 

    connectionTimeout = setTimeout(() => {
        logMessage('Client timeout (25 seconds) reached. Closing connection.');
        stopWebSocket();
    }, 25000); 
});

ws.on('message', (data) => {
    const messageFromServer = data.toString();
    logMessage(`Received: ${messageFromServer}`);
});

ws.on('error', (error) => {
    logMessage(`WebSocket Error: ${error.message || 'Unknown WebSocket error'}`);
    cleanupResources();
});

ws.on('close', (code, reason) => {
    const reasonString = reason ? reason.toString() : 'No reason provided';
    logMessage(`WebSocket connection closed. Code: ${code}, Reason: ${reasonString}`);
    cleanupResources(); 
});

function stopWebSocket() {
    if (ws && ws.readyState === WebSocket.OPEN) {
        ws.close(1000, `${CLIENT_ID} is closing connection gracefully after timeout.`); 
    }
    cleanupResources();
}

function cleanupResources() {
    if (messageInterval) {
        clearInterval(messageInterval);
        messageInterval = null;
        logMessage('Cleared message interval.');
    }
    if (connectionTimeout) {
        clearTimeout(connectionTimeout);
        connectionTimeout = null;
        logMessage('Cleared connection timeout.');
    }
}

process.on('SIGINT', () => {
    logMessage('SIGINT signal received. Closing WebSocket connection...');
    stopWebSocket();
    setTimeout(() => process.exit(0), 500);
});

logMessage(`[Client 10-02] Setup complete. Waiting for connection...`);