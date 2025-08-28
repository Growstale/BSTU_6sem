from flask import Flask, jsonify
import random
from datetime import datetime, timezone
import logging

logging.basicConfig(level=logging.INFO, format='%(asctime)s - Источник 2 - %(message)s')

app = Flask(__name__)
SERVICE_ID = "ТИ_2"


@app.route('/data')
def get_data():
    timestamp = datetime.now(timezone.utc).isoformat()
    value = round(random.uniform(200, 300), 2)
    data = {
        "source_id": SERVICE_ID,
        "timestamp_utc": timestamp,
        "measurement": value
    }
    logging.info(f"Отдаю данные: {data}")
    return jsonify(data)


@app.route('/status')
def get_status():
    if random.randint(1, 3) == 2:
        logging.warning("Имитирую временный сбой статуса (ошибка 503)")
        return jsonify({"error": "Service temporarily unavailable"}), 503
    logging.info("Запрошен статус. Отвечаю ОК.")
    return jsonify({"status": "OK"})


if __name__ == '__main__':
    print(f"--- Запуск Источника 2 ({SERVICE_ID}) на порту 5002 ---")
    app.run(host='0.0.0.0', port=5002, debug=False)