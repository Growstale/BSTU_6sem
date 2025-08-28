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
    healthCheckInterval,
    healthCheckTimeout,
    electionTimeout,
    maxFailures,
    coordinatorStatusFile
} = config;

const myIp = process.argv[2];
if (!myIp || !serverIps.includes(myIp)) {
    console.error(`Usage: node time_server.js <your_ip>`);
    console.error(`Available IPs in config: ${serverIps.join(', ')}`);
    process.exit(1);
}

console.log(`[${myIp}] Запуск сервера ...`);

let currentCoordinatorIp = null;
let isCoordinator = false;
let electionInProgress = false; // Идут ли сейчас выборы, инициированные этим узлом
let healthCheckTimer = null; // ID таймера для периодической проверки координатора
let failureCounter = 0;
let electionWaitTimer = null;  // ID таймера ожидания OK

const server = dgram.createSocket('udp4');


const ipCompare = (ip1, ip2) => ip1.localeCompare(ip2);

const sendMessage = (targetIp, targetPort, message) => {
    const buffer = Buffer.from(message);
    server.send(buffer, 0, buffer.length, targetPort, targetIp, (err) => {
        if (err) {
            console.error(`[${myIp}] Ошибка отправки сообщения "${message}" на ${targetIp}:${targetPort}:`, err.message);
        }
    });
};

const getCurrentTimeFormatted = () => {
    const now = new Date();
    const pad = (num) => num.toString().padStart(2, '0');
    const day = pad(now.getDate());
    const month = pad(now.getMonth() + 1);
    const year = now.getFullYear();
    const hours = pad(now.getHours());
    const minutes = pad(now.getMinutes());
    const seconds = pad(now.getSeconds());
    return `${day}${month}${year}:${hours}:${minutes}:${seconds}`;
};

const log = (message) => console.log(`[${myIp}] ${message}`);


const updateCoordinatorStatusFile = (coordinatorIp) => {
    log(`Обновляю файл состояния ${coordinatorStatusFile} -> ${coordinatorIp}`);
    try {
        const status = { coordinatorIp: coordinatorIp, timestamp: Date.now() };
        fs.writeFileSync(coordinatorStatusFile, JSON.stringify(status, null, 2));
        log(`Файл состояния ${coordinatorStatusFile} обновлен.`);
    } catch (err) {
        log(`!!! Ошибка записи в файл состояния ${coordinatorStatusFile}: ${err.message}`);
    }
};


const startElection = () => {
    if (electionInProgress || isCoordinator) {
        log(`Выборы не начаты (electionInProgress=${electionInProgress}, isCoordinator=${isCoordinator})`);
        return;
    }
    log("Обнаружен сбой координатора или выборы инициированы получением ELECTION. Начинаю выборы...");
    electionInProgress = true;
    failureCounter = 0;
    clearTimeout(healthCheckTimer); // Остановить проверку старого координатора

    const higherPeers = serverIps.filter(ip => ipCompare(ip, myIp) > 0);

    if (higherPeers.length === 0) {
        log("Нет серверов старше. Я выигрываю выборы и становлюсь координатором.");
        becomeCoordinator();
    } else {
        log(`Отправляю ELECTION старшим: ${higherPeers.join(', ')}`);
        higherPeers.forEach(peerIp => sendMessage(peerIp, port, "ELECTION"));

        clearTimeout(electionWaitTimer);
        electionWaitTimer = setTimeout(() => {
            // Если таймаут сработал И выборы все еще идут (т.е. не было получено OK)
            if (electionInProgress) {
                log("Таймаут ожидания OK. Никто из старших не ответил. Я выигрываю выборы и становлюсь координатором.");
                becomeCoordinator();
            } else {
                 log("Таймаут ожидания OK сработал, но выборы уже не идут (вероятно, был получен OK). Ничего не делаю.");
            }
        }, electionTimeout);
        log(`Таймер ожидания OK (${electionTimeout} мс) запущен.`);
    }
};

// --- СТАТЬ КООРДИНАТОРОМ ---
const becomeCoordinator = () => {
    if (isCoordinator && currentCoordinatorIp === myIp) {
        return;
    }

    isCoordinator = true;
    currentCoordinatorIp = myIp;
    log(`*********************************************`);
    log(`*** Я (${myIp}) СТАЛ КООРДИНАТОРОМ ***`);
    log(`*********************************************`);

    electionInProgress = false;
    clearTimeout(electionWaitTimer);
    failureCounter = 0;
    clearTimeout(healthCheckTimer);

    updateCoordinatorStatusFile(myIp);

    log(`Рассылаю сообщение COORDINATOR:${myIp} остальным...`);
    serverIps.forEach(peerIp => {
        if (peerIp !== myIp) {
            sendMessage(peerIp, port, `COORDINATOR:${myIp}`);
        }
    });
};

