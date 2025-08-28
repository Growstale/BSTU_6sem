const http = require('http'); 
const xml2js = require('xml2js'); // Подключаем библиотеку для работы с XML

const requestData = {
    request: {
        _comment: "Запрос.Лабораторная работа 8/10",
        x: 1,
        y: 2,
        s: "Сообщение",
        m: { item: ["a", "b", "c", "d"] },
        o: { surname: "Иванов", name: "Иван" }
    }
};

const builder = new xml2js.Builder(); // Создаем экземпляр Builder
const xmlRequestData = builder.buildObject(requestData); // Строим XML из объекта


const options = {
    hostname: 'localhost', 
    port: 3000,            
    path: '/',         
    method: 'POST',     
    headers: {
        'Content-Type': 'application/xml', 
        // 'Content-Length' важен для POST-запросов. Вычисляем длину тела запроса в байтах.
        'Content-Length': Buffer.byteLength(xmlRequestData),
        // Сообщаем серверу, что мы предпочитаем получить ответ в формате XML
        'Accept': 'application/xml'
    }
};

const req = http.request(options, (res) => { 

    let responseBodyXml = ''; 
    res.setEncoding('utf8');

    res.on('data', (chunk) => { 
        responseBodyXml += chunk; 
    });

    res.on('end', async () => { 
        if (res.statusCode === 200 && responseBodyXml) {
             try {
                const parser = new xml2js.Parser({ explicitArray: false }); // Создаем парсер
                // Парсим XML-строку ответа
                const parsedResponse = await parser.parseStringPromise(responseBodyXml);
                console.log(JSON.stringify(parsedResponse.response, null, 2));
             } catch (parseError) {
                console.error('\nОшибка парсинга XML ответа:', parseError);
                console.log('Не удалось распарсить тело ответа.');
             }
        } else if (responseBodyXml) {
            console.log('\nТело ответа (Текст):', responseBodyXml);
        } else {
            console.log('\nОтвет не содержит тела.');
        }
    });
});

req.on('error', (e) => {
    console.error(`\n--- ОШИБКА ЗАПРОСА ---`);
    console.error(`Проблема с запросом: ${e.message}`);
    console.error('Убедитесь, что сервер запущен на http://localhost:3000');
    console.log('-------------------------\n');
});

// Записываем данные (XML) в тело запроса
req.write(xmlRequestData);
req.end();