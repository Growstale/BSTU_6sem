const http = require('http');
const querystring = require('querystring');

const postData = querystring.stringify({
    x: '5', // Отправляем как строки, сервер распарсит
    y: '10',
    s: 'Тестовое сообщение'
});

const options = {
    hostname: '127.0.0.1',
    port: 3000,
    path: '/data',
    method: 'POST',
    headers: {
        'Content-Type': 'application/x-www-form-urlencoded',
        'Content-Length': Buffer.byteLength(postData) // Важно для POST
        // Он указывает серверу точный размер (в байтах) данных, содержащихся в теле запроса
    }
};

const req = http.request(options, (res) => {
    console.log(`Статус ответа: ${res.statusCode}`);

    let data = '';
    res.on('data', (chunk) => {
        data += chunk;
    });

    res.on('end', () => {
        console.log('Данные в теле ответа:', data);
    });
});

req.on('error', (e) => {
    console.error(`Ошибка при запросе: ${e.message}`);
});

console.log('Отправка POST запроса на http://127.0.0.1:3000/data');
// Записываем данные в тело запроса
req.write(postData);
req.end();