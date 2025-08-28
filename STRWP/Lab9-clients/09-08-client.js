const http = require('http');
const fs = require('fs');
const path = require('path');
const url = require('url');

const SERVER_URL = `http://localhost:3002/download`; 
const DOWNLOAD_DIR = path.join(__dirname, 'downloads_client');
const DEFAULT_SAVE_FILENAME = 'received_file.png'; // Имя файла по умолчанию

// --- Создаем папку для загрузок, если её нет ---
if (!fs.existsSync(DOWNLOAD_DIR)) {
    try {
        fs.mkdirSync(DOWNLOAD_DIR);
        console.log(`Папка для сохранения создана: ${DOWNLOAD_DIR}`);
    } catch (err) {
        console.error(`Не удалось создать папку для сохранения "${DOWNLOAD_DIR}":`, err);
        process.exit(1);
    }
} else {
     console.log(`Папка для сохранения существует: ${DOWNLOAD_DIR}`);
}


console.log(`Отправка GET запроса на ${SERVER_URL}...`);

const request = http.get(SERVER_URL, (res) => {
    if (res.statusCode !== 200) {
        console.error(`Ошибка на сервере: Статус ${res.statusCode}`);
        res.resume();
        return;
    }

    // --- Определение имени файла для сохранения ---
    let saveFilename = DEFAULT_SAVE_FILENAME;
    const contentDisposition = res.headers['content-disposition'];
    if (contentDisposition) {
        // Простой парсинг заголовка Content-Disposition
        const filenameMatch = contentDisposition.match(/filename="([^"]+)"/i);
        if (filenameMatch && filenameMatch[1]) {
            saveFilename = path.basename(filenameMatch[1]); 
            console.log(`Имя файла из заголовка Content-Disposition: "${saveFilename}"`);
        } else {
            console.log(`Не удалось извлечь имя файла из Content-Disposition, используется имя по умолчанию: "${saveFilename}"`);
        }
    } else {
        console.log(`Заголовок Content-Disposition не найден, используется имя по умолчанию: "${saveFilename}"`);
    }

    const savePath = path.join(DOWNLOAD_DIR, saveFilename);
    console.log(`Файл будет сохранен как: ${savePath}`);

    // --- Создание потока для записи файла ---
    const fileStream = fs.createWriteStream(savePath);

    let receivedBytes = 0;
    const totalBytes = res.headers['content-length'] ? parseInt(res.headers['content-length'], 10) : null;


    // --- Получение данных и запись в файл ---
    res.on('data', (chunk) => {
        receivedBytes += chunk.length;
        if(totalBytes) {
            const percent = ((receivedBytes / totalBytes) * 100).toFixed(2);
            // Выводим прогресс в ту же строку
            process.stdout.write(`\rЗагружено: ${receivedBytes} / ${totalBytes} байт (${percent}%)`);
        } else {
            process.stdout.write(`\rЗагружено: ${receivedBytes} байт`);
        }
    });


    // --- Перенаправление потока ответа в файловый поток ---
    res.pipe(fileStream);

    fileStream.on('finish', () => {
        fileStream.close(() => { 
            process.stdout.write('\n'); 
            console.log(`\nФайл успешно скачан и сохранен как "${savePath}"`);
            console.log(`Размер файла: ${fs.statSync(savePath).size} байт.`);
        });
    });

    fileStream.on('error', (err) => {
        process.stdout.write('\n'); 
        console.error(`\nОшибка при записи файла "${savePath}":`, err);
        fs.unlink(savePath, (unlinkErr) => {
            if (unlinkErr) console.error(`Ошибка при удалении частичного файла "${savePath}":`, unlinkErr);
        });
    });

    res.on('error', (err) => {
        process.stdout.write('\n');
         console.error('\nОшибка при получении ответа от сервера:', err);
    });

});

request.on('error', (e) => {
    console.error(`\nОшибка при выполнении GET запроса к ${SERVER_URL}: ${e.message}`);
});

request.end();