import os
import sqlite3
import time
import random
import logging
import sys
import threading
from datetime import datetime, timezone
from flask import Flask, request, jsonify

OBJECT_COUNT = 10
# Как часто генерировать данные в фоне
GENERATION_INTERVAL_SECONDS = 30
SOURCE_DB_FILE_ARG = None # Исходный аргумент
ABS_SOURCE_DB_FILE = None # Абсолютный путь к БД
FLASK_PORT = None
logger = None
app = Flask(__name__)


def get_db_connection(db_file, read_only=False):
    db_uri = None
    try:
        if not os.path.exists(db_file):
            logger.error(f"Файл БД НЕ НАЙДЕН по абсолютному пути: {db_file}")
            return None
        elif not os.access(db_file, os.R_OK):
             logger.warning(f"Нет прав на ЧТЕНИЕ файла (проверка os.access): {db_file}")
        if not read_only and not os.access(db_file, os.W_OK):
             logger.warning(f"Нет прав на ЗАПИСЬ файла (проверка os.access): {db_file}")

        abs_db_file_uri_path = db_file.replace('\\', '/')
        db_uri = f'file:{abs_db_file_uri_path}?mode={"ro" if read_only else "rwc"}'

        use_check_same_thread = not read_only

        conn = sqlite3.connect(db_uri, uri=True, timeout=15.0, check_same_thread=use_check_same_thread) # Увеличил таймаут
        conn.row_factory = sqlite3.Row

        if not read_only:
            try:
                conn.execute("PRAGMA journal_mode=WAL;")
            except sqlite3.OperationalError as wal_err:
                 logger.warning(f"Не удалось включить WAL для {db_file}: {wal_err}. Возможно, БД уже открыта другим процессом в несовместимом режиме.")
        else:
            pass

        logger.debug(f"Успешное подключение к БД: {db_file}{' (read-only)' if read_only else ''} c check_same_thread={use_check_same_thread}")
        return conn
    except sqlite3.Error as e:
        logger.error(f"Ошибка подключения к БД {db_file} (URI: {db_uri}): {e}", exc_info=True)
        return None


