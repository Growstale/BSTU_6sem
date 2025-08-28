var http = require('http')
var fs = require('fs')

http.createServer(function (request, response) {
    if (request.method === 'GET' && request.url === '/png') {
        const fname = './pic.jpg'

        fs.stat(fname, (err, stat) => {
            if (err) {
                console.log('error:', err)
            } else {
                let jpg = fs.readFileSync(fname)
                response.writeHead(200, { 'Content-Type': 'image/jpeg', 'Content-Length': stat.size })
                response.end(jpg)
            }
        })
    } else {
    response.writeHead(404, { 'Content-Type': 'text/plain' })
    response.end('The page is not found')
}
}).listen(5000)