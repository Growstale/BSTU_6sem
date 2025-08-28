const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');

const SERVER_URL = 'ws://localhost:4000';
const FILE_TO_REQUEST = 'example.txt';
const SAVE_DIR = path.join(__dirname, 'client_uploads'); 

if (!fs.existsSync(SAVE_DIR)) {
    console.log(`Creating save directory: ${SAVE_DIR}`);
    fs.mkdirSync(SAVE_DIR, { recursive: true });
} else {
    console.log(`Save directory exists: ${SAVE_DIR}`);
}

const ws = new WebSocket(SERVER_URL);

// Переменные для отслеживания состояния загрузки
let currentDownload = {
    writeStream: null,
    filename: null,
    filePath: null,
    bytesReceived: 0,
    totalSize: 0
};

ws.on('open', () => {
    console.log(`Connected to server: ${SERVER_URL}`);

    // 2. Отправляем запрос на файл
    const request = JSON.stringify({
        type: 'request_file',
        filename: FILE_TO_REQUEST
    });
    console.log(`Requesting file: ${FILE_TO_REQUEST}`);
    ws.send(request);
});

ws.on('message', (message) => {
    try {
        const data = JSON.parse(message.toString());
        console.log('Received JSON from server:', data);

        if (data.type === 'file_metadata' && data.filename) {
            // Получили метаданные - начинаем подготовку к записи
            if (currentDownload.writeStream) {
                console.warn('Received new metadata while a download is in progress. Aborting previous.');
                currentDownload.writeStream.destroy();
            }

            currentDownload.filename = path.basename(data.filename);
            currentDownload.filePath = path.join(SAVE_DIR, currentDownload.filename);
            currentDownload.totalSize = data.size || 0;
            currentDownload.bytesReceived = 0;

            console.log(`Preparing to save file: ${currentDownload.filePath} (${currentDownload.totalSize} bytes)`);
            currentDownload.writeStream = fs.createWriteStream(currentDownload.filePath);

            currentDownload.writeStream.on('error', (err) => {
                console.error('Error writing file:', err);
                ws.close(); 
                if (currentDownload.filePath && fs.existsSync(currentDownload.filePath)) {
                    fs.unlink(currentDownload.filePath, (unlinkErr) => {
                         if (unlinkErr) console.error('Error deleting partial file:', unlinkErr);
                    });
                }
                resetDownloadState();
            });

             currentDownload.writeStream.on('finish', () => {
                 console.log(`File ${currentDownload.filename} stream finished writing.`);
                 // Проверка размера файла после закрытия потока
                 if (currentDownload.totalSize > 0 && currentDownload.bytesReceived !== currentDownload.totalSize) {
                    console.warn(`Warning: Final file size (${currentDownload.bytesReceived}) does not match expected size (${currentDownload.totalSize}).`);
                 }
             });


        } else if (data.type === 'file_end') {
            // Сервер сигнализирует о конце передачи файла
            if (currentDownload.writeStream) {
                console.log('Received file_end marker. Finalizing download.');
                // Завершаем поток записи, чтобы убедиться, что все данные записаны на диск.
                 currentDownload.writeStream.end(() => {
                    console.log(`File ${currentDownload.filename} saved successfully to ${currentDownload.filePath}.`);
                     resetDownloadState();
                 });

            } else {
                console.warn('Received file_end marker but no download was in progress.');
            }
        } else if (data.type === 'error') {
            console.error('Server error:', data.message);
             if (currentDownload.writeStream) {
                 currentDownload.writeStream.destroy();
                  if (currentDownload.filePath && fs.existsSync(currentDownload.filePath)) {
                    fs.unlink(currentDownload.filePath, (unlinkErr) => {
                        if (unlinkErr) console.error('Error deleting partial file after server error:', unlinkErr);
                    });
                 }
             }
             resetDownloadState();
            ws.close(); 
        } else {
             console.warn('Received unknown JSON message type:', data.type);
        }

    } catch (e) {
        // сервер прислал бинарный чанк файла
        if (e instanceof SyntaxError && currentDownload.writeStream && Buffer.isBuffer(message)) {
             console.log(`Received chunk: ${message.length} bytes`);
             currentDownload.bytesReceived += message.length;
             const canWrite = currentDownload.writeStream.write(message);
             if (!canWrite) {
                 console.log('Client write stream paused (backpressure).');
             }

        } else if (currentDownload.writeStream && !Buffer.isBuffer(message)) {
             console.warn('Received non-buffer data during file transfer:', message.toString());
        }
        else {
            console.error('Error processing server message or unexpected data:', e);
        }
    }
});

ws.on('close', (code, reason) => {
    console.log(`Disconnected from server. Code: ${code}, Reason: ${reason ? reason.toString() : 'N/A'}`);
    if (currentDownload.writeStream && !currentDownload.writeStream.closed) {
        console.warn('Connection closed during download. Closing file stream.');
         currentDownload.writeStream.end(); // Попытаться сохранить то, что успело прийти
    }
     resetDownloadState(); 
});

ws.on('error', (error) => {
    console.error('WebSocket connection error:', error.message);
    if (error.code === 'ECONNREFUSED') {
        console.error(`Could not connect to ${SERVER_URL}. Is the server running?`);
    }
     if (currentDownload.writeStream) {
        currentDownload.writeStream.destroy();
         if (currentDownload.filePath && fs.existsSync(currentDownload.filePath)) {
            fs.unlink(currentDownload.filePath, (unlinkErr) => {
                if (unlinkErr) console.error('Error deleting partial file after connection error:', unlinkErr);
            });
         }
     }
    resetDownloadState();
});

// Функция для сброса состояния загрузки
function resetDownloadState() {
    currentDownload.writeStream = null;
    currentDownload.filename = null;
    currentDownload.filePath = null;
    currentDownload.bytesReceived = 0;
    currentDownload.totalSize = 0;
    console.log('Download state reset.');
}