def create_source_schema(conn):
    try:
        cursor = conn.cursor()
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS readings (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                object_id TEXT NOT NULL,
                reading_value REAL NOT NULL,
                timestamp_utc TEXT NOT NULL
            )
        """)
        cursor.execute("""
            CREATE INDEX IF NOT EXISTS idx_readings_timestamp
            ON readings (timestamp_utc)
        """)
        conn.commit()
        logger.info("Схема БД источника (таблица readings и индекс) проверена/создана.")
    except sqlite3.Error as e:
        logger.error(f"Ошибка при создании схемы БД источника: {e}")
        conn.rollback()


def generate_and_insert_data(gen_conn, object_ids, value_min, value_max):
    generated_count = 0
    try:
        cursor = gen_conn.cursor()
        current_time_utc = datetime.now(timezone.utc).isoformat(sep=' ', timespec='milliseconds')

        cursor.execute("BEGIN IMMEDIATE")
        for obj_id in object_ids:
            value = round(random.uniform(value_min, value_max), 3)
            cursor.execute("""
                INSERT INTO readings (object_id, reading_value, timestamp_utc)
                VALUES (?, ?, ?)
            """, (obj_id, value, current_time_utc))
            generated_count += 1
        gen_conn.commit()

        logger.debug(f"Фоновый поток: сгенерировано и вставлено {generated_count} записей.")
        return True
    except sqlite3.OperationalError as e:
        if "database is locked" in str(e).lower() or "busy" in str(e).lower():
            logger.warning(f"Фоновый поток: База данных занята ({e}), пропуск генерации в этом цикле.")
            try:
                gen_conn.rollback()
            except sqlite3.Error as re:
                logger.error(f"Фоновый поток: Ошибка при откате транзакции: {re}")
        else:
            logger.error(f"Фоновый поток: Ошибка SQLite при вставке данных: {e}", exc_info=True)
            try:
                gen_conn.rollback()
            except sqlite3.Error as re:
                logger.error(f"Фоновый поток: Ошибка при откате транзакции: {re}")
        return False
    except sqlite3.Error as e:
        logger.error(f"Фоновый поток: Ошибка SQLite при вставке данных: {e}", exc_info=True)
        try:
             gen_conn.rollback()
        except sqlite3.Error as re:
             logger.error(f"Фоновый поток: Ошибка при откате транзакции: {re}")
        return False


# --- Фоновый поток для генерации данных ---
def data_generation_loop(stop_event, abs_db_path, object_ids, min_val, max_val, interval):
    logger.info("Поток генерации данных запущен.")
    gen_conn = None
    while not stop_event.is_set():
        next_run_time = time.monotonic() + interval
        try:
            if not gen_conn:
                logger.debug("Поток генерации: Попытка установить соединение с БД...")
                gen_conn = get_db_connection(abs_db_path, read_only=False)
                if not gen_conn:
                    logger.error("Поток генерации: Не удалось установить соединение с БД. Повтор через 5 сек.")
                    time.sleep(5)
                    continue
                else:
                     logger.info("Поток генерации: соединение с БД установлено.")

            generate_and_insert_data(gen_conn, object_ids, min_val, max_val)

        except sqlite3.Error as e:
            logger.error(f"Поток генерации: Критическая ошибка SQLite: {e}", exc_info=True)
            if gen_conn:
                try:
                    gen_conn.close()
                except Exception as ce:
                    logger.error(f"Поток генерации: Ошибка при закрытии соединения: {ce}")
                gen_conn = None
            logger.info("Поток генерации: Пауза 5 сек после ошибки.")
            time.sleep(5) # Пауза перед следующей попыткой
        except Exception as e:
            logger.error(f"Поток генерации: Неожиданная ошибка: {e}", exc_info=True)
            pass

        wait_time = max(0, next_run_time - time.monotonic())
        stop_event.wait(wait_time)

    if gen_conn:
        try:
            gen_conn.close()
            logger.info("Поток генерации: соединение с БД закрыто.")
        except Exception as ce:
            logger.error(f"Поток генерации: Ошибка при финальном закрытии соединения: {ce}")
    logger.info("Поток генерации данных остановлен.")


@app.route('/get_data', methods=['GET'])
def get_new_data():
    global ABS_SOURCE_DB_FILE

    last_ts = request.args.get('last_timestamp_utc', '1970-01-01 00:00:00.000')
    limit = request.args.get('limit', default=1000, type=int)

    current_cwd = os.getcwd()
    logger.debug(f"API запрос: last_ts='{last_ts}', limit={limit}. CWD='{current_cwd}'. DB Path='{ABS_SOURCE_DB_FILE}'")

    conn = None
    try:
        conn = get_db_connection(ABS_SOURCE_DB_FILE, read_only=True)
        if not conn:
            logger.error("API: Не удалось получить read-only соединение с БД источника.")
            return jsonify({"error": "Could not connect to source database", "db_path": ABS_SOURCE_DB_FILE}), 500

        cursor = conn.cursor()
        cursor.execute("""
            SELECT id as original_id, object_id, reading_value, timestamp_utc
            FROM readings
            WHERE timestamp_utc > ?
            ORDER BY timestamp_utc ASC
            LIMIT ?
        """, (last_ts, limit))
        rows = cursor.fetchall()

        records = [dict(row) for row in rows]
        logger.info(f"API ответ: Найдено {len(records)} записей после {last_ts}.")
        return jsonify({"records": records})

    except sqlite3.Error as e:
        logger.error(f"API: Ошибка SQLite при запросе данных из '{ABS_SOURCE_DB_FILE}': {e}", exc_info=True)
        return jsonify({"error": "Database error occurred", "details": str(e)}), 500
    except Exception as e:
        logger.error(f"API: Неожиданная ошибка при запросе данных из '{ABS_SOURCE_DB_FILE}': {e}", exc_info=True)
        return jsonify({"error": "Internal server error"}), 500
    finally:
        if conn:
            conn.close()
            logger.debug("API: Read-only соединение с БД закрыто.")


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Использование: python source_api_server.py <путь_к_БД_ТО> <номер_студента> <ID_источника> <порт>")
        print("Пример: python source_api_server.py to1.db 15 TO1 5001")
        sys.exit(1)

    SOURCE_DB_FILE_ARG = sys.argv[1]
    ABS_SOURCE_DB_FILE = os.path.abspath(SOURCE_DB_FILE_ARG)
    try:
        student_number = int(sys.argv[2])
        if student_number <= 0:
            raise ValueError("Номер студента должен быть положительным")
    except ValueError as e:
        print(f"Ошибка: Неверный номер студента '{sys.argv[2]}'. {e}")
        sys.exit(1)
    source_id = sys.argv[3]
    try:
        FLASK_PORT = int(sys.argv[4])
        if not (1024 < FLASK_PORT < 65535):
            raise ValueError("Порт должен быть между 1025 и 65534")
    except ValueError as e:
        print(f"Ошибка: Неверный порт '{sys.argv[4]}'. {e}")
        sys.exit(1)

    log_format = f'%(asctime)s - Источник {source_id} (API:{FLASK_PORT}) %(levelname)s - %(message)s'
    logging.basicConfig(level=logging.DEBUG, format=log_format)
    logger = logging.getLogger()

    logger.info(f"=== Запуск API Сервера Источника для '{source_id}' ===")
    logger.info(f"Путь к файлу БД (аргумент): '{SOURCE_DB_FILE_ARG}'")
    logger.info(f"Абсолютный путь к файлу БД: '{ABS_SOURCE_DB_FILE}'")

    max_value = float(student_number * 10)
    min_value = 0.0
    logger.info(f"Генерация данных: диапазон [{min_value:.1f}, {max_value:.1f}], интервал {GENERATION_INTERVAL_SECONDS}с")
    logger.info(f"API будет слушать на порту: {FLASK_PORT}")

    object_ids = [f"OBJ-{i:03d}" for i in range(1, OBJECT_COUNT + 1)]

    init_conn = get_db_connection(ABS_SOURCE_DB_FILE, read_only=False)
    if init_conn:
        create_source_schema(init_conn)
        init_conn.close()
        logger.info("Инициализация схемы БД завершена.")
    else:
        logger.critical(f"Критическая ошибка: Не удалось инициализировать БД по пути '{ABS_SOURCE_DB_FILE}'. Проверьте путь и права доступа.")
        sys.exit(1)

    stop_generation_event = threading.Event()

    # Создание и запуск потока генерации
    generation_thread = threading.Thread(
        target=data_generation_loop,
        args=(stop_generation_event, ABS_SOURCE_DB_FILE, object_ids, min_value, max_value, GENERATION_INTERVAL_SECONDS),
        daemon=True
    )
    generation_thread.start()

    try:
        app.run(host='0.0.0.0', port=FLASK_PORT, debug=False, use_reloader=False)
    except Exception as e:
        logger.error(f"Ошибка запуска Flask сервера: {e}")
    finally:
        logger.info("Получен сигнал завершения. Остановка потока генерации...")
        stop_generation_event.set()
        generation_thread.join(timeout=10)
        if generation_thread.is_alive():
             logger.warning("Поток генерации не завершился за 10 секунд.")
        logger.info("Сервер источника остановлен.")