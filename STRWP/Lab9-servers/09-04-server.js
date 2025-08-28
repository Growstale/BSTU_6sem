const http = require('http');

const PORT = 3000;

const server = http.createServer((req, res) => {
    if (req.method === 'POST' && req.url === '/json') {
        console.log(`[${new Date().toISOString()}] Получен POST запрос на /json`);
        let body = '';

        req.on('data', chunk => {
            body += chunk.toString();
        });

        req.on('end', () => {
            try {
                if (req.headers['content-type'] !== 'application/json') {
                   res.writeHead(400, { 'Content-Type': 'application/json; charset=utf-8' });
                   res.end(JSON.stringify({ error: 'Ожидался Content-Type: application/json' }));
                   return;
                }

                const jsonData = JSON.parse(body);
                console.log('Полученные JSON данные:', jsonData);


                if (typeof jsonData !== 'object' || jsonData === null ||
                    typeof jsonData._comment !== 'string' ||
                    typeof jsonData.x !== 'number' ||
                    typeof jsonData.y !== 'number' ||
                    typeof jsonData.s !== 'string' ||
                    typeof jsonData.o !== 'object' || jsonData.o === null || typeof jsonData.o.surname !== 'string' || typeof jsonData.o.name !== 'string' ||
                    !Array.isArray(jsonData.m)) {
                        res.writeHead(400, { 'Content-Type': 'application/json; charset=utf-8' });
                        res.end(JSON.stringify({ error: 'Неверная структура JSON данных' }));
                        return;
                }

            
                const responseJson = {
                    "_comment": "Ответ сервера",
                    "x_plus_y": jsonData.x + jsonData.y,
                    "Concatenation_s_o": `${jsonData.s}: ${jsonData.o.surname}, ${jsonData.o.name}`,
                    "Length_m": jsonData.m.length
                };


                res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
                res.end(JSON.stringify(responseJson));

            } catch (error) {
                console.error("Ошибка парсинга JSON:", error);
                res.writeHead(400, { 'Content-Type': 'application/json; charset=utf-8' });
                res.end(JSON.stringify({ error: 'Ошибка парсинга JSON запроса' }));
            }
        });

    } else {
        res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Ресурс не найден');
    }
});

server.listen(PORT, '127.0.0.1', () => {
    console.log(`Сервер 09-04 запущен на http://127.0.0.1:${PORT}`);
});

server.on('error', (err) => {
    console.error('Ошибка сервера:', err);
});