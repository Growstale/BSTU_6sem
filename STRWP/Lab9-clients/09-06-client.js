const http = require('http');
const fs = require('fs');
const path = require('path');

const filePath = path.join(__dirname, 'MyFile.txt');
const serverHost = 'localhost';
const serverPort = 3000;
const serverPath = '/upload';

// Проверяем, существует ли файл перед отправкой
if (!fs.existsSync(filePath)) {
    console.error(`Ошибка: Файл не найден по пути ${filePath}`);
    process.exit(1); 
}

// Получаем размер файла для заголовка Content-Length
const fileStats = fs.statSync(filePath);

const options = {
    hostname: serverHost,
    port: serverPort,
    path: serverPath,
    method: 'POST',
    headers: {
        'Content-Type': 'text/plain',
        'Content-Length': fileStats.size // Обязательный заголовок для POST с телом
    }
};

console.log(`Отправка файла ${filePath} (${fileStats.size} байт) на http://${serverHost}:${serverPort}${serverPath}`);

const req = http.request(options, (res) => {
    console.log(`\nОтвет сервера:`);
    console.log(`Статус код: ${res.statusCode}`);
    console.log('Заголовки ответа:', res.headers);

    let responseBody = '';
    res.on('data', (chunk) => {
        responseBody += chunk;
    });

    res.on('end', () => {
        console.log('Тело ответа:');
        console.log(responseBody);
        console.log('\nЗапрос завершен.');
    });
});

req.on('error', (e) => {
    console.error(`\nОшибка при выполнении запроса: ${e.message}`);
});

// Создаем поток для чтения файла
const readStream = fs.createReadStream(filePath);

// Перенаправляем данные из файла (readStream) в запрос (req)
readStream.pipe(req);

readStream.on('error', (err) => {
    console.error(`Ошибка при чтении файла ${filePath}:`, err);
    req.abort(); // Прерываем HTTP-запрос, если файл не читается
});

// req.end() будет вызван автоматически, когда readStream завершит чтение
// и передачу всех данных через pipe(). Нам не нужно вызывать его вручную.