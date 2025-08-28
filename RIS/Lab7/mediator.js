const dgram = require('dgram');
const fs = require('fs');

let config;
try {
    config = JSON.parse(fs.readFileSync('config.json', 'utf8'));
} catch (err) {
    console.error("Ошибка чтения config.json:", err);
    process.exit(1);
}

const {
    port,
    serverIps,
    mediatorIp,
    coordinatorStatusFile,
    mediatorCheckInterval 
} = config;
const requestTimeout = 3000; // Таймаут ожидания ответа от координатора

let currentCoordinatorIp = null;
let statusCheckIntervalId = null; 

console.log(`[Mediator] Запуск сервера-посредника на ${mediatorIp}:${port}...`);
console.log(`[Mediator] Файл состояния координатора: ${coordinatorStatusFile}`);
console.log(`[Mediator] Интервал проверки файла: ${mediatorCheckInterval} мс`);

const server = dgram.createSocket('udp4');
const clientRequests = new Map(); // Хранение информации об ожидающих клиентах (Key: uniqueClientKey, Value: {clientAddress, clientPort, timer})

const log = (message) => console.log(`[Mediator] ${message}`);


const readAndUpdateCoordinatorIp = () => {
    let foundIp = null;
    try {
        if (!fs.existsSync(coordinatorStatusFile)) {
            return; 
        }

        const data = fs.readFileSync(coordinatorStatusFile, 'utf8');
        const status = JSON.parse(data);

        if (status && status.coordinatorIp && serverIps.includes(status.coordinatorIp)) {
             foundIp = status.coordinatorIp;
        } else {
            log(`!!! Ошибка: Некорректные данные в файле состояния ${coordinatorStatusFile} или IP нет в списке серверов.`);
            return;
        }
    } catch (err) {
        log(`!!! Ошибка чтения или парсинга файла состояния ${coordinatorStatusFile}: ${err.message}`);
        return;
    }

    // Обновляем IP только если он изменился
    if (foundIp && currentCoordinatorIp !== foundIp) {
         log(`Обнаружен новый координатор в файле: ${foundIp} (был ${currentCoordinatorIp})`);
         currentCoordinatorIp = foundIp;
    } else if (!currentCoordinatorIp && foundIp) {
         log(`Координатор инициализирован из файла: ${foundIp}`);
         currentCoordinatorIp = foundIp;
    }
};


