const http = require('http');

const PORT = 3000;

const server = http.createServer((req, res) => {
    if (req.method === 'GET' && req.url === '/') {
        console.log(`[${new Date().toISOString()}] Получен GET запрос на /`);

        res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Привет от сервера!');
    } else {
        res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Ресурс не найден');
    }
});

server.listen(PORT, '127.0.0.1', () => {
    console.log(`Сервер 09-01 запущен на http://127.0.0.1:${PORT}`);
});

server.on('error', (err) => {
    console.error('Ошибка сервера:', err);
});