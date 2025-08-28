const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');

const PORT = 4000;
const UPLOAD_DIR = path.join(__dirname, 'upload');

if (!fs.existsSync(UPLOAD_DIR)) {
    console.log(`Creating upload directory: ${UPLOAD_DIR}`);
    fs.mkdirSync(UPLOAD_DIR, { recursive: true });
} else {
    console.log(`Upload directory already exists: ${UPLOAD_DIR}`);
}

const wss = new WebSocket.Server({ port: PORT });

console.log(`WebSocket server started on ws://localhost:${PORT}`);

wss.on('connection', (ws) => {
    console.log('Client connected');

    let fileInfo = null; // метаданные
    let fileStream = null; // поток записи

    ws.on('message', (message) => {
        try {
            if (!fileInfo && !fileStream) {
                const metadata = JSON.parse(message.toString());
                if (metadata.type === 'metadata' && metadata.filename) {
                    fileInfo = metadata;
                    const sanitizedFilename = path.basename(fileInfo.filename);
                    const filePath = path.join(UPLOAD_DIR, sanitizedFilename); // формируем путь

                    console.log(`Receiving metadata: ${JSON.stringify(fileInfo)}`);
                    console.log(`Preparing to write file: ${filePath}`);

                    // Создаем WriteStream для записи файла
                    fileStream = fs.createWriteStream(filePath);

                    fileStream.on('error', (err) => {
                        console.error('File stream error:', err);
                        ws.send(JSON.stringify({ type: 'error', message: 'Failed to save file on server.' }));
                        ws.close();
                    });

                    // Сработает, когда все данные будут успешно записаны в файл
                     fileStream.on('finish', () => {
                        console.log(`File ${sanitizedFilename} saved successfully.`);
                        ws.send(JSON.stringify({ type: 'complete', message: `File ${sanitizedFilename} uploaded successfully.` }));
                        fileInfo = null;
                        fileStream = null;
                    });


                    ws.send(JSON.stringify({ type: 'ack_metadata', message: 'Metadata received, ready for file data.' }));
                } else {
                    console.error('Invalid initial message format. Expected file metadata.');
                    ws.send(JSON.stringify({ type: 'error', message: 'Invalid metadata format.' }));
                    ws.close(); 
                }
            }
            // Если метаданные получены, последующие сообщения - это бинарные данные файла
            else if (fileInfo && fileStream) {
                // Проверяем, не является ли сообщение сигналом конца передачи
                if (message.toString() === 'EOF') {
                     console.log('Received EOF marker.');
                     fileStream.end(() => {
                         console.log('File stream closed after EOF.');
                     });
                } else if (Buffer.isBuffer(message)) {
                    // Записываем бинарные данные в файл
                    console.log(`Received chunk of size: ${message.length} bytes`);
                    const canWrite = fileStream.write(message);
                    if (!canWrite) {
                        console.log('Stream paused due to backpressure.');
                        ws.pause();
                        // Событие 'drain' сработает, когда буфер потока записи освободится и он снова будет готов принимать данные
                        fileStream.once('drain', () => {
                            console.log('Stream drained, resuming.');
                            ws.resume(); // Возобновление приема данных
                        });
                    }
                } else {
                     console.warn('Received unexpected non-buffer data after metadata.');
                }
            } else {
                 console.error('Received data before metadata or after file completion.');
                 ws.send(JSON.stringify({ type: 'error', message: 'Protocol error: Data received unexpectedly.' }));
                 ws.close();
            }
        } catch (e) {
            if (e instanceof SyntaxError) {
                console.error('Error parsing JSON metadata:', e.message);
                 ws.send(JSON.stringify({ type: 'error', message: 'Invalid JSON metadata received.' }));
            } else {
                console.error('Error processing message:', e);
                 ws.send(JSON.stringify({ type: 'error', message: 'Internal server error.' }));
            }
             if (fileStream) fileStream.destroy();
             ws.close();
        }
    });

    ws.on('close', () => {
        console.log('Client disconnected');
        if (fileStream && !fileStream.closed) {
            console.log('Client disconnected during transfer, closing file stream.');
            fileStream.end(); // Завершаем запись
            fileStream = null;
        }
        fileInfo = null;
    });

    ws.on('error', (error) => {
        console.error('WebSocket error:', error);
        if (fileStream) {
            fileStream.destroy(); // Уничтожаем поток при ошибке сокета
            fileStream = null;
        }
         fileInfo = null;
    });
});

wss.on('error', (error) => {
    console.error('WebSocket Server Error:', error);
    if (error.code === 'EADDRINUSE') {
        console.error(`Port ${PORT} is already in use.`);
    }
});