// --- ОБНОВИТЬ КООРДИНАТОРА (при получении сообщения COORDINATOR) ---
const updateCoordinator = (newCoordinatorIp) => {
    if (newCoordinatorIp === myIp) {
        // Если пришло сообщение о себе, но я не был координатором -> стать им
        if (!isCoordinator) {
             log(`Получил сообщение COORDINATOR о себе (${newCoordinatorIp}), но не был им. Становлюсь координатором.`);
             becomeCoordinator();
        }
        return; 
    }

    if (currentCoordinatorIp === newCoordinatorIp && !isCoordinator) {
        // Уже знаем этого координатора (и это не я), ничего не меняем
        return;
    }

    log(`Новый координатор объявлен: ${newCoordinatorIp}. Обновляю состояние.`);
    const wasCoordinator = isCoordinator;

    isCoordinator = false;
    currentCoordinatorIp = newCoordinatorIp; 
    electionInProgress = false;
    clearTimeout(electionWaitTimer);
    failureCounter = 0; 
    clearTimeout(healthCheckTimer); 

    // Начинаем проверять нового координатора
    scheduleHealthCheck();

    if (wasCoordinator) {
         log(`Я (${myIp}) больше не координатор.`);
    }
};

// --- ПРОВЕРКА РАБОТОСПОСОБНОСТИ КООРДИНАТОРА ---

const scheduleHealthCheck = () => {
    clearTimeout(healthCheckTimer);
    if (!isCoordinator && currentCoordinatorIp) {
        healthCheckTimer = setTimeout(checkCoordinatorHealth, healthCheckInterval);
    }
};


const checkCoordinatorHealth = () => {
    // Не проверяем, если: я координатор ИЛИ координатор неизвестен ИЛИ идут выборы
    if (isCoordinator || !currentCoordinatorIp || electionInProgress) {
        return;
    }

    const pingMessage = `PING:${myIp}`;
    sendMessage(currentCoordinatorIp, port, pingMessage);

    const pongTimeout = setTimeout(() => {
        if (!isCoordinator && currentCoordinatorIp && !electionInProgress) {
             failureCounter++;
             log(`Координатор ${currentCoordinatorIp} НЕ ОТВЕТИЛ на PING. Ошибок: ${failureCounter}/${maxFailures}`);
             if (failureCounter >= maxFailures) {
                 log(`Координатор ${currentCoordinatorIp} считается упавшим. Забываю его.`);
                 const failedCoordinator = currentCoordinatorIp;
                 currentCoordinatorIp = null; // Забываем старого координатора
                 startElection(); // НАЧИНАЕМ ВЫБОРЫ
             } else {
                 scheduleHealthCheck(); 
             }
        }
    }, healthCheckTimeout);

    // Временный слушатель для PONG
    const pongListener = (msg, rinfo) => {
        const message = msg.toString();
        if (rinfo.address === currentCoordinatorIp && message === `PONG:${myIp}`) {
            clearTimeout(pongTimeout);
            server.removeListener('message', pongListener); 
            if (failureCounter > 0) {
                log(`Координатор ${currentCoordinatorIp} снова отвечает. Сброс ошибок.`);
                failureCounter = 0; 
            }
            scheduleHealthCheck(); 
        }
    };

    server.on('message', pongListener);
    setTimeout(() => server.removeListener('message', pongListener), healthCheckTimeout + 200);
};


