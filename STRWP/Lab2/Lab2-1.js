const http = require('http')
const fs = require('fs')

http.createServer(function (request, response) {
    if (request.url === '/html') {
        let html = fs.readFileSync('./index.html')
        response.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' })
        response.end(html)
    } else {
        response.writeHead(404, { 'Content-Type': 'text/plain' })
        response.end('The page is not found')
    }
}).listen(5000)