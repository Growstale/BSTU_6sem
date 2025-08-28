const dgram = require('dgram');
const fs = require('fs');

let config;
try {
    config = JSON.parse(fs.readFileSync('config.json', 'utf8'));
} catch (err) {
    console.error("Ошибка чтения config.json:", err);
    process.exit(1);
}

const { port, mediatorIp } = config;
const message = Buffer.from("TIME?");

const client = dgram.createSocket('udp4');

client.on('message', (msg, rinfo) => {
    console.log(`Получено время от сервера ${rinfo.address}:${rinfo.port}: ${msg.toString()}`);
    client.close();
});

client.on('close', () => {
    console.log('Клиент закрыт.');
});

client.on('error', (err) => {
    console.error(`Ошибка клиента: ${err.stack}`);
    client.close();
});

console.log(`Отправка запроса времени посреднику на ${mediatorIp}:${port}`);
client.send(message, port, mediatorIp, (err) => {
    if (err) {
        console.error('Ошибка отправки:', err);
        client.close();
    }
});

setTimeout(() => {
    try {
        client.close(); 
        console.log("Клиент закрыт по таймауту.");
    } catch (e) { /* ignore */ }
}, 5000); 