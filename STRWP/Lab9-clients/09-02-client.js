const http = require('http');
const querystring = require('querystring');

const params = { x: 15, y: 7 };
const path = `/sum?${querystring.stringify(params)}`;

const options = {
    hostname: '127.0.0.1',
    port: 3000,
    path: path,
    method: 'GET',
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

console.log(`Отправка GET запроса на http://127.0.0.1:3000${path}`);
req.end();