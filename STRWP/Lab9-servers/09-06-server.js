const http = require('http');
const fs = require('fs');
const path = require('path');

const PORT = 3000; 

const server = http.createServer((req, res) => {
    if (req.method === 'POST' && req.url === '/upload') {
        console.log('Получен POST-запрос на /upload');

        const filename = 'uploaded_' + Date.now() + '.txt'; 
        const savePath = path.join(__dirname, filename); // Полный путь для сохранения

        console.log(`Попытка сохранить файл как: ${savePath}`);

        // Создаем поток для записи в файл
        const writeStream = fs.createWriteStream(savePath);

        // Перенаправляем данные из запроса (req) в поток записи (writeStream)
        req.pipe(writeStream);

        // Обработка успешного завершения записи
        writeStream.on('finish', () => {
            console.log(`Файл успешно сохранен: ${savePath}`);
            res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end('Файл успешно загружен и сохранен на сервере.');
        });

        // Обработка ошибок при записи файла
        writeStream.on('error', (err) => {
            console.error('Ошибка при записи файла:', err);
            // Важно! Удалить частично записанный файл при ошибке
            fs.unlink(savePath, (unlinkErr) => {
                 if (unlinkErr) console.error('Ошибка при удалении частичного файла:', unlinkErr);
            });
            res.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end('Ошибка на сервере при сохранении файла.');
        });

         // Обработка ошибок самого запроса (например, обрыв соединения клиентом)
        req.on('error', (err) => {
            console.error('Ошибка в потоке запроса:', err);
            res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end('Ошибка в запросе клиента.');
            // Закрыть поток записи, если он еще открыт
            writeStream.close();
             // Попытаться удалить файл, если он был создан
             fs.unlink(savePath, (unlinkErr) => {
                 if (unlinkErr && unlinkErr.code !== 'ENOENT') { // Игнорировать ошибку, если файла нет
                    console.error('Ошибка при удалении файла после ошибки запроса:', unlinkErr);
                 }
            });
        });

    } else {
        res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Ресурс не найден. Используйте POST /upload для загрузки файла.');
    }
});

server.listen(PORT, () => {
    console.log(`Сервер запущен и слушает порт ${PORT}`);
    console.log(`Ожидание POST-запросов на http://localhost:${PORT}/upload`);
});