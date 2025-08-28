// Порядок выполнения: После успешного логина клиент инициирует 
// все RPC-вызовы (square, sum, mul, fib, fact) практически одновременно, 
// не дожидаясь ответа на предыдущий. Он собирает все возвращаемые функцией 
// callRpc промисы в один массив (promises). Затем он использует 
// await Promise.all(promises), чтобы дождаться, пока все отправленные 
// запросы получат свои ответы.

const WebSocket = require('ws');

const SERVER_URL = 'ws://localhost:4000';

const ws = new WebSocket(SERVER_URL);
let requestIdCounter = 1;
const pendingRequests = new Map();

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
        console.log("\n--- Parallel RPC Calls with Login ---");

        console.log("\n[Client] Step 1: Attempting login...");
        const loginResult = await callRpc('login', ["user", "password123"]);
        console.log(`[Client] Login result: ${loginResult}`);

        console.log("\n[Client] Step 2: Initiating all calls in parallel...");
        const promises = [

            callRpc('square', [3]),
            callRpc('square', [5, 4]),

            callRpc('sum', [2]),
            callRpc('sum', [2, 4, 6, 8, 10]),

            callRpc('mul', [3]),
            callRpc('mul', [3, 5, 7, 9, 11, 13]),

            callRpc('fib', [1]),
            callRpc('fib', [2]),
            callRpc('fib', [7]),

            callRpc('fact', [0]),
            callRpc('fact', [5]),
            callRpc('fact', [10])
        ];

        // Ожидаем выполнения всех промисов параллельно
        const results = await Promise.all(promises);

        console.log("\n[Client] Parallel RPC Results (after login):");
        console.log(` - square(3) = ${results[0]}`);
        console.log(` - square(5, 4) = ${results[1]}`);
        console.log(` - sum(2) = ${results[2]}`);
        console.log(` - sum(2, 4, 6, 8, 10) = ${results[3]}`);
        console.log(` - mul(3) = ${results[4]}`);
        console.log(` - mul(3, 5, 7, 9, 11, 13) = ${results[5]}`);
        console.log(` - fib(1) = [${results[6].join(', ')}]`);
        console.log(` - fib(2) = [${results[7].join(', ')}]`);
        console.log(` - fib(7) = [${results[8].join(', ')}]`);
        console.log(` - fact(0) = ${results[9]}`);
        console.log(` - fact(5) = ${results[10]}`);
        console.log(` - fact(10) = ${results[11]}`);

        console.log('\n[Client] All parallel calls completed.');

    } catch (error) {
        console.error('[Client] Error during parallel RPC calls:', error.message || error);
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
                reject(response.error);
            } else {
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