const http = require('http')
const url = require('url')
const fs = require('fs')

function factorial(n, callback) {
    if (n <= 1) {
        return callback(1)
    }
    
    process.nextTick(() => {
        factorial(n - 1, (result) => {
            callback(n * result)
        })
    })
}

const server = http.createServer((req, res) => {
    const parsedUrl  = url.parse(req.url, true)
    if (parsedUrl.pathname === '/') {
        let html = fs.readFileSync('./index.html')
        res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' })
        res.end(html)
    }
    else if (parsedUrl.pathname === '/fact') {
        const queryObject = parsedUrl.query
        if (queryObject.k) {
            const k = parseInt(queryObject.k, 10)
            
            if (isNaN(k)) {
                res.writeHead(400, { 'Content-Type': 'application/json' })
                res.end(JSON.stringify({ error: 'Invalid value for k, it must be a number' }))
                return
            }
    
            factorial(k, (fact) => {
                res.writeHead(200, { 'Content-Type': 'application/json' })
                res.end(JSON.stringify({ k: k, fact: fact }))
            })
        } else {
            res.writeHead(400, { 'Content-Type': 'application/json' })
            res.end(JSON.stringify({ error: 'Parameter k is required' }))
        }
    
    }
    else {
        res.writeHead(404, { 'Content-Type': 'text/plain' })
        res.end('The page is not found')
    }
})

server.listen(5000, () => {
    console.log('Server is running on http://localhost:5000')
})
