const JsonRPCServer = require('jsonrpc-server-http-nats');

const server = new JsonRPCServer();

// Процедура sum(x1, x2, ..., xn)
server.on('sum', (params, response) => {
    console.log('Handling "sum" with params:', params);
    if (!Array.isArray(params)) {
        return response({ code: -32602, message: 'Invalid params: Expected an array for sum.' }, null);
    }
    if (params.some(isNaN)) {
        return response({ code: -32602, message: 'Invalid params: All elements must be numbers for sum.' }, null);
    }
    try {
        const result = params.reduce((acc, val) => acc + Number(val), 0);
        response(null, result);
    } catch (e) {
        response({ code: -32000, message: 'Server error during sum calculation: ' + e.message }, null);
    }
});

// Процедура mul(x1, x2, ..., xn)
server.on('mul', (params, channel, response) => {
    console.log('Handling "mul" with params:', params);
    if (!Array.isArray(params)) {
        return response({ code: -32602, message: 'Invalid params: Expected an array for mul.' }, null);
    }
    if (params.length === 0) { 
        return response(null, 1);
    }
    if (params.some(isNaN)) {
        return response({ code: -32602, message: 'Invalid params: All elements must be numbers for mul.' }, null);
    }
    try {
        const result = params.reduce((acc, val) => acc * Number(val), 1);
        response(null, result);
    } catch (e) {
        response({ code: -32000, message: 'Server error during mul calculation: ' + e.message }, null);
    }
});

// Процедура div(x, y)
server.on('div', (params, channel, response) => {
    console.log('Handling "div" with params:', params);
    if (!Array.isArray(params) || params.length !== 2) {
        return response({ code: -32602, message: 'Invalid params: Expected an array of two numbers for div [x, y].' }, null);
    }
    const [x, y] = params.map(Number);
    if (isNaN(x) || isNaN(y)) {
        return response({ code: -32602, message: 'Invalid params: Both parameters must be numbers for div.' }, null);
    }
    if (y === 0) {
        return response({ code: -32001, message: 'Server error: Division by zero.' }, null); // Пользовательская ошибка сервера
    }
    try {
        response(null, x / y);
    } catch (e) {
        response({ code: -32000, message: 'Server error during div calculation: ' + e.message }, null);
    }
});

// Процедура proc(x, y)
server.on('proc', (params, channel, response) => {
    console.log('Handling "proc" with params:', params);
    if (!Array.isArray(params) || params.length !== 2) {
        return response({ code: -32602, message: 'Invalid params: Expected an array of two numbers for proc [x, y].' }, null);
    }
    const [x, y] = params.map(Number);
    if (isNaN(x) || isNaN(y)) {
        return response({ code: -32602, message: 'Invalid params: Both parameters must be numbers for proc.' }, null);
    }
    if (y === 0) {
        // В зависимости от того, как трактовать x/0 * 100. Обычно это ошибка.
        return response({ code: -32001, message: 'Server error: Division by zero in percentage calculation.' }, null);
    }
    try {
        response(null, (x / y) * 100);
    } catch (e) {
        response({ code: -32000, message: 'Server error during proc calculation: ' + e.message }, null);
    }
});

// Запуск HTTP сервера
const PORT = 3000; // Можете выбрать другой порт
server.listenHttp({ host: '127.0.0.1', port: PORT }, () => {
    console.log(`JSON-RPC Server 25-01 listening on http://127.0.0.1:${PORT}`);
    console.log('Available methods: sum, mul, div, proc');
});