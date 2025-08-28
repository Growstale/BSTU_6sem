const express = require('express');
const path = require('path');
const app = express();
const port = 3000;

// Middleware для раздачи статических файлов из папки 'public'
// Все файлы из 'public' будут доступны по URL

app.use(express.static(path.join(__dirname, 'public')));

app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'task2.html'));
});

app.listen(port, () => {
    console.log(`Server listening at http://localhost:${port}`);
    console.log(`WASM file should be available at http://localhost:${port}/my_functions.wasm`);
});