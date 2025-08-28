const WebSocket = require('ws');
const serverAddress = 'ws://localhost:3000';

const ws = new WebSocket(serverAddress);

function displayPrompt() {
    process.stdout.write('> ');
}

ws.on('open', () => {
    console.log('Подключено к серверу', serverAddress);
    process.stdin.setEncoding('utf8');
    process.stdin.resume(); // Начинаем слушать stdin
    displayPrompt(); // Показываем первый промпт
});

ws.on('message', (message) => {
    process.stdout.write('\n'); 
    console.log(`Сервер: ${message}`);
    displayPrompt(); 
});

ws.on('close', () => {
    console.log('\nОтключено от сервера');
    process.exit(0); 
});

ws.on('error', (error) => {
    console.error(`\nОшибка WebSocket: ${error.message}`);
    console.log('Не удалось подключиться к серверу. Попробуйте запустить сервер.');
    process.exit(1);
});

// Обработчик получения данных из stdin (ввод пользователя)
process.stdin.on('data', (input) => {
    const message = input.trim();

    if (message.toLowerCase() === 'exit') {
        ws.close();
        return; 
    }

    if (ws.readyState === WebSocket.OPEN) {
        try {
            ws.send(message);
             displayPrompt();
        } catch (error) {
             console.error(`\nОшибка отправки: ${error.message}`);
             displayPrompt();
        }
    } else {
        console.log('\nСоединение не установлено. Невозможно отправить сообщение.');
        displayPrompt();
    }
});