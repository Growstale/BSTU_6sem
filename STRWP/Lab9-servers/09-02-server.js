const http = require('http');
const url = require('url');

const PORT = 3000;

const server = http.createServer((req, res) => {
    const parsedUrl = url.parse(req.url, true);

    if (req.method === 'GET' && parsedUrl.pathname === '/sum') {
        console.log(`[${new Date().toISOString()}] Получен GET запрос на /sum с параметрами`);
        const x = parseInt(parsedUrl.query.x);
        const y = parseInt(parsedUrl.query.y);

        if (isNaN(x) || isNaN(y)) {
            res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end('Ошибка: Параметры x и y должны быть числами.');
            return;
        }

        const sum = x + y;
        res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end(`Сумма ${x} и ${y} равна ${sum}`);

    } else {
        res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Ресурс не найден');
    }
});

server.listen(PORT, '127.0.0.1', () => {
    console.log(`Сервер 09-02 запущен на http://127.0.0.1:${PORT}`);
});

server.on('error', (err) => {
    console.error('Ошибка сервера:', err);
});