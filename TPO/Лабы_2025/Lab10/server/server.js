const express = require('express');
const { v4: uuidv4 } = require('uuid');
const bodyParser = require('body-parser');

const app = express();
const PORT = process.env.PORT || 3000;

// Middleware
app.use(bodyParser.json()); 
app.use(bodyParser.urlencoded({ extended: true })); 

let users = [
    { id: uuidv4(), name: 'Alice Smith', email: 'alice@example.com', age: 30 },
    { id: uuidv4(), name: 'Bob Johnson', email: 'bob@example.com', age: 24 },
];

// --- Middleware для "аутентификации" ---
const API_KEY = "supersecretkey";

const authenticate = (req, res, next) => {
    const providedKey = req.headers['x-api-key'];
    if (req.path.startsWith('/admin') && (!providedKey || providedKey !== API_KEY)) {
        return res.status(403).json({ message: "Forbidden: Invalid or missing API Key for admin routes" });
    }
    next();
};
app.use(authenticate); 

// --- Эндпоинты ---

// 1. Получение списка доступных методов API
app.get('/api/methods', (req, res) => {
    res.json({
        message: "Available API methods",
        methods: [
            { path: "/api/methods", method: "GET", description: "Get list of available API methods" },
            { path: "/users", method: "GET", description: "Get all users (supports pagination: ?page=1&limit=5)" },
            { path: "/users", method: "POST", description: "Create a new user" },
            { path: "/users/:id", method: "GET", description: "Get a user by ID" },
            { path: "/users/:id", method: "PUT", description: "Update a user by ID" },
            { path: "/users/:id", method: "DELETE", description: "Delete a user by ID" },
            { path: "/admin/users", method: "GET", description: "Get all users (admin access required)" }
        ]
    });
});

// 2. CRUD для пользователей (/users)

// Create (POST /users)
app.post('/users', (req, res) => {
    const { name, email, age } = req.body;

    // Валидация
    if (!name || typeof name !== 'string' || name.trim() === '') {
        return res.status(400).json({ message: "Name is required and must be a non-empty string." });
    }
    if (name.length > 50) {
        return res.status(400).json({ message: "Name cannot be longer than 50 characters." });
    }
    if (!email || !/^\S+@\S+\.\S+$/.test(email)) {
        return res.status(400).json({ message: "Valid email is required." });
    }
    if (users.some(user => user.email === email)) {
        return res.status(409).json({ message: "Email already exists." }); // 409 Conflict
    }
    if (age !== undefined && (typeof age !== 'number' || age < 0 || age > 120)) {
        return res.status(400).json({ message: "Age must be a number between 0 and 120." });
    }

    const newUser = {
        id: uuidv4(),
        name: name.trim(),
        email,
        age: age === undefined ? null : age // Сохраняем null если возраст не указан
    };
    users.push(newUser);
    res.status(201).json({ message: "User created successfully", user: newUser });
});

// Read all (GET /users) - с пагинацией
app.get('/users', (req, res) => {
    let { page, limit } = req.query;
    page = parseInt(page, 10) || 1;
    limit = parseInt(limit, 10) || 10;

    if (page < 1) page = 1;
    if (limit < 1) limit = 1;
    if (limit > 100) limit = 100; // Ограничение на максимальный limit

    const startIndex = (page - 1) * limit;
    const endIndex = page * limit;

    const paginatedUsers = users.slice(startIndex, endIndex);

    if (paginatedUsers.length === 0 && page > 1 && startIndex >= users.length) {
         // Запрошена страница за пределами существующего диапазона
         return res.status(404).json({
            message: "Page not found. No users on this page.",
            page,
            limit,
            totalPages: Math.ceil(users.length / limit),
            totalUsers: users.length
        });
    }

    res.json({
        message: "Users retrieved successfully",
        page,
        limit,
        totalPages: Math.ceil(users.length / limit),
        totalUsers: users.length,
        users: paginatedUsers
    });
});

