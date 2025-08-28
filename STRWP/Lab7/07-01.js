const http = require('http');
const path = require('path');
const createStaticServer = require('./m07-01');

const staticDir = path.join(__dirname, 'static');
const PORT = 3000;

const requestHandler = createStaticServer(staticDir);
const server = http.createServer(requestHandler);

server.listen(PORT, () => {
    console.log(`http://localhost:${PORT}`);
});