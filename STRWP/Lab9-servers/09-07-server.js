const http = require('http');
const fs = require('fs');
const path = require('path');

const PORT = 3001; 
const UPLOAD_DIR = path.join(__dirname, 'raw_uploads');
const SAVE_FILENAME = 'uploaded_image.png'; 

if (!fs.existsSync(UPLOAD_DIR)) {
    try {
        fs.mkdirSync(UPLOAD_DIR);
        console.log(`Папка для загрузок создана: ${UPLOAD_DIR}`);
    } catch (err) {
        console.error('Не удалось создать папку для загрузок:', err);
        process.exit(1);
    }
}

const server = http.createServer((req, res) => {
    console.log(`Получен запрос: ${req.method} ${req.url}`);

    if (req.method === 'POST' && req.url === '/upload_raw') {
        const filePath = path.join(UPLOAD_DIR, SAVE_FILENAME);
        const writeStream = fs.createWriteStream(filePath);

        let totalBytes = 0;

        // Читаем поток данных из запроса и пишем в файл
        req.on('data', chunk => {
            totalBytes += chunk.length;
            console.log(`Получено ${chunk.length} байт, всего ${totalBytes}`);
            writeStream.write(chunk); // Пишем чанк в файл
        });

        req.on('end', () => {
            writeStream.end(() => { 
                console.log(`Файл успешно сохранен как "${SAVE_FILENAME}" (${totalBytes} байт)`);
                res.writeHead(200, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify({
                    message: 'Файл успешно получен и сохранен (raw)',
                    filename: SAVE_FILENAME,
                    size: totalBytes
                }));
            });
        });

        req.on('error', (err) => {
            console.error('Ошибка чтения потока запроса:', err);
            writeStream.end(); // Закрываем поток записи при ошибке
            // Пытаемся удалить частично записанный файл
            fs.unlink(filePath, (unlinkErr) => {
                if (unlinkErr) console.error('Ошибка удаления частичного файла:', unlinkErr);
            });
            res.writeHead(500, { 'Content-Type': 'text/plain' });
            res.end('Ошибка сервера при приеме файла');
        });

        writeStream.on('error', (err) => {
            console.error('Ошибка записи файла:', err);
            req.destroy();
            res.writeHead(500, { 'Content-Type': 'text/plain' });
            res.end('Ошибка сервера при записи файла');
        });

    } else {
        res.writeHead(404, { 'Content-Type': 'text/plain' });
        res.end('Ресурс не найден. Используйте POST /upload_raw для загрузки файла.');
    }
});

server.listen(PORT, () => {
    console.log(`Сервер для приема "сырых" файлов запущен на http://localhost:${PORT}`);
    console.log(`Ожидает POST запросы на http://localhost:${PORT}/upload_raw`);
});