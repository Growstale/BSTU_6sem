const http = require('http');

http.createServer((request, response) => {
    if (request.method === 'GET' && request.url === '/api/name') {
        response.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
        response.end('Водчиц Анастасия Витальевна'); 
    } else {
        response.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
        response.end('The page is not found');
    }
}).listen(5000);