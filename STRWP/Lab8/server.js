const http = require('http');
const url = require('url');
const fs = require('fs');
const path = require('path');
const querystring = require('querystring'); // Для парсинга form data
const xml2js = require('xml2js');         // Для парсинга и создания XML
const { IncomingForm } = require('formidable'); // Для парсинга multipart/form-data (файлы)

const PORT = 3000;
const STATIC_DIR = path.join(__dirname, 'static'); // Путь к папке static

const server = http.createServer((req, res) => {
    const parsedUrl = url.parse(req.url, true);
    const pathname = parsedUrl.pathname;
    const query = parsedUrl.query;
    const method = req.method;

    console.log(`${method} ${pathname}`);

    if (method === 'GET') {
        // --- Задание 01 ---
        if (pathname === '/connection') {
            if (query.set) { // /connection?set=value
                const newTimeout = parseInt(query.set, 10);
                if (!isNaN(newTimeout) && newTimeout >= 0) {
                    server.keepAliveTimeout = newTimeout;
                    res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.end(`Установлено новое значение параметра KeepAliveTimeout = ${server.keepAliveTimeout}ms`);
                } else {
                    res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.end('Ошибка: Неверное значение для параметра set');
                }
            } else { // /connection
                res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
                res.end(`Текущее значение KeepAliveTimeout = ${server.keepAliveTimeout}ms`);
            }
            // Демонстрация KeepAliveTimeout: Низкое значение (например 1000ms) закроет неактивные соединения быстрее.
            // Высокое значение (по умолчанию 5000ms или больше) держит их дольше, экономя на установке новых соединений,
            // но потребляя ресурсы сервера. Разницу можно заметить при множественных быстрых запросах с клиента,
            // поддерживающего keep-alive (большинство браузеров), особенно если между запросами есть паузы.

        // --- Задание 02 ---
        } else if (pathname === '/headers') {
            res.setHeader('X-My-Custom-Header', 'My-Custom-Header-Value'); // Установка кастомного заголовка
            res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.write('--- Заголовки Запроса (Request Headers) ---\n');
            for (const key in req.headers) {
                res.write(`${key}: ${req.headers[key]}\n`);
            }
            res.write('\n--- Заголовки Ответа (Response Headers) ---\n');
            const responseHeaders = res.getHeaders();
             for (const key in responseHeaders) {
                res.write(`${key}: ${responseHeaders[key]}\n`);
            }
            res.end();

        // --- Задание 03 ---
        } else if (pathname === '/parameter' && query.x && query.y) {
            const x = parseFloat(query.x);
            const y = parseFloat(query.y);

            if (!isNaN(x) && !isNaN(y)) {
                res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
                res.write(`Параметры: x=${x}, y=${y}\n`);
                res.write(`Сумма (x + y): ${x + y}\n`);
                res.write(`Разность (x - y): ${x - y}\n`);
                res.write(`Произведение (x * y): ${x * y}\n`);
                if (y !== 0) {
                    res.write(`Частное (x / y): ${x / y}\n`);
                } else {
                    res.write('Частное (x / y): Деление на ноль невозможно.\n');
                }
                res.end();
            } else {
                res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
                res.end('Ошибка: Параметры x и y должны быть числовыми.');
            }

        // --- Задание 04 ---
        } else if (pathname.startsWith('/parameter/')) {
            const parts = pathname.split('/'); // ['', 'parameter', 'x', 'y']
            if (parts.length === 4) {
                const xStr = parts[2];
                const yStr = parts[3];
                const x = parseFloat(xStr);
                const y = parseFloat(yStr);

                if (!isNaN(x) && !isNaN(y)) {
                    res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.write(`Параметры из пути: x=${x}, y=${y}\n`);
                    res.write(`Сумма (x + y): ${x + y}\n`);
                    res.write(`Разность (x - y): ${x - y}\n`);
                    res.write(`Произведение (x * y): ${x * y}\n`);
                    if (y !== 0) {
                        res.write(`Частное (x / y): ${x / y}\n`);
                    } else {
                        res.write('Частное (x / y): Деление на ноль невозможно.\n');
                    }
                    res.end();
                } else {
                    res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.end(`Ошибка: Нечисловые параметры в URI. Запрошенный URI: ${req.url}`);
                }
            } else {
                 res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
                 res.end(`Неверный формат URI для /parameter/x/y. Запрошенный URI: ${req.url}`);
            }

        // --- Задание 05 ---
        } else if (pathname === '/close') {
            res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end('Сервер будет остановлен через 10 секунд.');
            console.log('Получен запрос на остановку. Сервер закроется через 10 секунд.');
            setTimeout(() => {
                server.close(() => {
                    console.log('Сервер остановлен.');
                    process.exit(0); // Завершаем процесс Node.js
                });
            }, 10000); 

        // --- Задание 06 ---
        } else if (pathname === '/socket') {
            res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.write(`--- Информация о сокете ---\n`);
            res.write(`IP-адрес клиента: ${req.socket.remoteAddress}\n`);
            res.write(`Порт клиента: ${req.socket.remotePort}\n`);
            res.write(`IP-адрес сервера: ${req.socket.localAddress}\n`);
            res.write(`Порт сервера: ${req.socket.localPort}\n`);
            res.end();

        // --- Задание 08 ---
        } else if (pathname === '/resp-status' && query.code && query.mess) {
            const statusCode = parseInt(query.code, 10);
            const statusMessage = query.mess;

            if (!isNaN(statusCode) && statusCode >= 100 && statusCode < 600) {
                res.writeHead(statusCode, statusMessage, { 'Content-Type': 'text/plain; charset=utf-8' });
                res.end(`Ответ со статусом ${statusCode} и сообщением "${statusMessage}"`);
            } else {
                 res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
                 res.end('Ошибка: Неверный код статуса.');
            }

        // --- Задание 09 (HTML форма для отправки на /formparameter) ---
        } else if (pathname === '/form') { 
            res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
            res.end(`
                <!DOCTYPE html>
                <html>
                <head>
                    <title>Форма для Задания 09</title>
                    <meta charset="UTF-8">
                </head>
                <body>
                    <h1>Тестовая форма</h1>
                    <form method="POST" action="/formparameter">
                        <label for="textInput">Текстовое поле:</label>
                        <input type="text" id="textInput" name="text_field"><br><br>

                        <label for="numInput">Числовое поле:</label>
                        <input type="number" id="numInput" name="number_field"><br><br>

                        <label for="dateInput">Дата:</label>
                        <input type="date" id="dateInput" name="date_field"><br><br>

                        <label>Чекбокс:</label>
                        <input type="checkbox" id="check1" name="checkbox_field" value="checked_value_1">
                        <label for="check1">Опция 1</label>
                        <input type="checkbox" id="check2" name="checkbox_field" value="checked_value_2">
                        <label for="check2">Опция 2</label><br><br>

                        <label>Радиокнопки:</label><br>
                        <input type="radio" id="radio1" name="radio_button" value="radio_option_1">
                        <label for="radio1">Выбор 1</label><br>
                        <input type="radio" id="radio2" name="radio_button" value="radio_option_2">
                        <label for="radio2">Выбор 2</label><br>
                        <input type="radio" id="radio3" name="radio_button" value="radio_option_3">
                        <label for="radio3">Выбор 3</label><br><br>

                        <label for="textArea">Текстовая область:</label><br>
                        <textarea id="textArea" name="text_area" rows="4" cols="50"></textarea><br><br>

                        <input type="submit" name="submit_button" value="Отправить (Значение 1)">
                        <input type="submit" name="submit_button" value="Отправить (Значение 2)">
                    </form>
                </body>
                </html>
            `);

        // --- Задание 12 ---
        } else if (pathname === '/files') {
            fs.readdir(STATIC_DIR, (err, files) => {
                if (err) {
                    console.error("Ошибка чтения директории static:", err);
                    res.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.end('Ошибка сервера при чтении директории static.');
                    return;
                }
                const fileCount = files.filter(file => fs.statSync(path.join(STATIC_DIR, file)).isFile()).length; // Считаем только файлы
                res.writeHead(200, {
                    'Content-Type': 'text/plain; charset=utf-8',
                    'X-Static-Files-Count': fileCount // Устанавливаем заголовок
                });
                res.end(`Количество файлов в директории static указано в заголовке X-Static-Files-Count.`);
            });

        // --- Задание 13 ---
        } else if (pathname.startsWith('/files/')) {
            const filename = path.basename(pathname);
            const filePath = path.join(STATIC_DIR, filename);

            fs.stat(filePath, (err, stats) => {
                if (err || !stats.isFile()) {
                    res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.end(`Файл ${filename} не найден в директории static.`);
                } else {
                    res.writeHead(200);
                    const readStream = fs.createReadStream(filePath);
                    readStream.pipe(res); // Эффективно передаем файл клиенту

                    readStream.on('error', (streamErr) => {
                        console.error("Ошибка чтения файла:", streamErr);
                        res.end();
                    });
                }
            });

        // --- Задание 14  ---
        } else if (pathname === '/upload') {
             res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
             res.end(`
                <!DOCTYPE html>
                <html>
                <head>
                    <title>Загрузка файла</title>
                    <meta charset="UTF-8">
                </head>
                <body>
                    <h1>Загрузка файла (Задание 14)</h1>
                    <form method="POST" action="/upload" enctype="multipart/form-data">
                        <label for="fileInput">Выберите файл для загрузки:</label>
                        <input type="file" id="fileInput" name="fileToUpload"><br><br>
                        <input type="submit" value="Загрузить файл">
                    </form>
                </body>
                </html>
             `);
        } else {
            res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end('404 Not Found');
        }

    } else if (method === 'POST') {
        // --- Задание 07 ---
        if (pathname === '/req-data') {
            let body = '';
            console.log('Начало приема данных для /req-data');
            req.on('data', chunk => {
                console.log(`Получен чанк размером ${chunk.length} байт`);
                body += chunk.toString(); 
            });
            req.on('end', () => {
                console.log('Прием данных для /req-data завершен.');
                res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
                res.end(`Данные получены. Общий размер: ${Buffer.byteLength(body)} байт.`);
            });
            req.on('error', (err) => {
                 console.error("Ошибка при чтении тела запроса:", err);
                 res.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
                 res.end('Ошибка сервера при чтении запроса.');
            });

        // --- Задание 09 ---
        } else if (pathname === '/formparameter') {
             let body = '';
             req.on('data', chunk => { body += chunk.toString(); });
             req.on('end', () => {
                 if (req.headers['content-type'] !== 'application/x-www-form-urlencoded') {
                     res.writeHead(415, { 'Content-Type': 'text/plain; charset=utf-8' });
                     res.end('Unsupported Media Type: Ожидается application/x-www-form-urlencoded');
                     return;
                 }
                 try {
                    const formData = querystring.parse(body);
                    res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.write('--- Полученные параметры формы ---\n');
                    for (const key in formData) {
                        if (Array.isArray(formData[key])) {
                             res.write(`${key}: ${formData[key].join(', ')}\n`);
                        } else {
                             res.write(`${key}: ${formData[key]}\n`);
                        }
                    }
                    res.end();
                 } catch (e) {
                    console.error("Ошибка парсинга form data:", e);
                    res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.end('Ошибка парсинга данных формы.');
                 }
             });
             req.on('error', (err) => {
                 console.error("Ошибка при чтении тела запроса:", err);
                 res.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
                 res.end('Ошибка сервера при чтении запроса.');
            });

        // --- Задание 10 ---
        } else if (pathname === '/json') {
            let body = '';
            req.on('data', chunk => { body += chunk.toString(); });
            req.on('end', () => {
                 if (req.headers['content-type'] !== 'application/json') {
                     res.writeHead(415, { 'Content-Type': 'text/plain; charset=utf-8' });
                     res.end('Unsupported Media Type: Ожидается application/json');
                     return;
                 }
                 try {
                    const requestJson = JSON.parse(body);

                    if (typeof requestJson !== 'object' || requestJson === null ||
                        typeof requestJson._comment !== 'string' ||
                        typeof requestJson.x !== 'number' ||
                        typeof requestJson.y !== 'number' ||
                        typeof requestJson.s !== 'string' ||
                        typeof requestJson.o !== 'object' || requestJson.o === null || typeof requestJson.o.surname !== 'string' || typeof requestJson.o.name !== 'string' ||
                        !Array.isArray(requestJson.m)) {
                            throw new Error("Неверная структура JSON запроса");
                    }

                    const responseJson = {
                        "_comment": "Ответ сервера",
                        "x_plus_y": requestJson.x + requestJson.y,
                        "Concatenation_s_o": `${requestJson.s}: ${requestJson.o.surname}, ${requestJson.o.name}`,
                        "Length_m": requestJson.m.length
                    };

                    res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
                    res.end(JSON.stringify(responseJson));

                 } catch (e) {
                    console.error("Ошибка парсинга JSON или обработки:", e);
                    res.writeHead(400, { 'Content-Type': 'application/json; charset=utf-8' });
                    res.end(JSON.stringify({ error: "Ошибка парсинга JSON или неверный формат запроса", details: e.message }));
                 }
            });
             req.on('error', (err) => {
                 console.error("Ошибка при чтении тела запроса:", err);
                 res.writeHead(500, { 'Content-Type': 'application/json; charset=utf-8' });
                 res.end(JSON.stringify({ error: "Ошибка сервера при чтении запроса."}));
            });

        // --- Задание 11 ---
        } else if (pathname === '/xml') {
            let body = '';
            req.on('data', chunk => { body += chunk.toString(); });
            req.on('end', () => {
                // Проверяем Content-Type (может быть text/xml или application/xml)
                const contentType = req.headers['content-type'];
                if (contentType !== 'application/xml' && contentType !== 'text/xml') {
                    res.writeHead(415, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.end(`Unsupported Media Type: Ожидается application/xml или text/xml, получено ${contentType}`);
                    return;
                }

                const parser = new xml2js.Parser({ explicitArray: true }); // explicitArray: true - чтобы x и m всегда были массивами
                parser.parseString(body, (err, result) => {
                    if (err) {
                        console.error("Ошибка парсинга XML:", err);
                        res.writeHead(400, { 'Content-Type': 'application/xml; charset=utf-8' });
                        res.end('<error>Ошибка парсинга XML</error>');
                        return;
                    }

                    try {
                        if (!result || !result.request || !result.request.x || !result.request.m) {
                             throw new Error("Неверная структура XML запроса");
                        }

                        let sum = 0;
                        if (result.request.x && Array.isArray(result.request.x)) {
                            result.request.x.forEach(item => {
                                // Значение атрибута value находится в item.$.value
                                if (item && item.$ && item.$.value) {
                                    const val = parseFloat(item.$.value);
                                    if (!isNaN(val)) {
                                        sum += val;
                                    }
                                }
                            });
                        }

                        let concat = '';
                         // result.request.m это массив элементов m
                        if (result.request.m && Array.isArray(result.request.m)) {
                            result.request.m.forEach(item => {
                                if (item && item.$ && item.$.value) {
                                    concat += item.$.value;
                                }
                            });
                        }

                        // Формируем объект для ответа
                        const responseObject = {
                            response: {
                                $: { id: 'response_id', request: result.request.$.id || 'unknown' }, // Пример добавления атрибутов
                                sum: { $: { element: 'x', result: sum.toString() } },
                                concat: { $: { element: 'm', result: concat } }
                            }
                        };

                        const builder = new xml2js.Builder();
                        const xmlResponse = builder.buildObject(responseObject);

                        res.writeHead(200, { 'Content-Type': 'application/xml; charset=utf-8' });
                        res.end(xmlResponse);

                    } catch (e) {
                         console.error("Ошибка обработки XML данных:", e);
                         res.writeHead(400, { 'Content-Type': 'application/xml; charset=utf-8' });
                         res.end(`<error>Ошибка обработки данных XML: ${e.message}</error>`);
                    }
                });
            });
             req.on('error', (err) => {
                 console.error("Ошибка при чтении тела запроса:", err);
                 res.writeHead(500, { 'Content-Type': 'application/xml; charset=utf-8' });
                 res.end('<error>Ошибка сервера при чтении запроса.</error>');
            });

        // --- Задание 14 (Прием файла) ---
        } else if (pathname === '/upload') {
            if (!req.headers['content-type'] || !req.headers['content-type'].startsWith('multipart/form-data')) {
                // это MIME-тип, используемый для отправки данных HTML-форм, когда форма содержит файлы
                res.writeHead(415, { 'Content-Type': 'text/plain; charset=utf-8' });
                res.end('Unsupported Media Type: Ожидается multipart/form-data');
                return;
            }

            const form = new IncomingForm({ // облегчить парсинг (разбор) данных, приходящих в теле HTTP-запроса
                uploadDir: STATIC_DIR, // Директория для сохранения файлов
                keepExtensions: true, // Сохранять расширения файлов
            });

            form.parse(req, (err, fields, files) => {
                if (err) {
                    console.error("Ошибка парсинга формы (upload):", err);
                    res.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.end('Ошибка сервера при обработке загрузки файла.');
                    return;
                }

                // fields содержит поля формы в виде пар ключ-значение
                // files содержит информацию о загруженных файлах из полей <input type="file">. Это объект, где ключ — name поля файла, а значение — объект с информацией о файле
                const uploadedFile = files.fileToUpload; // Имя поля из нашей HTML формы

                if (!uploadedFile) {
                     res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
                     res.end('Файл не был загружен. Убедитесь, что поле называется "fileToUpload".');
                     return;
                }

                // является ли uploadedFile массивом
                const fileInfo = Array.isArray(uploadedFile) ? uploadedFile[0] : uploadedFile;

                if (!fileInfo || !fileInfo.originalFilename) {
                     res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
                     res.end('Не удалось получить информацию о загруженном файле.');
                     return;
                }

                // / На данный момент файл уже сохранен на сервере под временным именем
                // в папке STATIC

                const oldPath = fileInfo.filepath; // Путь к временно сохраненному файлу
                const newPath = path.join(STATIC_DIR, fileInfo.originalFilename); // Новый путь с оригинальным именем

                fs.rename(oldPath, newPath, (renameErr) => {
                    if (renameErr) {
                        console.error("Ошибка переименования файла:", renameErr);
                         res.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
                         res.end(`Файл "${fileInfo.originalFilename}" загружен, но произошла ошибка при переименовании.`);
                    } else {
                        console.log(`Файл ${fileInfo.originalFilename} успешно загружен и сохранен в ${newPath}`);
                        res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
                        res.end(`Файл "${fileInfo.originalFilename}" успешно загружен!`);
                    }
                });
            });

             form.on('error', (err) => {
                console.error("Ошибка парсера Formidable:", err);
                if (!res.headersSent) { // Отправляем ответ только если он еще не был отправлен
                    res.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
                    res.end('Ошибка сервера при обработке формы.');
                }
            });
        } else {
            res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end('404 Not Found');
        }

    } else {
        res.writeHead(405, { 'Content-Type': 'text/plain; charset=utf-8', 'Allow': 'GET, POST' });
        res.end('405 Method Not Allowed');
    }
});

// --- Запуск сервера ---
server.listen(PORT, () => {
    console.log(`Сервер запущен на http://localhost:${PORT}`);
    console.log(`Директория static: ${STATIC_DIR}`);
    console.log(`Начальное значение KeepAliveTimeout: ${server.keepAliveTimeout}ms`);
    // Создаем директорию static, если она не существует
    if (!fs.existsSync(STATIC_DIR)) {
        fs.mkdirSync(STATIC_DIR);
        console.log(`Создана директория ${STATIC_DIR}`);
    }
});

// --- Обработка ошибок сервера ---
server.on('error', (err) => {
    console.error("Ошибка сервера:", err);
});