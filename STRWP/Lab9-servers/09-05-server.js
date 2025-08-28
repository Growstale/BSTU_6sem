const http = require('http');
const xml2js = require('xml2js'); // Подключаем библиотеку для работы с XML

const PORT = 3000;

// Вспомогательная функция для создания XML-ответа
const buildXMLResponse = (data) => {
    const builder = new xml2js.Builder({ rootName: 'response' });
    return builder.buildObject(data); // Строим XML из объекта JavaScript
};

const server = http.createServer(async (req, res) => {
    console.log(`Получен запрос: ${req.method} ${req.url}`);

    if (req.method !== 'POST') {
        res.writeHead(405, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Метод не разрешен. Используйте POST.');
        console.log('Отправлен ответ 405 Method Not Allowed');
        return;
    }

    const contentType = req.headers['content-type'];
    // Проверяем, содержит ли Content-Type указание на XML
    if (!contentType || !(contentType.includes('application/xml') || contentType.includes('text/xml'))) {
        res.writeHead(415, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Неподдерживаемый тип данных. Ожидается application/xml или text/xml.');
        console.log('Отправлен ответ 415 Unsupported Media Type');
        return;
    }

    let requestBodyXml = '';
    req.on('data', chunk => { 
        requestBodyXml += chunk.toString();
    });

    req.on('end', async () => { 
        console.log('Получено тело XML запроса:\n', requestBodyXml);

        try {
            // 3. Парсим (разбираем) XML запрос
            const parser = new xml2js.Parser({ explicitArray: false, explicitRoot: true });
            const parsedData = await parser.parseStringPromise(requestBodyXml); // Асинхронный парсинг

            const requestContent = parsedData.request; // корневой элемент
            console.log('Распарсенный JS объект:', requestContent);

            if (!requestContent || typeof requestContent.x === 'undefined' || typeof requestContent.y === 'undefined' ||
                typeof requestContent.s === 'undefined' || !requestContent.m || !requestContent.o) {
                throw new Error('Неверная структура XML запроса.');
            }

            const x = parseInt(requestContent.x, 10); 
            const y = parseInt(requestContent.y, 10);
            const s = requestContent.s; 
            const m_items = Array.isArray(requestContent.m.item)
                            ? requestContent.m.item
                            : (requestContent.m.item ? [requestContent.m.item] : []);
            const o = requestContent.o; 

            if (isNaN(x) || isNaN(y) || typeof s !== 'string' || !Array.isArray(m_items) || typeof o !== 'object' || !o.surname || !o.name) {
                throw new Error('Неверные типы данных в XML запросе.');
            }

            const sum = x + y;
            const concat = `${s}: ${o.surname}, ${o.name}`;
            const len = m_items.length;

            const responseObject = {
                _comment: "Ответ.Лабораторная работа 8/10",
                x_plus_y: sum,
                Concatinatiоn_s_o: concat,
                Length_m: len
            };
            console.log('Объект JS для ответа:', responseObject);

            const responseXml = buildXMLResponse(responseObject);
            console.log('Отправляемый XML ответ:\n', responseXml);

            res.writeHead(200, { 'Content-Type': 'application/xml; charset=utf-8' });
            res.end(responseXml); 

        } catch (error) {
            console.error('Ошибка обработки запроса:', error);
            res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end(`Ошибка обработки запроса: ${error.message}`);
        }
    });

    req.on('error', (err) => {
        console.error('Ошибка запроса:', err);
        res.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Внутренняя ошибка сервера при чтении запроса.');
    });
});

server.listen(PORT, () => {
    console.log(`Сервер запущен и слушает порт ${PORT}`);
    console.log(`Ожидает POST запросы с XML на http://localhost:${PORT}/`);
});