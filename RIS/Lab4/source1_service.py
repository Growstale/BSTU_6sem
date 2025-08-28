from flask import Flask, jsonify
import random
from datetime import datetime, timezone
import logging

logging.basicConfig(level=logging.INFO, format='%(asctime)s - Источник 1 - %(message)s')

app = Flask(__name__)
SERVICE_ID = "ТИ_1"


# Декоратор, связывает URL-путь с функцией
@app.route('/data')
def get_data():
    timestamp = datetime.now(timezone.utc).isoformat()
    value = round(random.uniform(50, 150), 2)
    data = {
        "source_id": SERVICE_ID,
        "timestamp_utc": timestamp,
        "measurement": value
    }
    logging.info(f"Отдаю данные: {data}")
    return jsonify(data)


@app.route('/status')
def get_status():
    logging.info("Запрошен статус. Отвечаю ОК.")
    return jsonify({"status": "OK"})


if __name__ == '__main__':
    print(f"--- Запуск Источника 1 ({SERVICE_ID}) на порту 5001 ---")
    app.run(host='0.0.0.0', port=5001, debug=False)