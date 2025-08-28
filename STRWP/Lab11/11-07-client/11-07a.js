const WebSocket = require('ws');

const SERVER_URL = 'ws://localhost:4000';
const RECONNECT_INTERVAL = 5000;
const PROMPT = 'Enter notification (A, B, C) or EXIT > ';

let ws = null;
// Флаг, чтобы не добавлять обработчик stdin повторно
let isStdinListenerAttached = false; 
let inputBuffer = ''; 

function sendNotification(eventName) {
    if (ws && ws.readyState === WebSocket.OPEN) {
        const message = JSON.stringify({
            type: 'notification',
            event: eventName
        });
        console.log(`[Client] Sending notification: ${message}`);
        ws.send(message, (err) => {
            if (err) {
                console.error(`[Client] Error sending notification: ${err.message}`);
            }
        });
    } else {
        console.error('[Client] Cannot send notification: WebSocket connection is not open.');
    }
}

// --- Функция для обработки одной строки ввода ---
function processLineInput(line) {
    const input = line.trim().toUpperCase();

    switch (input) {
        case 'A':
        case 'B':
        case 'C':
            sendNotification(input);
            break;
        case 'EXIT':
            console.log('\nExiting...');
            if (ws) ws.close();
            process.exit(0); 
        default:
            console.log(`Invalid input: "${line.trim()}". Please enter A, B, C, or EXIT.`);
            break;
    }
    // Приглашение
    process.stdout.write(PROMPT);
}

// --- Настройка обработки ввода из process.stdin ---
function setupStdinProcessing() {
    if (isStdinListenerAttached) {
        process.stdout.write(PROMPT);
        return;
    }

    console.log('[Client] Setting up console input...');
    process.stdin.setEncoding('utf8');
    process.stdin.resume();

    process.stdin.on('data', (chunk) => {
        inputBuffer += chunk;
        let newlineIndex;
        while ((newlineIndex = inputBuffer.indexOf('\n')) >= 0) {
            const line = inputBuffer.substring(0, newlineIndex);
            inputBuffer = inputBuffer.substring(newlineIndex + 1);
            processLineInput(line);
        }
    });

    isStdinListenerAttached = true; 
    process.stdout.write(PROMPT);  // Показываем первый промпт
}

process.on('SIGINT', () => {
    console.log('\n[Client] SIGINT (Ctrl+C) received. Disconnecting and exiting...');
    if (ws && ws.readyState === WebSocket.OPEN) {
        ws.close();
    }
    setTimeout(() => process.exit(0), 100);
});


function connect() {
    console.log(`[Client] Attempting to connect to ${SERVER_URL}...`);
    ws = new WebSocket(SERVER_URL);

    ws.on('open', () => {
        console.log(`[Client] Connected to server ${SERVER_URL}`);
        setupStdinProcessing();
    });

    ws.on('message', (message) => {
        process.stdout.write('\n');
        console.log('[Client] Received unexpected message from server:', message.toString());
        if (isStdinListenerAttached) {
            process.stdout.write(PROMPT);
        }
    });

    ws.on('close', (code, reason) => {
        const reasonString = reason ? reason.toString() : 'N/A';
        process.stdout.write('\n');
        console.log(`[Client] Disconnected from server. Code: ${code}, Reason: ${reasonString}`);
        ws = null;
        console.log(`[Client] Reconnecting in ${RECONNECT_INTERVAL / 1000} seconds...`);
        setTimeout(connect, RECONNECT_INTERVAL);
    });

    ws.on('error', (error) => {
        process.stdout.write('\n');
        console.error(`[Client] WebSocket error: ${error.message}`);
        if (ws && ws.readyState !== WebSocket.CLOSED && ws.readyState !== WebSocket.CLOSING) {
             ws.close();
        } else {
            ws = null;
            console.log(`[Client] Retrying connection in ${RECONNECT_INTERVAL / 1000} seconds...`);
            setTimeout(connect, RECONNECT_INTERVAL);
        }
    });
}

connect();

console.log('[Client] Started. Waiting for connection...');