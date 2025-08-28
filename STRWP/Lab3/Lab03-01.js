const http = require('http');

let appState = 'norm';
const validStates = ['norm', 'stop', 'test', 'idle'];

const server = http.createServer((req, res) => {
    res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
    res.end(`<h1>${appState}</h1>`);
});

server.listen(5000, () => {
    console.log('Server is running on http://localhost:5000');
    process.stdout.write(`reg = ${appState}--> `);
});

process.stdin.on("readable", () => {
    let chunk;
    while ((chunk = process.stdin.read()) !== null) {
        const input = chunk.toString().trim();
        
        if (validStates.includes(input)) {
            console.log(`${appState}->${input}`);
            appState = input;
        } else if (input === 'exit') {
            server.close();
            process.exit(0);
        } else {
            console.log(`Invalid state: ${input}`);
        }
        process.stdout.write(`reg = ${appState}--> `);
    }
});