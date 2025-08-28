const express = require('express');
const { Sequelize, DataTypes } = require('sequelize');
const path = require('path'); // Для отправки HTML файла

const app = express();
const PORT = 3000;

// --- Middleware ---
app.use(express.json()); // Встроенный парсер JSON
app.use(express.urlencoded({ extended: true })); // Для URL-encoded данных (на всякий случай)

// --- Конфигурация Sequelize ---
const sequelize = new Sequelize('VAV', 'NodeAppUser', 'password', {
    host: 'VODCHYTS', // Имя вашего сервера
    port: 1433, // <--- Добавляем порт
    dialect: 'mssql',   // Явно указываем диалект MS SQL Server
    dialectOptions: {
        options: { // <--- Убедитесь в этом уровне вложенности
             encrypt: false,
             trustServerCertificate: true // Оставляем на всякий случай
        }
    },
    pool: { // Конфигурация пула соединений
        max: 5,     // Макс. количество соединений в пуле
        min: 0,     // Мин. количество соединений в пуле
        acquire: 30000, // Макс. время (мс) попытки получить соединение перед ошибкой
        idle: 10000     // Макс. время (мс) простоя соединения перед его освобождением
    },
    logging: console.log // Выводить SQL-запросы в консоль (можно убрать или заменить)
});

// --- Определение моделей Sequelize ---
// Важно: имена полей и таблиц должны совпадать с БД (регистрозависимо для некоторых БД)

const Faculty = sequelize.define('Faculty', {
    FACULTY: { type: DataTypes.STRING(10), primaryKey: true, allowNull: false },
    FACULTY_NAME: { type: DataTypes.STRING(100), allowNull: true } // Разрешим NULL для имени
}, { tableName: 'FACULTY', timestamps: false });

const Pulpit = sequelize.define('Pulpit', {
    PULPIT: { type: DataTypes.STRING(10), primaryKey: true, allowNull: false },
    PULPIT_NAME: { type: DataTypes.STRING(100), allowNull: false },
    FACULTY: { type: DataTypes.STRING(10), allowNull: false }
}, { tableName: 'PULPIT', timestamps: false });

const Subject = sequelize.define('Subject', {
    SUBJECT: { type: DataTypes.STRING(10), primaryKey: true, allowNull: false },
    SUBJECT_NAME: { type: DataTypes.STRING(100), allowNull: false },
    PULPIT: { type: DataTypes.STRING(10), allowNull: false }
}, { tableName: 'SUBJECT', timestamps: false });

const AuditoriumType = sequelize.define('Auditorium_Type', { // Имя модели может отличаться от таблицы
    AUDITORIUM_TYPE: { type: DataTypes.STRING(10), primaryKey: true, allowNull: false },
    AUDITORIUM_TYPENAME: { type: DataTypes.STRING(100), allowNull: false }
}, { tableName: 'AUDITORIUM_TYPE', timestamps: false });

const Auditorium = sequelize.define('Auditorium', {
    AUDITORIUM: { type: DataTypes.STRING(10), primaryKey: true, allowNull: false },
    AUDITORIUM_NAME: { type: DataTypes.STRING(100), allowNull: true }, // Разрешим NULL для имени
    AUDITORIUM_CAPACITY: { type: DataTypes.INTEGER, allowNull: false },
    AUDITORIUM_TYPE: { type: DataTypes.STRING(10), allowNull: false }
}, { tableName: 'AUDITORIUM', timestamps: false });

// --- Определение связей (для возможных будущих запросов с JOIN) ---
// Faculty <-> Pulpit
Faculty.hasMany(Pulpit, { foreignKey: 'FACULTY', sourceKey: 'FACULTY' });
Pulpit.belongsTo(Faculty, { foreignKey: 'FACULTY', targetKey: 'FACULTY' });

// Pulpit <-> Subject
Pulpit.hasMany(Subject, { foreignKey: 'PULPIT', sourceKey: 'PULPIT' });
Subject.belongsTo(Pulpit, { foreignKey: 'PULPIT', targetKey: 'PULPIT' });

// AuditoriumType <-> Auditorium
AuditoriumType.hasMany(Auditorium, { foreignKey: 'AUDITORIUM_TYPE', sourceKey: 'AUDITORIUM_TYPE' });
Auditorium.belongsTo(AuditoriumType, { foreignKey: 'AUDITORIUM_TYPE', targetKey: 'AUDITORIUM_TYPE' });


