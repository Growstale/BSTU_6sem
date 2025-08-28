const http = require('http');
const fs = require('fs');
const path = require('path');
const WebSocket = require('ws'); // npm install ws

const HTTP_PORT = 3000;
const WS_PORT = 4000;

// --- HTTP Server ---
const httpServer = http.createServer((req, res) => {
    if (req.method === 'GET' && req.url === '/start') {
        const filePath = path.join(__dirname, 'index.html');
        fs.readFile(filePath, (err, data) => {
            if (err) {
                console.error('Error reading index.html:', err);
                res.writeHead(500, { 'Content-Type': 'text/plain' });
                res.end('Internal Server Error');
                return;
            }
            res.writeHead(200, { 'Content-Type': 'text/html' });
            res.end(data);
        });
    } else {
        res.writeHead(400, { 'Content-Type': 'text/plain' });
        res.end('Bad Request');
    }
});

httpServer.listen(HTTP_PORT, () => {
    console.log(`HTTP server listening on port ${HTTP_PORT}`);
    console.log(`Visit http://localhost:${HTTP_PORT}/start`);
});

// --- WebSocket Server ---
const wss = new WebSocket.Server({ port: WS_PORT });

console.log(`WebSocket server listening on port ${WS_PORT}`);

wss.on('connection', (ws) => {
    console.log('Client connected');

    let lastClientMessageNum = 0; // Храним номер последнего сообщения от этого клиента
    let serverMessageCounter = 0; // Счетчик сообщений сервера для этого клиента
    let serverInterval = null;

    // Прием сообщений от клиента
    ws.on('message', (message) => {
        const messageText = message.toString();
        console.log(`Received from client: ${messageText}`);

        const match = messageText.match(/(\d+)$/);
        if (match && match[1]) {
            lastClientMessageNum = parseInt(match[1], 10);
        }
    });

    // Отправка сообщений клиенту каждые 5 секунд
    serverInterval = setInterval(() => {
        serverMessageCounter++;
        const serverMessage = `10-01-server: ${lastClientMessageNum}->${serverMessageCounter}`;
        if (ws.readyState === WebSocket.OPEN) {
            console.log(`Sending to client: ${serverMessage}`);
            ws.send(serverMessage);
        } else {
            console.log('Client connection closed, stopping interval for this client.');
            clearInterval(serverInterval); 
        }
    }, 5000);

    ws.on('close', () => {
        console.log('Client disconnected');
        clearInterval(serverInterval); 
    });

    ws.on('error', (error) => {
        console.error('WebSocket error:', error);
        clearInterval(serverInterval); 
    });
});

wss.on('error', (error) => {
    console.error('WebSocket Server Error:', error);
});