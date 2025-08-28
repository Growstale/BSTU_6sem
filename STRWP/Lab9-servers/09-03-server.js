const http = require('http');
const querystring = require('querystring');

const PORT = 3000;

const server = http.createServer((req, res) => {
    if (req.method === 'POST' && req.url === '/data') {
        console.log(`[${new Date().toISOString()}] Получен POST запрос на /data`);
        let body = '';

        req.on('data', chunk => {
            body += chunk.toString();
        });

        req.on('end', () => {
            try {
                const postData = querystring.parse(body);
                console.log('Полученные параметры:', postData);

                const x = parseInt(postData.x);
                const y = parseInt(postData.y);
                const s = postData.s;

                if (isNaN(x) || isNaN(y) || typeof s !== 'string') {
                   res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
                   res.end('Ошибка: Неверные или отсутствующие параметры x, y, s.');
                   return;
                }

                const responseMessage = `Получены параметры: x=${x}, y=${y}, s='${s}'. Результат: ${x+y}-${s}`;
                res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
                res.end(responseMessage);

            } catch (error) {
                res.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
                res.end('Ошибка сервера при обработке данных.');
            }
        });

    } else {
        res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Ресурс не найден');
    }
});

server.listen(PORT, '127.0.0.1', () => {
    console.log(`Сервер 09-03 запущен на http://127.0.0.1:${PORT}`);
});

server.on('error', (err) => {
    console.error('Ошибка сервера:', err);
});