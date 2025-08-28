# territorial_api.py
import sqlite3
import logging
import os
from flask import Flask, request, jsonify
import argparse

LOG_FILE_TEMPLATE = 'territorial_api_{port}.log'

app = Flask(__name__)

db_path_global = None
logger_global = None


# --- Настройка логирования ---
def setup_logging(port):
    global logger_global
    log_file = LOG_FILE_TEMPLATE.format(port=port)
    log_formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
    logger = logging.getLogger(f'territorial_api_{port}')
    logger.setLevel(logging.INFO)

    if logger.hasHandlers():
        logger.handlers.clear()

    # Файловый обработчик
    try:
        file_handler = logging.FileHandler(log_file, encoding='utf-8')
        file_handler.setFormatter(log_formatter)
        logger.addHandler(file_handler)
    except Exception as e:
        print(f"FATAL: Could not set up file logging for port {port}: {e}") # Используем print, т.к. логгер может быть не готов

    # Консольный обработчик
    console_handler = logging.StreamHandler()
    console_handler.setFormatter(log_formatter)
    logger.addHandler(console_handler)
    logger_global = logger
    logger_global.info(f"Логгер для порта {port} настроен, лог в {log_file}")


def connect_db(db_path):
    try:
        conn = sqlite3.connect(db_path, timeout=10, check_same_thread=False) # check_same_thread=False важно для Flask
        conn.row_factory = sqlite3.Row
        return conn
    except sqlite3.Error as e:
        if logger_global:
            logger_global.error(f"Ошибка подключения к БД {db_path}: {e}")
        else:
            print(f"ERROR: Ошибка подключения к БД {db_path}: {e}")
        return None


def setup_local_database(db_path):
    if not logger_global:
        print("ERROR: Логгер не инициализирован для setup_local_database")
        return False
    logger_global.info(f"Проверка и настройка структуры БД: {db_path}...")
    conn = connect_db(db_path)
    if conn:
        try:
            cursor = conn.cursor()
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS telemetry (
                    central_id INTEGER PRIMARY KEY,
                    timestamp DATETIME,
                    sensor_id TEXT,
                    value REAL
                )
            ''')
            conn.commit()
            logger_global.info(f"Таблица 'telemetry' в {db_path} готова.")
            return True
        except sqlite3.Error as e:
            logger_global.error(f"Ошибка настройки таблицы 'telemetry' в {db_path}: {e}")
            return False
        finally:
            conn.close()
    return False


def save_replicated_data(db_path, records):
    # Сохраняет полученные данные в локальную БД
    if not logger_global:
        print("ERROR: Логгер не инициализирован для save_replicated_data")
        return None, "Internal logger error"

    successful_ids = []
    conn = connect_db(db_path)
    if not conn:
        return None, f"Could not connect to database {db_path}"

    try:
        cursor = conn.cursor()
        conn.execute('BEGIN TRANSACTION')
        for record in records:
            central_id = record.get('id')
            timestamp = record.get('timestamp')
            sensor_id = record.get('sensor_id')
            value = record.get('value')

            # Валидация
            if central_id is None or timestamp is None or sensor_id is None or value is None:
                logger_global.warning(f"Пропуск записи из-за отсутствия данных: {record}")
                continue

            try:
                cursor.execute('''
                    INSERT OR IGNORE INTO telemetry (central_id, timestamp, sensor_id, value)
                    VALUES (?, ?, ?, ?)
                ''', (central_id, timestamp, sensor_id, value))

                successful_ids.append(central_id)

            except sqlite3.Error as e:
                logger_global.error(f"Ошибка при вставке записи {central_id}: {e}. Запись: {record}")
                continue

        conn.commit()
        logger_global.info(f"Успешно обработано {len(successful_ids)} записей.")
        return successful_ids, None

    except Exception as e:
        logger_global.exception(f"Критическая ошибка во время сохранения данных в {db_path}. Откат транзакции.")
        if conn:
            conn.rollback()
        return None, f"Internal server error during database operation: {e}"
    finally:
        if conn:
            conn.close()


# Эндпоинт API
@app.route('/replicate', methods=['POST'])
def handle_replication():
    if not logger_global:
        return jsonify({"status": "error", "message": "Server logger not configured"}), 500

    logger_global.info("Получен входящий POST запрос на /replicate")

    if not request.is_json:
        logger_global.warning("Запрос не содержит JSON данные")
        return jsonify({"status": "error", "message": "Request must be JSON"}), 400

    data = request.get_json()

    if not isinstance(data, list):
        logger_global.warning(f"Ожидался список записей, получен {type(data)}")
        return jsonify({"status": "error", "message": "Expected a JSON list of records"}), 400

    if not data:
        logger_global.info("Получен пустой список записей.")
        return jsonify({"status": "success", "message": "Empty list received", "processed_ids": []}), 200

    logger_global.info(f"Получено {len(data)} записей для сохранения.")

    processed_ids, error_message = save_replicated_data(db_path_global, data)

    if error_message:
        logger_global.error(f"Ошибка при сохранении данных: {error_message}")
        return jsonify({"status": "error", "message": error_message}), 500
    else:
        logger_global.info(f"Успешно обработаны ID: {processed_ids}")
        return jsonify({"status": "success", "processed_ids": processed_ids}), 200


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Territorial Telemetry Replication API')
    parser.add_argument('--port', type=int, required=True, help='Port number to run the API on')
    parser.add_argument('--db', type=str, required=True, help='Path to the territorial SQLite database file')
    args = parser.parse_args()

    db_path_global = args.db
    setup_logging(args.port)

    if not setup_local_database(db_path_global):
        if logger_global:
            logger_global.critical(f"Не удалось настроить базу данных {db_path_global}. API не может стартовать.")
        else:
            print(f"FATAL: Не удалось настроить базу данных {db_path_global}. API не может стартовать.")
        exit(1)

    logger_global.info(f"Запуск API на порту {args.port} для работы с БД {args.db}")
    app.run(host='0.0.0.0', port=args.port, debug=False)