// Read one (GET /users/:id)
app.get('/users/:id', (req, res) => {
    const { id } = req.params;
    const user = users.find(u => u.id === id);
    if (!user) {
        return res.status(404).json({ message: "User not found" });
    }
    res.json({ message: "User retrieved successfully", user });
});

// Update (PUT /users/:id)
app.put('/users/:id', (req, res) => {
    const { id } = req.params;
    const { name, email, age } = req.body;
    const userIndex = users.findIndex(u => u.id === id);

    if (userIndex === -1) {
        return res.status(404).json({ message: "User not found" });
    }

    const updatedUser = { ...users[userIndex] };

    if (name !== undefined) {
        if (typeof name !== 'string' || name.trim() === '') {
            return res.status(400).json({ message: "Name must be a non-empty string if provided." });
        }
        if (name.length > 50) {
            return res.status(400).json({ message: "Name cannot be longer than 50 characters." });
        }
        updatedUser.name = name.trim();
    }
    if (email !== undefined) {
        if (!/^\S+@\S+\.\S+$/.test(email)) {
            return res.status(400).json({ message: "Valid email is required if provided." });
        }
        if (users.some(user => user.email === email && user.id !== id)) {
            return res.status(409).json({ message: "Email already in use by another user." });
        }
        updatedUser.email = email;
    }
    if (age !== undefined) {
        if (typeof age !== 'number' || age < 1 || age > 120) {
            return res.status(400).json({ message: "Age must be a number between 1 and 120 if provided." });
        }
        updatedUser.age = age;
    }


    users[userIndex] = updatedUser;
    res.json({ message: "User updated successfully", user: updatedUser });
});

// Delete (DELETE /users/:id)
app.delete('/users/:id', (req, res) => {
    const { id } = req.params;
    const initialLength = users.length;
    users = users.filter(u => u.id !== id);

    if (users.length === initialLength) {
        return res.status(404).json({ message: "User not found" });
    }
    // Стандартный ответ для DELETE - 204 No Content
    res.status(204).send();
});

// --- Защищенный эндпоинт ---
app.get('/admin/users', (req, res) => {
    // Middleware 'authenticate' уже проверил ключ
    res.json({ message: "Admin access: All users", users });
});


// --- Обработка несуществующих роутов (404) ---
app.use((req, res, next) => {
    res.status(404).json({ message: `Endpoint not found: ${req.method} ${req.originalUrl}` });
});

// --- Глобальный обработчик ошибок ---
// Если в каком-то из роутов произойдет ошибка (через next(err)), она попадет сюда
app.use((err, req, res, next) => {
    console.error("Error caught by global error handler:", err.message);
    if (process.env.NODE_ENV !== 'production' && process.env.NODE_ENV !== 'test') {
        console.error(err.stack);
    }

    // Специальная обработка для ошибок парсинга JSON от body-parser
    // Такие ошибки являются экземплярами SyntaxError, имеют статус 400,
    // свойство 'body' (где произошла ошибка парсинга), и err.type === 'entity.parse.failed'.
    if (err instanceof SyntaxError && err.status === 400 && 'body' in err && err.type === 'entity.parse.failed') {
        return res.status(400).json({
            status: "error",
            message: "Invalid JSON payload. Please ensure your JSON is well-formed and the 'Content-Type' header is set to 'application/json'."
        });
    }

    // Обработка других ошибок, которые могут иметь свойство status
    // (например, ошибки от других middleware или кастомные ошибки, переданные через next(err))
    if (typeof err.status === 'number') {
        return res.status(err.status).json({
            status: "error",
            message: err.message || "An error occurred."
        });
    }

    // Для всех остальных непредвиденных ошибок на сервере (без явного статуса)
    // или если err.status не число (что маловероятно для стандартных ошибок)
    res.status(500).json({
        status: "error",
        message: "Something went wrong on the server. Please try again later."
    });
});


// Запуск сервера
app.listen(PORT, () => {
    console.log(`Server is running on http://localhost:${PORT}`);
});