// --- ОБРАБОТКА ВХОДЯЩИХ СООБЩЕНИЙ ---
server.on('message', (msg, rinfo) => {
    const message = msg.toString();
    const senderIp = rinfo.address;
    const senderPort = rinfo.port;

    // Игнорируем сообщения от самих себя
    if (senderIp === myIp && senderPort === port) {
        return;
    }

    if (message === 'ELECTION') {
        log(`Получено сообщение ELECTION от ${senderIp}`);
        if (ipCompare(myIp, senderIp) > 0) {
            log(`Отправляю OK на ${senderIp}`);
            sendMessage(senderIp, senderPort, "OK");
            startElection();
        } else {
             if(electionInProgress && electionWaitTimer) {
                 log(`Получен ELECTION от старшего (${senderIp}), пока я ждал OK. Прерываю ожидание OK.`);
                 clearTimeout(electionWaitTimer);
             }
        }
    } else if (message === 'OK') {
        log(`Получено сообщение OK от ${senderIp}. Он (${senderIp}) старше и будет проводить выборы.`);
        if (electionInProgress) {
            log("Прекращаю свои выборы (был инициатором), так как получен OK. Жду объявления нового координатора.");
            electionInProgress = false; 
            clearTimeout(electionWaitTimer); 
        }
    }
    else if (message.startsWith('COORDINATOR:')) {
        const newCoordinatorIp = message.split(':')[1];
        if (serverIps.includes(newCoordinatorIp)){
             log(`Получено объявление координатора: ${newCoordinatorIp}`);
             updateCoordinator(newCoordinatorIp); 
        } else {
             log(`Получен COORDINATOR с неизвестным IP: ${newCoordinatorIp}. Игнорирую.`);
        }
    }
    else if (message.startsWith('PING:')) {
        const requesterIp = message.split(':')[1];
        if (isCoordinator) {
            sendMessage(senderIp, senderPort, `PONG:${requesterIp}`);
        }
        // Если не координатор, PING игнорируется (слушатель pongListener его не поймает)
    } else if (message.startsWith('PONG:')) {
        // PONG обрабатывается в pongListener внутри checkCoordinatorHealth
    }
    else if (message === 'GET_TIME') {
        if (isCoordinator) {
            const currentTime = getCurrentTimeFormatted();
            log(`Получен GET_TIME от медиатора ${senderIp}:${senderPort}. Отправляю время: ${currentTime}`);
            sendMessage(senderIp, senderPort, currentTime);
        } else {
             log(`Получен GET_TIME, но я не координатор (${currentCoordinatorIp}). Игнорирую.`);
        }
    }
    else {
        log(`Получено неизвестное или непредусмотренное сообщение: "${message}" от ${senderIp}:${senderPort}`);
    }
});


server.on('listening', () => {
    const address = server.address();
    log(`Сервер СВВ слушает на ${address.address}:${address.port}`);

    log("Применяю логику забияки при старте...");

    let coordinatorFromFile = null;
    try {
        if (fs.existsSync(coordinatorStatusFile)) {
            const data = fs.readFileSync(coordinatorStatusFile, 'utf8');
            const status = JSON.parse(data);
            if (status && status.coordinatorIp && serverIps.includes(status.coordinatorIp)) {
                coordinatorFromFile = status.coordinatorIp;
                log(`Обнаружен координатор в файле состояния: ${coordinatorFromFile}`);
            } else {
                log(`Файл состояния ${coordinatorStatusFile} невалиден.`);
            }
        } else {
            log(`Файл состояния ${coordinatorStatusFile} не найден.`);
        }
    } catch(err) {
        log(`Ошибка чтения файла состояния при старте: ${err.message}`);
        coordinatorFromFile = null;
    }

    // Если Я старше координатора из файла ИЛИ файла нет -> Я должен стать координатором
    if (coordinatorFromFile === null || ipCompare(myIp, coordinatorFromFile) > 0) {
        if (coordinatorFromFile !== null) {
            log(`Мой IP (${myIp}) старше координатора из файла (${coordinatorFromFile}). Захватываю роль!`);
        } else {
            log(`Нет активного координатора в файле состояния. Объявляю себя координатором.`);
        }
        becomeCoordinator(); 
    }
    // Если Я младше координатора из файла -> Начать его проверять
    else if (ipCompare(myIp, coordinatorFromFile) < 0) {
        log(`Мой IP (${myIp}) младше координатора из файла (${coordinatorFromFile}). Начинаю его проверять.`);
        currentCoordinatorIp = coordinatorFromFile;
        scheduleHealthCheck();
    }
    // Если Я равен координатору из файла (восстановление) -> Подтвердить роль
    else { // myIp === coordinatorFromFile
        log(`Мой IP (${myIp}) совпадает с координатором из файла. Восстанавливаю роль координатора.`);
        becomeCoordinator(); 
    }
});


server.on('error', (err) => {
    log(`Критическая ошибка сервера: ${err.stack}`);
    server.close();
    process.exit(1);
});

server.bind(port, myIp);


process.on('SIGINT', () => {
    log('Получен SIGINT. Завершение работы...');
    clearTimeout(healthCheckTimer);
    clearTimeout(electionWaitTimer);
    server.close(() => {
        log('Сервер остановлен.');
        process.exit(0);
    });
});