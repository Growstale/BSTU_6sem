const http = require('http');
const fs = require('fs');
const path = require('path');
const url = require('url'); 

const PORT = 3002; 
const FILE_TO_SEND = path.join(__dirname, 'MyFile.png'); // Файл, который будем отправлять
const DOWNLOAD_FILENAME = 'DownloadedPicture.png'; // Имя, которое предложим клиенту для сохранения

const server = http.createServer((req, res) => {
    const parsedUrl = url.parse(req.url);
    console.log(`Получен запрос: ${req.method} ${parsedUrl.pathname}`);

    if (req.method === 'GET' && parsedUrl.pathname === '/download') {
        console.log(`Запрос на скачивание файла: ${path.basename(FILE_TO_SEND)}`);

        // Проверяем, существует ли файл
        fs.stat(FILE_TO_SEND, (err, stats) => {
            if (err) {
                if (err.code === 'ENOENT') {
                    console.error(`Ошибка: Файл не найден - ${FILE_TO_SEND}`);
                    res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.end('Ошибка 404: Файл не найден на сервере.');
                } else {
                    console.error(`Ошибка доступа к файлу ${FILE_TO_SEND}:`, err);
                    res.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.end('Ошибка 500: Внутренняя ошибка сервера при доступе к файлу.');
                }
                return;
            }

            console.log(`Файл найден. Размер: ${stats.size} байт.`);
            res.writeHead(200, {
                'Content-Type': 'image/png', // Указываем MIME-тип файла
                'Content-Length': stats.size, // Указываем размер файла
                'Content-Disposition': `attachment; filename="${DOWNLOAD_FILENAME}"`
            });

            // Создаем поток чтения файла
            const readStream = fs.createReadStream(FILE_TO_SEND);

            readStream.on('error', (streamErr) => {
                console.error('Ошибка чтения потока файла:', streamErr);
                if (!res.writableEnded) {
                    res.end(); 
                }
            });

            // Перенаправляем данные из файла в поток ответа
            console.log(`Начало отправки файла "${DOWNLOAD_FILENAME}" клиенту...`);
            readStream.pipe(res);

            res.on('finish', () => {
                 console.log(`Отправка файла "${DOWNLOAD_FILENAME}" клиенту завершена.`);
            });
            req.on('close', () => {
                console.log('Клиент закрыл соединение до завершения скачивания.');
                readStream.destroy(); 
            });


        });

    } else {
        res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Ресурс не найден. Используйте GET /download для скачивания файла.');
    }
});

server.listen(PORT, () => {
    console.log(`Сервер для отдачи файлов запущен на http://localhost:${PORT}`);
    console.log(`Запросите файл по адресу: http://localhost:${PORT}/download`);
    if (!fs.existsSync(FILE_TO_SEND)) {
         console.warn(`ВНИМАНИЕ: Файл для отправки (${FILE_TO_SEND}) не найден! Сервер не сможет его отдать.`);
    }
});