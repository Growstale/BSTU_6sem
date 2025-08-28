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
        console.log("\n--- Expression Evaluation with Login ---");
        console.log("Calculating: sum(square(3), square(5,4), mul(3,5,7,9,11,13)) + fib(7)[last] * mul(2,4,6)");

        console.log("\n[Client] Step 1: Attempting login...");
        const loginResult = await callRpc('login', ["user", "password123"]); // Используйте правильные учетные данные
        console.log(`[Client] Login result: ${loginResult}`);


        console.log("\n[Client] Step 2: Calling base functions in parallel (post-login)...");
        const [sq1Result, sq2Result, mul1Result, fibResult, mul2Result] = await Promise.all([
            callRpc('square', [3]),              // public
            callRpc('square', [5, 4]),           // public
            callRpc('mul', [3, 5, 7, 9, 11, 13]), // public
            callRpc('fib', [7]),                 // protected
            callRpc('mul', [2, 4, 6])            // public
        ]);

        console.log("\n[Client] Step 2 Results:");
        console.log(` - square(3) = ${sq1Result}`);
        console.log(` - square(5, 4) = ${sq2Result}`);
        console.log(` - mul(3,..,13) = ${mul1Result}`);
        console.log(` - fib(7) = [${fibResult.join(', ')}]`); // Должен успешно выполниться
        console.log(` - mul(2,4,6) = ${mul2Result}`);

        // Вызываем sum с результатами из шага 2
        console.log("\n[Client] Step 3: Calling sum with previous results...");
        const sumParams = [sq1Result, sq2Result, mul1Result];
        const sumResult = await callRpc('sum', sumParams); 
        console.log(`[Client] Step 3 Result: sum(${sumParams.join(', ')}) = ${sumResult}`);

        console.log("\n[Client] Step 4: Performing final calculation...");
        const lastFibNumber = fibResult.length > 0 ? fibResult[fibResult.length - 1] : 0;
        console.log(` - Using last element of fib(7): ${lastFibNumber}`);
        const finalResult = sumResult + (lastFibNumber * mul2Result);

        console.log(`\n--- Final Result ---`);
        console.log(`${sumResult} + (${lastFibNumber} * ${mul2Result}) = ${finalResult}`);
        console.log(`Expression result: ${finalResult}`);


    } catch (error) {
        console.error('[Client] Error during expression evaluation:', error.message || error);
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