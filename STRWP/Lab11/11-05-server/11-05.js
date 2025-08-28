const WebSocket = require('ws');

const PORT = 4000;

const VALID_USERNAME = "user";
const VALID_PASSWORD = "password123";

function square(params, ws) { 
    if (!Array.isArray(params)) throw new Error('Parameters must be an array.');
    if (params.length === 1 && typeof params[0] === 'number') {
        const r = params[0];
        return Math.PI * r * r;
    } else if (params.length === 2 && typeof params[0] === 'number' && typeof params[1] === 'number') {
        const a = params[0];
        const b = params[1];
        return a * b;
    } else {
        throw new Error('Invalid parameters for square. Use [r] or [a, b].');
    }
}

function sum(params, ws) { 
    if (!Array.isArray(params)) throw new Error('Parameters must be an array.');
    if (params.some(p => typeof p !== 'number')) {
        throw new Error('All parameters for sum must be numbers.');
    }
    return params.reduce((acc, val) => acc + val, 0);
}

function mul(params, ws) { 
    if (!Array.isArray(params)) throw new Error('Parameters must be an array.');
     if (params.some(p => typeof p !== 'number')) {
        throw new Error('All parameters for mul must be numbers.');
    }
    return params.reduce((acc, val) => acc * val, 1);
}

// protected
function fib(params, ws) { 
    if (!Array.isArray(params) || params.length !== 1 || typeof params[0] !== 'number' || !Number.isInteger(params[0]) || params[0] < 0) {
        throw new Error('Invalid parameter for fib. Use [n] where n is a non-negative integer.');
    }
    const n = params[0];
    if (n === 0) return [];
    if (n === 1) return [0];
    const result = [0, 1];
    if (n === 2) return result;
    for (let i = 2; i < n; i++) {
        result.push(result[i - 1] + result[i - 2]);
    }
    return result;
}

function fact(params, ws) {
     if (!Array.isArray(params) || params.length !== 1 || typeof params[0] !== 'number' || !Number.isInteger(params[0]) || params[0] < 0) {
        throw new Error('Invalid parameter for fact. Use [n] where n is a non-negative integer.');
    }
    const n = params[0];
    if (n === 0) return 1;
    let result = 1;
    for (let i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

function login(params, ws) { 
    if (!Array.isArray(params) || params.length !== 2 || typeof params[0] !== 'string' || typeof params[1] !== 'string') {
        throw new Error('Invalid parameters for login. Use [username, password].');
    }
    const username = params[0];
    const password = params[1];

    if (username === VALID_USERNAME && password === VALID_PASSWORD) {
        ws.isLoggedIn = true;
        console.log(`Client ${ws._socket.remoteAddress} logged in successfully as '${username}'.`);
        return "Login successful";
    } else {
        ws.isLoggedIn = false;
        console.log(`Client ${ws._socket.remoteAddress} login failed for user '${username}'.`);
        throw new Error("Invalid credentials");
    }
}


const rpcMethods = {
    square,
    sum,
    mul,
    fib,
    fact,
    login 
};

const protectedMethods = new Set(['fib', 'fact']);

const wss = new WebSocket.Server({ port: PORT });
console.log(`WebSocket RPC server with login started on ws://localhost:${PORT}`);

wss.on('connection', (ws) => {
    console.log('Client connected');
    ws.isLoggedIn = false;

    ws.on('message', (message) => {
        let request;
        let response;

        try {
            request = JSON.parse(message.toString());

            if (request.jsonrpc !== '2.0' || !request.method || !request.id) {
                throw new Error('Invalid JSON-RPC request structure.');
            }

            console.log(`Received RPC call: method=${request.method}, params=${JSON.stringify(request.params)}, id=${request.id}`);

            const method = rpcMethods[request.method];
            if (!method) {
                throw { code: -32601, message: `Method not found: ${request.method}` };
            }

            if (protectedMethods.has(request.method)) {
                if (!ws.isLoggedIn) {
                     console.log(`Access denied for ${request.method}: Client not logged in.`);
                     throw { code: -32001, message: `Access denied. Please login to use method: ${request.method}` }; // Пользовательский код ошибки
                } else {
                     console.log(`Access granted for protected method ${request.method}: Client is logged in.`);
                }
            }

            const result = method(request.params || [], ws);

            response = {
                jsonrpc: '2.0',
                result: result,
                id: request.id
            };

        } catch (error) {
            console.error('RPC Error:', error);
            response = {
                jsonrpc: '2.0',
                error: {
                    code: error.code || (error instanceof SyntaxError ? -32700 : -32000),
                    message: error.message || 'An error occurred'
                },
                id: request ? request.id : null
            };
        }

        if (response && ws.readyState === WebSocket.OPEN) {
             const responseJson = JSON.stringify(response);
             console.log(`Sending RPC response: ${responseJson}`);
             ws.send(responseJson);
        }
    });

    ws.on('close', () => {
        console.log(`Client disconnected (logged in: ${ws.isLoggedIn})`);
    });

    ws.on('error', (error) => {
        console.error('WebSocket client error:', error);
    });
});

wss.on('error', (error) => {
    console.error('WebSocket Server Error:', error);
});