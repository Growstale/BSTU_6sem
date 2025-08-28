const http = require('http');
const fs = require('fs');
const path = require('path');

const serverOptions = {
    hostname: 'localhost',
    port: 3001, 
    path: '/upload_raw',
    method: 'POST',
    headers: {
        'Content-Type': 'image/png',
    }
};

const filePath = path.join(__dirname, 'image.jpg');

// Проверяем, существует ли файл
if (!fs.existsSync(filePath)) {
    console.error(`Ошибка: Файл не найден по пути "${filePath}"`);
    process.exit(1);
}

// Получаем размер файла для информации
const stats = fs.statSync(filePath);
const fileSize = stats.size;
console.log(`Файл для отправки: "${path.basename(filePath)}", Размер: ${fileSize} байт`);

const req = http.request(serverOptions, (res) => {

    let responseBody = '';
    res.setEncoding('utf8');
    res.on('data', (chunk) => {
        responseBody += chunk;
    });
    res.on('end', () => {
        console.log('Тело ответа:');
        try {
            // Пытаемся распарсить JSON
            console.log(JSON.parse(responseBody));
        } catch (e) {
            // Если не JSON, выводим как текст
            console.log(responseBody);
        }
        console.log('\nОтправка файла завершена.');
    });
});

req.on('error', (e) => {
    console.error(`Ошибка при отправке запроса: ${e.message}`);
});

// Создаем поток для чтения файла
const readStream = fs.createReadStream(filePath);

// Обработка ошибок чтения файла
readStream.on('error', (err) => {
    console.error('Ошибка чтения файла:', err);
    req.destroy(err); // Прерываем запрос при ошибке чтения файла
});

// Перенаправляем (pipe) содержимое файла в тело запроса
// Node.js автоматически обработает поток, установит Content-Length (или Transfer-Encoding: chunked)
// и завершит запрос (req.end()) после окончания потока файла.
console.log(`Отправка содержимого файла на ${serverOptions.hostname}:${serverOptions.port}${serverOptions.path}...`);
readStream.pipe(req);
