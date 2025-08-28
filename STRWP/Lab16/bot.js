// Long Polling

const https = require('https'); 

const BOT_TOKEN = '7253327117:AAFv6FgOZ5pF3hKDhqWJQ7OPAUYrAHTVaTo';
const API_BASE_URL = `https://api.telegram.org/bot${BOT_TOKEN}`; // Базовый URL для всех запросов к API

let offset = 0; // Смещение для getUpdates, чтобы не получать старые сообщения
// offset - это ID последнего обработанного обновления + 1

function sendMessage(chatId, text) {
    const messageData = JSON.stringify({
        chat_id: chatId,
        text: text
    });

    const options = {
        hostname: 'api.telegram.org',
        port: 443,
        path: `/bot${BOT_TOKEN}/sendMessage`, // Путь к API-методу sendMessag
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Content-Length': Buffer.byteLength(messageData)
        }
    };

    const req = https.request(options, (res) => {
        let responseBody = '';
        res.on('data', (chunk) => {
            responseBody += chunk;
        });
        res.on('end', () => {
            try {
                const parsedResponse = JSON.parse(responseBody);
                if (!parsedResponse.ok) {
                    console.error('Error sending message:', parsedResponse.description);
                }
            } catch (e) {
                console.error('Error parsing send message response:', e);
            }
        });
    });

    req.on('error', (e) => {
        console.error('Problem with request to sendMessage:', e.message);
    });

    req.write(messageData);
    req.end();
}

// Функция для получения обновлений (Long Polling)
function getUpdates() {
    const url = `${API_BASE_URL}/getUpdates?offset=${offset}&timeout=60`; // timeout=60 секунд

    https.get(url, (res) => {
        let data = '';

        res.on('data', (chunk) => {
            data += chunk;
        });

        res.on('end', () => {
            try {
                const response = JSON.parse(data);
                if (response.ok && response.result) {
                    processUpdates(response.result);
                } else if (!response.ok) {
                    console.error('Error in getUpdates response:', response.description);
                }
            } catch (e) {
                console.error('Error parsing getUpdates response:', e.message);
                console.error('Received data:', data); // Показать, что пришло
            }
            // Рекурсивно вызываем getUpdates для продолжения опроса,
            // даже если были ошибки (но с небольшой задержкой, чтобы не спамить API)
            setTimeout(getUpdates, 100);
        });

    }).on('error', (e) => {
        console.error('Problem with request to getUpdates:', e.message);
        // В случае ошибки сети, пробуем снова через некоторое время
        setTimeout(getUpdates, 5000);
    });
}

// Функция для обработки полученных обновлений
function processUpdates(updates) {
    if (!updates || updates.length === 0) {
        return;
    }

    for (const update of updates) {
        // Обновляем offset до следующего update_id, чтобы не обрабатывать это сообщение снова
        offset = update.update_id + 1;

        if (update.message && update.message.text) {
            const chatId = update.message.chat.id;
            const receivedText = update.message.text;
            const userName = update.message.from.first_name || update.message.from.username || "User";

            console.log(`Received from ${userName} (chatId: ${chatId}): ${receivedText}`);

            const echoMessage = `echo: ${receivedText}`;
            sendMessage(chatId, echoMessage);
        }
    }
}


console.log('Бот запускается...');


// Проверяем токен
https.get(`${API_BASE_URL}/getMe`, (res) => {
    let data = '';
    res.on('data', (chunk) => { data += chunk; });
    res.on('end', () => {
        try {
            const response = JSON.parse(data);
            if (response.ok) {
                console.log(`Бот авторизован как: ${response.result.first_name} (@${response.result.username})`);
                console.log('Начинаю опрос обновлений (Long Polling)...');
                getUpdates(); // Начинаем опрос
            } else {
                console.error('Ошибка авторизации бота. Проверьте токен!');
                console.error('Ответ API:', response.description);
                process.exit(1); 
            }
        } catch(e) {
            console.error('Не удалось распарсить ответ от getMe:', e);
            process.exit(1);
        }
    });
}).on('error', (e) => {
    console.error('Не удалось подключиться к Telegram API. Проверьте интернет соединение или токен.');
    console.error(e);
    process.exit(1);
});