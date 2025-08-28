const http = require('http');
const url = require('url');

function factorial(n) {
    if (n <= 1) return 1; 
    return n * factorial(n - 1);
}

const server = http.createServer((req, res) => {
    const queryObject = url.parse(req.url, true).query;

    if (queryObject.k) {
        const k = parseInt(queryObject.k, 10);
        
        if (isNaN(k)) {
            res.writeHead(400, { 'Content-Type': 'application/json' });
            res.end(JSON.stringify({ error: 'Invalid value for k, it must be a number' }));
            return;
        }

        const fact = factorial(k);

        res.writeHead(200, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify({ k: k, fact: fact }));
    } else {
        res.writeHead(400, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify({ error: 'Parameter k is required' }));
    }
});

server.listen(5000, () => {
    console.log('Server is running on http://localhost:5000');
});
