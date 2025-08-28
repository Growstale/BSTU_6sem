const http = require('http')
const fs = require('fs')

http.createServer(function (request, response) {
    if (request.url === '/fetch') {
        let html = fs.readFileSync('./fetch.html')
        response.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' })
        response.end(html)
    } else if (request.method === 'GET' && request.url === '/api/name') {
        response.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
        response.end('Водчиц Анастасия Витальевна'); 
    } else {
        response.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
        response.end('The page is not found');
    }

}).listen(5000)
