const nodemailer = require('nodemailer');

function sendMail(message, password) {
    const transporter = nodemailer.createTransport({
        service: 'gmail',
        auth: {
            user: 'cleo2005254@gmail.com',
            pass: password,
        },
    });

    const mailOptions = {
        from: 'cleo2005254@gmail.com',
        to: 'vodchytsanastasiya@gmail.com',
        subject: 'Сообщение с формы',
        text: message,
    };

    transporter.sendMail(mailOptions, (error, info) => {
        if (error) {
            console.error(`Ошибка: ${error.message}`);
        } else {
            console.log('Письмо отправлено!');
        }
    });
}

module.exports = sendMail;
// делаем функцию sendMail доступной для использования в других файлах (модулях) проекта