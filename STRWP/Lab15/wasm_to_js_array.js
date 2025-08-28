const fs = require('fs');
const path = require('path');

const wasmFilePath = path.join(__dirname, 'my_functions.wasm'); // Убедитесь, что путь верный
const wasmBuffer = fs.readFileSync(wasmFilePath);

// Преобразование Buffer в массив чисел
const byteArray = Array.from(wasmBuffer);

// Форматирование в строку Uint8Array
const jsArrayString = `let wasmCode = new Uint8Array([${byteArray.join(',')}]);`;

console.log(jsArrayString);

// fs.writeFileSync('wasm_code_array.js', jsArrayString);