server.on('message', (msg, rinfo) => {
    const senderIp = rinfo.address;
    const senderPort = rinfo.port;
    const messageStr = msg.toString();

    // Это ответ от сервера?
    if (serverIps.includes(senderIp) && senderPort === port) {

        if (clientRequests.size > 0) {
            // Берем первый ключ из итератора Map
            const firstClientKey = clientRequests.keys().next().value;
            const requestInfo = clientRequests.get(firstClientKey);

            if (requestInfo) {
                 clearTimeout(requestInfo.timer); // Отменяем таймаут
                 clientRequests.delete(firstClientKey); // Удаляем из ожидающих

                 // Отправляем ответ исходному клиенту
                 sendMessage(requestInfo.clientAddress, requestInfo.clientPort, messageStr);
            } else {
                 log(`!!! Ошибка: Не найдена информация для клиента с ключом ${firstClientKey}`);
                 clientRequests.delete(firstClientKey);
            }

        } else {
            log(`Получен ответ от СВВ ${senderIp}, но нет ожидающих клиентов.`);
        }

    // Запрос от клиента
    } else {
        const clientAddress = senderIp;
        const clientPort = senderPort;
        const clientKey = `${clientAddress}:${clientPort}_${Date.now()}_${Math.random()}`;

        log(`Получен запрос от клиента ${clientAddress}:${clientPort}`);

        if (!currentCoordinatorIp) {
            log(`Ошибка: Координатор в данный момент неизвестен (проверьте файл ${coordinatorStatusFile}). Не могу перенаправить запрос.`);
            sendMessage(clientAddress, clientPort, "ERROR: Coordinator unknown or status file invalid");
            return; // Важно: выходим, не обрабатываем дальше
        }

        log(`Перенаправляю запрос на текущего координатора (${currentCoordinatorIp}:${port})`);

        // Формируем запрос к координатору
        const requestPayload = Buffer.from("GET_TIME");

        // Сохраняем информацию о клиенте, чтобы знать, кому вернуть ответ
        const requestInfo = {
            clientAddress: clientAddress,
            clientPort: clientPort,
            timer: setTimeout(() => {
                if (clientRequests.has(clientKey)) { 
                    log(`!!! Ошибка: Таймаут ожидания ответа от координатора ${currentCoordinatorIp} для клиента ${clientAddress}:${clientPort}`);
                    const timedOutRequest = clientRequests.get(clientKey);
                    clientRequests.delete(clientKey);
                    if (timedOutRequest) { 
                         sendMessage(timedOutRequest.clientAddress, timedOutRequest.clientPort, "ERROR: Service timeout");
                    }
                }
            }, requestTimeout)
        };
        clientRequests.set(clientKey, requestInfo); // Используем уникальный clientKey

        // Отправляем запрос координатору
        server.send(requestPayload, 0, requestPayload.length, port, currentCoordinatorIp, (err) => {
            if (err) {
                log(`!!! Ошибка отправки запроса координатору ${currentCoordinatorIp}: ${err.message}`);
                if (clientRequests.has(clientKey)) {
                     const failedRequest = clientRequests.get(clientKey);
                     if(failedRequest){
                         clearTimeout(failedRequest.timer);
                         sendMessage(failedRequest.clientAddress, failedRequest.clientPort, "ERROR: Failed to contact service");
                     }
                     clientRequests.delete(clientKey); 
                }
            }
        });
    }
});

// --- Вспомогательная функция для отправки ---
const sendMessage = (targetIp, targetPort, message) => {
    const buffer = Buffer.from(message);
    server.send(buffer, 0, buffer.length, targetPort, targetIp, (err) => {
        if (err) {
            if (!serverIps.includes(targetIp)) {
                 log(`Ошибка отправки сообщения "${message}" на ${targetIp}:${targetPort}: ${err}`);
            } else {
            }
        }
    });
};

// --- Запуск сервера ---
server.on('listening', () => {
    const address = server.address();
    log(`Сервер-посредник слушает на ${address.address}:${address.port}`);

    // --- Первоначальное чтение файла и запуск интервала ---
    log("Первоначальная проверка файла состояния координатора...");
    readAndUpdateCoordinatorIp(); 

    if (!currentCoordinatorIp) {
        log(`!!! Внимание: Координатор не определен после первого чтения файла. Запросы не будут перенаправляться до появления файла.`);
    } else {
        log(`Начальный координатор из файла: ${currentCoordinatorIp}`);
    }

    // Запустить периодическую проверку файла
    statusCheckIntervalId = setInterval(readAndUpdateCoordinatorIp, mediatorCheckInterval);
    log(`Запущена периодическая проверка файла ${coordinatorStatusFile} каждые ${mediatorCheckInterval} мс`);

});

server.on('error', (err) => {
    log(`Ошибка сервера-посредника: ${err.stack}`);
    if (statusCheckIntervalId) clearInterval(statusCheckIntervalId);
    server.close();
    process.exit(1);
});

server.bind(port, mediatorIp);

// Обработка завершения работы
process.on('SIGINT', () => {
    log('Получен SIGINT. Завершение работы...');
    if (statusCheckIntervalId) {
        clearInterval(statusCheckIntervalId); 
        log('Остановлена проверка файла состояния.');
    }
    clientRequests.forEach(req => clearTimeout(req.timer)); 
    clientRequests.clear(); 
    server.close(() => {
        log('Сервер-посредник остановлен.');
        process.exit(0);
    });
});