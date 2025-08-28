// Порядок выполнения: Вызывает RPC-методы (square, sum, mul, fib, fact) 
// один за другим. Он использует await перед каждым вызовом callRpc. 
// Это означает, что клиент отправляет запрос, ждет ответа от сервера, 
// обрабатывает его, и только потом отправляет следующий запрос

const WebSocket = require('ws');

const SERVER_URL = 'ws://localhost:4000';

const ws = new WebSocket(SERVER_URL);
let requestIdCounter = 1;

// Карта (Map) для хранения ожидающих ответов от сервера
const pendingRequests = new Map();

// Эта функция инкапсулирует логику отправки RPC-запроса и возвращает Promise
function callRpc(method, params) {
     return new Promise((resolve, reject) => {
        if (ws.readyState !== WebSocket.OPEN) {
            return reject(new Error('WebSocket connection is not open.'));
        }
        const id = requestIdCounter++;
        const request = { jsonrpc: '2.0', method: method, params: params, id: id };
        pendingRequests.set(id, { resolve, reject });
        const requestJson = JSON.stringify(request);
        console.log(`\n[Client] Sending RPC Request (ID: ${id}): ${requestJson}`);
        ws.send(requestJson, (err) => {
            if (err) {
                pendingRequests.delete(id);
                reject(new Error(`Failed to send RPC request: ${err.message}`));
            }
        });
        // Сервер не отвечает
        setTimeout(() => {
            if (pendingRequests.has(id)) {
                pendingRequests.get(id).reject(new Error(`RPC request (ID: ${id}) timed out`));
                pendingRequests.delete(id);
            }
        }, 10000);
    });
}

ws.on('open', async () => {
    console.log('[Client] Connected to RPC server.');

    try {
        console.log("\n--- Sequential RPC Calls ---");

        // --- ШАГ 1: Попытка вызова защищенного метода без логина (ожидаем ошибку) ---
        console.log("\n[Client] Attempting protected call (fib) before login (expecting error)...");
        try {
            await callRpc('fib', [5]);
            console.error("[Client] ERROR: Protected method call succeeded without login!");
        } catch(error) {
             console.log(`[Client] Received expected error: ${error.message} (Code: ${error.code})`);
        }

         // --- ШАГ 2: Логин ---
         console.log("\n[Client] Attempting login...");
         const loginResult = await callRpc('login', ["user", "password123"]);
         console.log(`[Client] Login result: ${loginResult}`);

        console.log("\n[Client] Calling methods after successful login...");

        // square
        let result = await callRpc('square', [3]);
        console.log(`[Client] square(3) = ${result}`);
        result = await callRpc('square', [5, 4]);
        console.log(`[Client] square(5, 4) = ${result}`);

        // sum
        result = await callRpc('sum', [2]);
        console.log(`[Client] sum(2) = ${result}`);
        result = await callRpc('sum', [2, 4, 6, 8, 10]);
        console.log(`[Client] sum(2, 4, 6, 8, 10) = ${result}`);

        // mul
        result = await callRpc('mul', [3]);
        console.log(`[Client] mul(3) = ${result}`);
        result = await callRpc('mul', [3, 5, 7, 9, 11, 13]);
        console.log(`[Client] mul(3, 5, 7, 9, 11, 13) = ${result}`);

        // fib
        result = await callRpc('fib', [1]);
        console.log(`[Client] fib(1) = [${result.join(', ')}]`);
        result = await callRpc('fib', [2]);
        console.log(`[Client] fib(2) = [${result.join(', ')}]`);
        result = await callRpc('fib', [7]);
        console.log(`[Client] fib(7) = [${result.join(', ')}]`);

        // fact 
        result = await callRpc('fact', [0]);
        console.log(`[Client] fact(0) = ${result}`);
        result = await callRpc('fact', [5]);
        console.log(`[Client] fact(5) = ${result}`);
        result = await callRpc('fact', [10]);
        console.log(`[Client] fact(10) = ${result}`);

        console.log('\n[Client] All sequential calls completed.');

    } catch (error) {
        console.error('[Client] Error during RPC call sequence:', error.message || error);
    } finally {
        ws.close();
    }
});

ws.on('message', (message) => {
     try {
        const response = JSON.parse(message.toString());
        console.log(`[Client] Received RPC Response (ID: ${response.id}): ${JSON.stringify(response)}`);
        if (pendingRequests.has(response.id)) {
            const { resolve, reject } = pendingRequests.get(response.id);
            if (response.error) {
                // Отклоняем (reject) соответствующий Promise, передавая объект ошибки от сервера
                reject(response.error);
            } else {
                // Разрешаем (resolve) соответствующий Promise, передавая результат из поля result
                resolve(response.result);
            }
            pendingRequests.delete(response.id);
        } else {
             console.warn(`[Client] Received response for unknown or timed out request ID: ${response.id}`);
        }
    } catch (error) {
        console.error('[Client] Error parsing server message:', error);
    }
});

ws.on('close', () => {
     console.log('[Client] Disconnected from server.');
});

ws.on('error', (error) => {
     console.error(`[Client] WebSocket error: ${error.message}`);
    pendingRequests.forEach(({ reject }) => reject(new Error('WebSocket connection error')));
    pendingRequests.clear();
});