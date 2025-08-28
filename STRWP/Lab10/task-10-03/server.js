const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 3000 });

// Хранилище для всех подключенных клиентов
const clients = new Set();

console.log('WebSocket-сервер запущен на порту 3000...');

wss.on('connection', (ws) => {
    console.log('Клиент подключился');
    clients.add(ws);

    ws.on('message', (message) => {
        console.log(`Получено сообщение: ${message}`);

        broadcast(message, ws);
    });

    ws.on('close', () => {
        console.log('Клиент отключился');
        clients.delete(ws);
    });

    ws.on('error', (error) => {
        console.error(`Ошибка WebSocket: ${error}`);
        clients.delete(ws);
    });

    ws.send('Добро пожаловать на широковещательный сервер!');
});

// Функция для рассылки сообщения всем клиентам, кроме отправителя
function broadcast(message, sender) {
    clients.forEach((client) => {
        if (client !== sender && client.readyState === WebSocket.OPEN) {
            if (client.readyState === WebSocket.OPEN) {
                try {
                    client.send(message.toString());
                } catch (error) {
                    console.error(`Не удалось отправить сообщение клиенту: ${error}`);
                    clients.delete(client);
                }
            }
        }
    });
}

wss.on('error', (error) => {
    console.error(`Ошибка WebSocket-сервера: ${error}`);
});