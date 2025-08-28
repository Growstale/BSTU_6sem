const http = require('http');

const options = {
    hostname: '127.0.0.1',
    port: 3000,
    path: '/',
    method: 'GET',
};

const req = http.request(options, (res) => {
    console.log(`Статус ответа: ${res.statusCode}`);
    console.log(`Сообщение к статусу: ${res.statusMessage}`);
    console.log(`IP-адрес удаленного сервера: ${res.socket.remoteAddress}`);
    console.log(`Порт удаленного сервера: ${res.socket.remotePort}`);

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

console.log('Отправка GET запроса на http://127.0.0.1:3000/');
req.end();