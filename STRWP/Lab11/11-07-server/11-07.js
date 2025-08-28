const WebSocket = require('ws');

const PORT = 4000;

const wss = new WebSocket.Server({ port: PORT });

console.log(`WebSocket Notification Server started on ws://localhost:${PORT}`);
console.log('Waiting for client connections and notifications (A, B, C)...');

wss.on('connection', (ws, req) => {
    const clientIp = req.socket.remoteAddress;
    console.log(`\n[Server] Client connected from ${clientIp}`);

    ws.on('message', (message) => {
        try {
            const data = JSON.parse(message.toString());
            if (data && data.type === 'notification' && typeof data.event === 'string') {
                const eventName = data.event.toUpperCase();

                if (['A', 'B', 'C'].includes(eventName)) {
                    console.log(`[Server] ===> Received notification '${eventName}' from ${clientIp} <===`);
                } else {
                    console.warn(`[Server] Received notification with unknown event type '${data.event}' from ${clientIp}`);
                }
            } else {
                console.warn(`[Server] Received message with unexpected format from ${clientIp}:`, data);
            }

        } catch (e) {
            if (e instanceof SyntaxError) {
                console.error(`[Server] Received non-JSON message from ${clientIp}: ${message.toString()}`);
            } else {
                console.error(`[Server] Error processing message from ${clientIp}:`, e);
            }
        }
    });

    ws.on('close', (code, reason) => {
        const reasonString = reason ? reason.toString() : 'N/A';
        console.log(`\n[Server] Client disconnected from ${clientIp}. Code: ${code}, Reason: ${reasonString}`);
    });

    ws.on('error', (error) => {
        console.error(`[Server] WebSocket error from ${clientIp}:`, error);
    });
});

wss.on('error', (error) => {
    console.error('[Server] WebSocket Server Error:', error);
    if (error.code === 'EADDRINUSE') {
        console.error(`[Server] Port ${PORT} is already in use.`);
    }
});