// --- Обработчик ошибок Sequelize ---
const handleSequelizeError = (err, res) => {
    console.error('Sequelize Error:', err);
    // Попытка извлечь более конкретное сообщение об ошибке
    let message = 'Database error';
    if (err.original && err.original.message) {
        message = err.original.message;
    } else if (err.message) {
        message = err.message;
    }
    // Определяем статус код (например, 400 для ошибок валидации/ограничений, 500 для других)
    // Sequelize может выбрасывать ValidationError, ForeignKeyConstraintError и др.
    let statusCode = 500;
    if (err.name && (err.name.includes('Validation') || err.name.includes('Constraint'))) {
        statusCode = 400; // Bad Request
    }
     // Если ошибка из-за дубликата первичного ключа (код 2627 для MS SQL)
     if (err.original && err.original.number === 2627) {
        statusCode = 409; // Conflict
        message = "Record with this primary key already exists.";
    }
    res.status(statusCode).json({ error: message });
};


// --- Маршруты ---

// GET / - Статическая HTML страница
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'index.html'));
});

// --- CRUD для Faculty ---
app.get('/api/faculties', async (req, res) => {
    try {
        const faculties = await Faculty.findAll();
        res.json(faculties);
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

app.post('/api/faculties', async (req, res) => {
    try {
        const newFaculty = await Faculty.create(req.body);
        res.status(201).json(newFaculty); // 201 Created
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

app.put('/api/faculties', async (req, res) => {
    try {
        const { FACULTY, FACULTY_NAME } = req.body;
        if (!FACULTY) {
            return res.status(400).json({ error: 'FACULTY code is required for update.' });
        }
        const [updatedRows] = await Faculty.update(
            { FACULTY_NAME }, // Поля для обновления
            { where: { FACULTY } } // Условие поиска
        );
        if (updatedRows > 0) {
            const updatedFaculty = await Faculty.findByPk(FACULTY);
            res.json(updatedFaculty);
        } else {
            res.status(404).json({ error: `Faculty with code ${FACULTY} not found.` });
        }
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

app.delete('/api/faculties/:facultyCode', async (req, res) => {
    try {
        const facultyCode = req.params.facultyCode;
        const deletedRows = await Faculty.destroy({
            where: { FACULTY: facultyCode }
        });
        if (deletedRows > 0) {
            res.json({ message: `Faculty ${facultyCode} deleted successfully.`}); // или status 204 .end()
        } else {
            res.status(404).json({ error: `Faculty with code ${facultyCode} not found.` });
        }
    } catch (err) {
        handleSequelizeError(err, res);
    }
});


// --- CRUD для Pulpit ---
app.get('/api/pulpits', async (req, res) => {
    try {
        const pulpits = await Pulpit.findAll({ include: [{ model: Faculty, attributes: ['FACULTY_NAME'] }] }); // Пример JOIN
        res.json(pulpits);
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

app.post('/api/pulpits', async (req, res) => {
    try {
        const newPulpit = await Pulpit.create(req.body);
        res.status(201).json(newPulpit);
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

app.put('/api/pulpits', async (req, res) => {
    try {
        const { PULPIT, ...updateData } = req.body; // Отделяем PK от данных для обновления
        if (!PULPIT) {
            return res.status(400).json({ error: 'PULPIT code is required for update.' });
        }
        const [updatedRows] = await Pulpit.update(updateData, { where: { PULPIT } });
        if (updatedRows > 0) {
            const updatedPulpit = await Pulpit.findByPk(PULPIT);
            res.json(updatedPulpit);
        } else {
            res.status(404).json({ error: `Pulpit with code ${PULPIT} not found.` });
        }
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

app.delete('/api/pulpits/:pulpitCode', async (req, res) => {
     try {
        const pulpitCode = req.params.pulpitCode;
        const deletedRows = await Pulpit.destroy({ where: { PULPIT: pulpitCode } });
        if (deletedRows > 0) {
            res.json({ message: `Pulpit ${pulpitCode} deleted successfully.`});
        } else {
            res.status(404).json({ error: `Pulpit with code ${pulpitCode} not found.` });
        }
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

// --- CRUD для Subject ---
app.get('/api/subjects', async (req, res) => {
    try {
        const subjects = await Subject.findAll({ include: [{ model: Pulpit, attributes: ['PULPIT_NAME'] }] }); // Пример JOIN
        res.json(subjects);
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

app.post('/api/subjects', async (req, res) => {
     try {
        const newSubject = await Subject.create(req.body);
        res.status(201).json(newSubject);
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

app.put('/api/subjects', async (req, res) => {
     try {
        const { SUBJECT, ...updateData } = req.body;
        if (!SUBJECT) {
            return res.status(400).json({ error: 'SUBJECT code is required for update.' });
        }
        const [updatedRows] = await Subject.update(updateData, { where: { SUBJECT } });
        if (updatedRows > 0) {
            const updatedSubject = await Subject.findByPk(SUBJECT);
            res.json(updatedSubject);
        } else {
            res.status(404).json({ error: `Subject with code ${SUBJECT} not found.` });
        }
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

app.delete('/api/subjects/:subjectCode', async (req, res) => {
      try {
        const subjectCode = req.params.subjectCode;
        const deletedRows = await Subject.destroy({ where: { SUBJECT: subjectCode } });
        if (deletedRows > 0) {
            res.json({ message: `Subject ${subjectCode} deleted successfully.`});
        } else {
            res.status(404).json({ error: `Subject with code ${subjectCode} not found.` });
        }
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

// --- CRUD для AuditoriumType ---
app.get('/api/auditoriumstypes', async (req, res) => {
    try {
        const types = await AuditoriumType.findAll();
        res.json(types);
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

app.post('/api/auditoriumstypes', async (req, res) => {
     try {
        const newType = await AuditoriumType.create(req.body);
        res.status(201).json(newType);
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

app.put('/api/auditoriumstypes', async (req, res) => {
    try {
        const { AUDITORIUM_TYPE, AUDITORIUM_TYPENAME } = req.body;
        if (!AUDITORIUM_TYPE) {
            return res.status(400).json({ error: 'AUDITORIUM_TYPE code is required for update.' });
        }
        const [updatedRows] = await AuditoriumType.update(
            { AUDITORIUM_TYPENAME },
            { where: { AUDITORIUM_TYPE } }
        );
        if (updatedRows > 0) {
            const updatedType = await AuditoriumType.findByPk(AUDITORIUM_TYPE);
            res.json(updatedType);
        } else {
            res.status(404).json({ error: `Auditorium type with code ${AUDITORIUM_TYPE} not found.` });
        }
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

// Обратите внимание на :typeCode в URL
app.delete('/api/auditoriumtypes/:typeCode', async (req, res) => {
    try {
        const typeCode = req.params.typeCode;
        const deletedRows = await AuditoriumType.destroy({ where: { AUDITORIUM_TYPE: typeCode } });
        if (deletedRows > 0) {
            res.json({ message: `Auditorium type ${typeCode} deleted successfully.`});
        } else {
            res.status(404).json({ error: `Auditorium type with code ${typeCode} not found.` });
        }
    } catch (err) {
        handleSequelizeError(err, res);
    }
});


// --- CRUD для Auditorium ---
// Исправляем URL на /api/auditoriums
app.get('/api/auditoriums', async (req, res) => {
    try {
        const auditoriums = await Auditorium.findAll({ include: [{ model: AuditoriumType, attributes: ['AUDITORIUM_TYPENAME'] }]}); // Пример JOIN
        res.json(auditoriums);
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

app.post('/api/auditoriums', async (req, res) => {
    try {
        const newAuditorium = await Auditorium.create(req.body);
        res.status(201).json(newAuditorium);
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

app.put('/api/auditoriums', async (req, res) => { // Исправлен URL с /auditorims
    try {
        const { AUDITORIUM, ...updateData } = req.body;
        if (!AUDITORIUM) {
            return res.status(400).json({ error: 'AUDITORIUM code is required for update.' });
        }
        const [updatedRows] = await Auditorium.update(updateData, { where: { AUDITORIUM } });
        if (updatedRows > 0) {
            const updatedAuditorium = await Auditorium.findByPk(AUDITORIUM);
            res.json(updatedAuditorium);
        } else {
            res.status(404).json({ error: `Auditorium with code ${AUDITORIUM} not found.` });
        }
    } catch (err) {
        handleSequelizeError(err, res);
    }
});

app.delete('/api/auditoriums/:auditoriumCode', async (req, res) => { // Исправлен URL с /auditorims
    try {
        const auditoriumCode = req.params.auditoriumCode;
        const deletedRows = await Auditorium.destroy({ where: { AUDITORIUM: auditoriumCode } });
        if (deletedRows > 0) {
            res.json({ message: `Auditorium ${auditoriumCode} deleted successfully.`});
        } else {
            res.status(404).json({ error: `Auditorium with code ${auditoriumCode} not found.` });
        }
    } catch (err) {
        handleSequelizeError(err, res);
    }
});


// --- Запуск сервера ---
app.listen(PORT, async () => {
    console.log(`Server started on http://localhost:${PORT}`);
    try {
        await sequelize.authenticate();
        console.log('Database connection has been established successfully.');
         // sequelize.sync(); // Не синхронизируем, т.к. таблицы уже созданы скриптом
    } catch (error) {
        console.error('Unable to connect to the database:', error);
    }
});