const fs = require('fs');
 const path = require('path');

 async function runWasmInNode() {
     try {
         const wasmFilePath = path.join(__dirname, 'public', 'my_functions.wasm');
         const wasmCode = fs.readFileSync(wasmFilePath);

         const importObject = {
         };

         const wasmModule = await WebAssembly.compile(wasmCode);
         const instance = await WebAssembly.instantiate(wasmModule, importObject);

         const x = 15;
         const y = 7;

         const sumResult = instance.exports.sum(x, y);
         const subResult = instance.exports.sub(x, y);
         const mulResult = instance.exports.mul(x, y);

         console.log(`Running WASM functions in Node.js (using ${path.basename(wasmFilePath)}):`);
         console.log(`sum(${x}, ${y}) = ${sumResult}`);
         console.log(`sub(${x}, ${y}) = ${subResult}`);
         console.log(`mul(${x}, ${y}) = ${mulResult}`);

     } catch (error) {
         console.error("Error running WASM in Node.js:", error);
     }
 }

 runWasmInNode();