const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');

const SERVER_URL = 'ws://localhost:4000';
const FILE_TO_SEND = path.join(__dirname, 'sample.txt'); 

if (!fs.existsSync(FILE_TO_SEND)) {
    console.error(`Error: File not found at ${FILE_TO_SEND}`);
    process.exit(1); 
}

const ws = new WebSocket(SERVER_URL);

ws.on('open', () => {
    console.log(`Connected to server: ${SERVER_URL}`);
    console.log(`Sending file: ${FILE_TO_SEND}`);

    // 1. Отправляем метаданные
    const filename = path.basename(FILE_TO_SEND);
    const metadata = JSON.stringify({
        type: 'metadata',
        filename: filename
    });
    console.log('Sending metadata:', metadata);
    ws.send(metadata);
});

ws.on('message', (message) => {
    try {
        const response = JSON.parse(message.toString());
        console.log('Received from server:', response);

        if (response.type === 'ack_metadata') {
            console.log('Server acknowledged metadata. Starting file transmission...');

            // 2. Читаем и отправляем файл по частям (chunking)
            const readStream = fs.createReadStream(FILE_TO_SEND);

            readStream.on('data', (chunk) => {
                // Отправляем каждый чанк как бинарные данные
                 if (ws.readyState === WebSocket.OPEN) {
                    console.log(`Sending chunk: ${chunk.length} bytes`);
                    ws.send(chunk, (err) => {
                        if (err) {
                            console.error('Error sending chunk:', err);
                        }
                    });
                 } else {
                     console.warn('WebSocket is not open. Cannot send chunk.');
                     readStream.destroy();
                 }
            });

            readStream.on('end', () => {
                console.log('File read complete. Sending EOF marker.');
                // Отправляем маркер конца файла (EOF) после отправки всех данных
                 if (ws.readyState === WebSocket.OPEN) {
                    ws.send('EOF', (err) => {
                         if(err) {
                            console.error('Error sending EOF:', err);
                         } else {
                            console.log('EOF marker sent.');
                         }
                    });
                 } else {
                    console.warn('WebSocket is not open. Cannot send EOF.');
                 }
            });

            readStream.on('error', (err) => {
                console.error('Error reading file:', err);
                ws.close();
            });

        } else if (response.type === 'complete') {
            console.log('Server confirmed file upload complete.');
            ws.close(); 
        } else if (response.type === 'error') {
            console.error('Server reported an error:', response.message);
            ws.close(); 
        }

    } catch (e) {
        console.error('Error processing server message:', e);
         console.log('Raw message from server:', message.toString());
        ws.close();
    }
});

ws.on('close', (code, reason) => {
    console.log(`Disconnected from server. Code: ${code}, Reason: ${reason ? reason.toString() : 'N/A'}`);
});

ws.on('error', (error) => {
    console.error('WebSocket connection error:', error.message);
    if (error.code === 'ECONNREFUSED') {
        console.error(`Could not connect to ${SERVER_URL}. Is the server running?`);
    }
});