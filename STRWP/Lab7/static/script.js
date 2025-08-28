console.log("Скрипт script.js загружен!");

async function fetchData(url, elementId) {
    const element = document.getElementById(elementId);
    try {
        const response = await fetch(url); 
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        const contentType = response.headers.get("content-type");
        let dataText;
        if (contentType && contentType.includes("application/json")) {
            const data = await response.json();
            dataText = JSON.stringify(data, null, 2); // Форматированный вывод JSON
        } else {
            dataText = await response.text(); // Для XML и других текстовых форматов
        }
        element.textContent = dataText;
    } catch (error) {
        console.error('Ошибка при загрузке данных:', error);
        element.textContent = `Ошибка загрузки: ${error.message}`;
    }
}

fetchData('/data.json', 'json-data');
fetchData('/data.xml', 'xml-data');