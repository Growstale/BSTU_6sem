// Протокол WebSocket определяет специальные управляющие фреймы ping и pong
const WebSocket = require('ws');

const PORT = 4000;
const MESSAGE_INTERVAL = 15 * 1000;
const PING_INTERVAL = 5 * 1000; 

const wss = new WebSocket.Server({ port: PORT });

const clients = new Set();

let messageCounter = 0; 
let messageIntervalId = null; 
let pingIntervalId = null; 

console.log(`WebSocket server started on ws://localhost:${PORT}`);

function broadcastMessage() {
    messageCounter++;
    const message = `11-03-server: ${messageCounter}`;
    console.log(`Broadcasting message: "${message}" to ${clients.size} clients.`);

    clients.forEach(client => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(message, (err) => {
                if (err) {
                    console.error(`Error sending message to a client: ${err.message}`);
                }
            });
        }
    });

    if (clients.size === 0 && messageIntervalId) {
        console.log('No clients connected. Pausing message broadcast.');
        clearInterval(messageIntervalId);
        messageIntervalId = null; 
    }
}

function checkClientActivity() {
    let aliveCount = 0;
    console.log('Checking client activity...');

    clients.forEach(client => {
        if (!client.isAlive) {
            console.log('Client did not respond to ping. Terminating connection.');
            // Принудительно закрываем соединение
            client.terminate(); 
            return; // Переходим к следующему клиенту
        }
        // Перед отправкой нового пинга мы сбрасываем его в 'false'
        client.isAlive = false;
        client.ping((err) => { 
             if (err) {
                console.error(`Error sending ping to a client: ${err.message}`);
                client.terminate();
             }
        });
        aliveCount++;
    });

    console.log(`Active connections (responded to last ping): ${aliveCount}`);

     if (clients.size === 0 && pingIntervalId) {
        console.log('No clients connected. Pausing activity check.');
        clearInterval(pingIntervalId);
        pingIntervalId = null;
     }
}


wss.on('connection', (ws) => {
    console.log('Client connected');

    clients.add(ws);

    ws.isAlive = true;

    ws.on('pong', () => {
        ws.isAlive = true;
    });

    ws.on('close', (code, reason) => {
        const reasonString = reason ? reason.toString() : 'N/A';
        console.log(`Client disconnected. Code: ${code}, Reason: ${reasonString}`);
        clients.delete(ws);
        console.log(`Remaining clients: ${clients.size}`);

         if (clients.size === 0) {
             if (messageIntervalId) {
                 console.log('Last client disconnected. Stopping message broadcast.');
                 clearInterval(messageIntervalId);
                 messageIntervalId = null;
             }
             if (pingIntervalId) {
                console.log('Last client disconnected. Stopping activity check.');
                 clearInterval(pingIntervalId);
                 pingIntervalId = null;
             }
         }
    });

    ws.on('error', (error) => {
        console.error('WebSocket error on client connection:', error);
        clients.delete(ws);
         if (clients.size === 0) {
            if (messageIntervalId) clearInterval(messageIntervalId); messageIntervalId = null;
            if (pingIntervalId) clearInterval(pingIntervalId); pingIntervalId = null;
         }
    });

    // --- Запуск интервалов, если они еще не запущены (при подключении первого клиента) ---
    if (!messageIntervalId && clients.size > 0) {
        console.log('First client connected. Starting message broadcast.');
        broadcastMessage();
        messageIntervalId = setInterval(broadcastMessage, MESSAGE_INTERVAL);
    }
    if (!pingIntervalId && clients.size > 0) {
        console.log('First client connected. Starting activity check.');
        pingIntervalId = setInterval(checkClientActivity, PING_INTERVAL);
    }


});

wss.on('error', (error) => {
    console.error('WebSocket Server Error:', error);
    if (error.code === 'EADDRINUSE') {
        console.error(`Port ${PORT} is already in use.`);
    }
     if (messageIntervalId) clearInterval(messageIntervalId);
     if (pingIntervalId) clearInterval(pingIntervalId);
});

console.log('Server setup complete. Waiting for connections...');