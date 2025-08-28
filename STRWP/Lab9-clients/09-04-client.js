const http = require('http');

const jsonData = {
    _comment: "Запрос от клиента 09-04",
    x: 11,
    y: 22,
    s: "Привет JSON",
    m: ["a", "b", "c"],
    o: { surname: "Петров", name: "Петр" }
};

const postData = JSON.stringify(jsonData);

const options = {
    hostname: '127.0.0.1',
    port: 3000,
    path: '/json',
    method: 'POST',
    headers: {
        'Content-Type': 'application/json',
        'Content-Length': Buffer.byteLength(postData),
        'Accept': 'application/json' // Сообщаем серверу, что ожидаем JSON в ответ
    }
};

const req = http.request(options, (res) => {
    console.log(`Статус ответа: ${res.statusCode}`);

    let data = '';
    res.on('data', (chunk) => {
        data += chunk;
    });

    res.on('end', () => {
        try {
            const parsedData = JSON.parse(data);
            console.log('Данные в теле ответа (JSON):', parsedData);
        } catch (error) {
            console.error('Ошибка парсинга JSON ответа:', error);
            console.log('Данные в теле ответа (raw):', data);
        }
    });
});

req.on('error', (e) => {
    console.error(`Ошибка при запросе: ${e.message}`);
});

console.log('Отправка POST запроса с JSON на http://127.0.0.1:3000/json');
req.write(postData);
req.end();