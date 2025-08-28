const http = require('http');
const fs = require('fs');
const qs = require('querystring');
const nodemailer = require('nodemailer');

http.createServer((req, res) => {
    if (req.method === 'GET') {
        fs.readFile('Lab06-02.html', (err, data) => {
            res.writeHead(200, { "Content-Type": "text/html; charset=utf-8" });
            res.end(err ? 'Ошибка загрузки страницы' : data);
        });
    } else if (req.method === 'POST' && req.url === '/send') {
        let body = '';
        req.on('data', chunk => body += chunk.toString());
        req.on('end', () => {
            const formData = qs.parse(body); // Тело запроса парсится в JavaScript-объект
            const { from, appPassword, to, message } = formData;

            if (!from || !appPassword || !to || !message) {
                res.writeHead(400, { "Content-Type": "text/html; charset=utf-8" });
                res.end('Ошибка: все поля должны быть заполнены');
                return;
            }

            const transporter = nodemailer.createTransport({
                service: 'gmail',
                auth: {
                    user: from,
                    pass: appPassword
                }
            });

            const mailOptions = {
                from: from,
                to: to,
                subject: 'Сообщение с формы',
                text: message
            };

            transporter.sendMail(mailOptions, (error, info) => {
                res.writeHead(200, { "Content-Type": "text/html; charset=utf-8" });
                res.end(error ? `Ошибка: ${error.message}` : 'Письмо отправлено!');
            });
        });
    } else {
        res.writeHead(404, { "Content-Type": "text/html; charset=utf-8" });
        res.end('Страница не найдена');
    }
}).listen(3000, () => console.log("Сервер запущен на порту 3000"));
