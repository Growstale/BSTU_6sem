const WebSocket = require('ws');

const PORT = 4000;

const wss = new WebSocket.Server({ port: PORT });

let messageCounter = 0; // Глобальный счетчик сообщений для всех клиентов

console.log(`WebSocket server started on ws://localhost:${PORT}`);

wss.on('connection', (ws, req) => {
    const clientIp = req.socket.remoteAddress;
    console.log(`Client connected from ${clientIp}`);

    ws.on('message', (message) => {
        let clientName = 'unknown'; 
        let originalTimestamp = null;

        try {
            const receivedData = JSON.parse(message.toString());
            console.log('Received raw data:', receivedData);

            if (receivedData && receivedData.client && receivedData.timestamp) {
                clientName = receivedData.client;
                originalTimestamp = receivedData.timestamp;

                messageCounter++;

                const responseData = {
                    server: messageCounter,
                    client: clientName,
                    timestamp: originalTimestamp 
                };

                const responseJson = JSON.stringify(responseData);
                console.log(`Sending response to ${clientName}:`, responseJson);
                ws.send(responseJson, (err) => {
                    if (err) {
                        console.error(`Error sending response to ${clientName}:`, err);
                    }
                });

            } else {
                console.error('Invalid message format received:', receivedData);
                ws.send(JSON.stringify({ error: 'Invalid message format. Expected {client: x, timestamp: t}.' }));
            }

        } catch (e) {
            if (e instanceof SyntaxError) {
                console.error('Received non-JSON message or invalid JSON:', message.toString());
                ws.send(JSON.stringify({ error: 'Invalid JSON format received.' }));
            } else {
                // Другие возможные ошибки при обработке
                console.error('Error processing message:', e);
                ws.send(JSON.stringify({ error: 'Internal server error.' }));
            }
        }
    });

    ws.on('close', (code, reason) => {
        const reasonString = reason ? reason.toString() : 'N/A';
        console.log(`Client disconnected. Code: ${code}, Reason: ${reasonString}`);
    });

    ws.on('error', (error) => {
        console.error('WebSocket client error:', error);
    });
});

wss.on('error', (error) => {
    console.error('WebSocket Server Error:', error);
    if (error.code === 'EADDRINUSE') {
        console.error(`Port ${PORT} is already in use.`);
    }
});

console.log('Server setup complete. Waiting for connections...');