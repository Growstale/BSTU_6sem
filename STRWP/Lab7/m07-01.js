const fs = require('fs');
const path = require('path');

const mimeTypes = {
    '.html': 'text/html',
    '.css': 'text/css',
    '.js': 'text/javascript',
    '.png': 'image/png',
    '.docx': 'application/msword',
    '.xml': 'application/xml',
    '.mp4': 'video/mp4'
};

// Параметр - корневая директория статических файлов
module.exports = (staticBasePath) => {

    // Возвращаем сам обработчик запросов 
    return (req, res) => {

        if (req.method !== 'GET') {
            res.writeHead(405, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end('Метод не разрешен (405)');
            return;
        }

        try {
            let urlPath = req.url;

            if (urlPath === '/') {
                urlPath = '/index.html';
            }

            // Формируем полный путь к файлу на сервере
            const filePath = path.normalize(path.join(staticBasePath, urlPath));

            if (!filePath.startsWith(staticBasePath)) {
                console.error(`Попытка доступа за пределы static: ${filePath}`);
                res.writeHead(403, { 'Content-Type': 'text/plain; charset=utf-8' });
                res.end('Доступ запрещен');
                return;
            }

            // Проверка существования файла
            fs.stat(filePath, (err, stats) => {
                if (err || !stats.isFile()) {
                    console.error(`Файл не найден: ${filePath}`);
                    res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.end('Ресурс не найден');
                    return;
                }

                // Определяем MIME тип по расширению файла
                const ext = path.extname(filePath).toLowerCase();
                const mimeType = mimeTypes[ext];

                if (!mimeType) {
                    console.error(`Неподдерживаемый тип файла: ${filePath}`);
                     res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' }); 
                     res.end('Ресурс не найден - неподдерживаемый тип');
                    return;
                }

                fs.readFile(filePath, (readErr, data) => {
                    if (readErr) {
                        console.error(`Ошибка чтения файла: ${filePath}`, readErr);
                        if (!res.headersSent) {
                            res.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
                        }
                        res.end('Внутренняя ошибка сервера - ошибка чтения файла');
                        return;
                    }
                    console.log(`Отправка файла (readFile): ${filePath} (MIME: ${mimeType})`);
                    res.writeHead(200, { 'Content-Type': mimeType });
                    res.end(data);
                });

            });

        } catch (e) {
            console.error(`Внутренняя ошибка сервера при обработке запроса ${req.url}:`, e);
             if (!res.headersSent) {
                 res.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
             }
             res.end('Внутренняя ошибка сервера');
        }
    };
};