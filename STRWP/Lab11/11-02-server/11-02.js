const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');

const PORT = 4000;
const DOWNLOAD_DIR = path.join(__dirname, 'download');

if (!fs.existsSync(DOWNLOAD_DIR)) {
    console.error(`Error: Download directory not found at ${DOWNLOAD_DIR}`);
    console.log('Please create the "download" directory and place files in it.');
    process.exit(1);
} else {
    console.log(`Serving files from directory: ${DOWNLOAD_DIR}`);
}

const wss = new WebSocket.Server({ port: PORT });

console.log(`WebSocket server started on ws://localhost:${PORT}`);

wss.on('connection', (ws) => {
    console.log('Client connected');

    let activeStream = null; // Для отслеживания активного потока чтения для этого клиента

    ws.on('message', async (message) => {
        try {
            const request = JSON.parse(message.toString());

            if (request.type === 'request_file' && request.filename) {
                console.log(`Client requested file: ${request.filename}`);

                const requestedFilename = path.basename(request.filename); // Убираем любые части пути
                const filePath = path.join(DOWNLOAD_DIR, requestedFilename);

                if (fs.existsSync(filePath) && filePath.startsWith(DOWNLOAD_DIR)) {
                    console.log(`File found: ${filePath}`);

                    // Получаем размер файла для метаданных
                    const stats = await fs.promises.stat(filePath);
                    const fileSize = stats.size;

                    // 4. Отправляем метаданные файла клиенту
                    const metadata = JSON.stringify({
                        type: 'file_metadata',
                        filename: requestedFilename,
                        size: fileSize
                    });
                    console.log('Sending metadata:', metadata);
                    ws.send(metadata);

                    // 5. Создаем поток чтения и отправляем файл по частям
                    activeStream = fs.createReadStream(filePath);

                    activeStream.on('data', (chunk) => {
                        // Отправляем чанк как бинарные данные
                        if (ws.readyState === WebSocket.OPEN) {
                            console.log(`Sending chunk: ${chunk.length} bytes`);
                             ws.send(chunk, (err) => {
                                if (err) {
                                    console.error('Error sending chunk:', err);
                                    if (activeStream) activeStream.destroy();
                                    activeStream = null;
                                }
                             });
                        } else {
                            console.warn('Client disconnected during transfer. Stopping stream.');
                            if (activeStream) activeStream.destroy();
                            activeStream = null;
                        }
                    });

                    activeStream.on('end', () => {
                        console.log(`File ${requestedFilename} sending complete.`);
                        // Отправляем маркер конца файла
                         if (ws.readyState === WebSocket.OPEN) {
                            ws.send(JSON.stringify({ type: 'file_end' }));
                            console.log('Sent file_end marker.');
                         }
                        activeStream = null; // Поток завершен
                    });

                    activeStream.on('error', (err) => {
                        console.error('Error reading file:', err);
                         if (ws.readyState === WebSocket.OPEN) {
                             ws.send(JSON.stringify({ type: 'error', message: 'Error reading file on server.' }));
                         }
                         activeStream = null;
                    });

                } else {
                    console.log(`File not found or access denied: ${requestedFilename}`);
                    ws.send(JSON.stringify({ type: 'error', message: `File '${requestedFilename}' not found.` }));
                }
            } else {
                 console.warn('Invalid request format from client:', request);
                 ws.send(JSON.stringify({ type: 'error', message: 'Invalid request format.' }));
            }

        } catch (e) {
             if (e instanceof SyntaxError) {
                console.error('Error parsing client request JSON:', e.message);
                ws.send(JSON.stringify({ type: 'error', message: 'Invalid JSON request.' }));
             } else {
                console.error('Error processing client request:', e);
                ws.send(JSON.stringify({ type: 'error', message: 'Internal server error.' }));
             }
        }
    });

    ws.on('close', () => {
        console.log('Client disconnected');
        if (activeStream) {
            console.log('Stopping active file stream due to client disconnection.');
            activeStream.destroy();
            activeStream = null;
        }
    });

    ws.on('error', (error) => {
        console.error('WebSocket error:', error);
        if (activeStream) {
            activeStream.destroy();
            activeStream = null;
        }
    });
});

wss.on('error', (error) => {
    console.error('WebSocket Server Error:', error);
    if (error.code === 'EADDRINUSE') {
        console.error(`Port ${PORT} is already in use.`);
    }
});