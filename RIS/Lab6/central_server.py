import sqlite3
import logging
import time
import requests
import json
import random
from datetime import datetime
import threading
import argparse

# --- Конфигурация ---
CENTRAL_DB_PATH = 'central_db.sqlite'
LOG_FILE = 'central_replicator.log'
BATCH_SIZE = 5
REPLICATION_INTERVAL_SECONDS = 10
REQUEST_TIMEOUT_SECONDS = 15
GENERATION_INTERVAL_SECONDS = 10
RECORD_NAMES = [f"record_{i}" for i in range(1, 6)]

TERRITORIAL_TARGETS = [
    {
        'id': 'terr1',
        'api_url': 'http://localhost:5001/replicate'
    },
    {
        'id': 'terr2',
        'api_url': 'http://localhost:5002/replicate'
    }
]

TERRITORY_IDS = [target['id'] for target in TERRITORIAL_TARGETS]

STATUS_PENDING = 0
STATUS_SUCCESS = 1

log_formatter = logging.Formatter('%(asctime)s - %(levelname)s - [%(name)s] - %(message)s')
logger = logging.getLogger('central_replicator')
logger.setLevel(logging.INFO)

try:
    file_handler = logging.FileHandler(LOG_FILE, encoding='utf-8')
    file_handler.setFormatter(log_formatter)
    logger.addHandler(file_handler)
except Exception as e:
    print(f"FATAL: Could not set up file logging: {e}")
console_handler = logging.StreamHandler()
console_handler.setFormatter(log_formatter)
logger.addHandler(console_handler)


def connect_db(db_path):
    try:
        conn = sqlite3.connect(db_path, timeout=10)
        conn.row_factory = sqlite3.Row
        try:
            conn.execute('PRAGMA journal_mode=WAL;')
            logger.debug(f"WAL mode enabled for {db_path}")
        except sqlite3.Error as e:
            logger.warning(f"Could not enable WAL mode for {db_path}: {e}.")
        logger.info(f"Успешное подключение к БД: {db_path}")
        return conn
    except sqlite3.Error as e:
        logger.error(f"Ошибка подключения к БД {db_path}: {e}")
        return None


def setup_central_database():
    logger.info("Проверка и настройка структуры центральной БД")
    conn_central = connect_db(CENTRAL_DB_PATH)
    if conn_central:
        try:
            cursor = conn_central.cursor()
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS telemetry (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
                    sensor_id TEXT NOT NULL, -- Имя записи, например 'record_1'
                    value REAL NOT NULL,
                    target_territory_id TEXT NOT NULL,
                    replication_status INTEGER NOT NULL DEFAULT 0
                )
            ''')
            cursor.execute(f'''
                CREATE INDEX IF NOT EXISTS idx_repl_target_status
                ON telemetry (target_territory_id, replication_status)
                WHERE replication_status = {STATUS_PENDING};
            ''')
            conn_central.commit()
            logger.info(f"Таблица 'telemetry' (с target_id) и индекс в {CENTRAL_DB_PATH} готовы.")
        except sqlite3.Error as e:
            logger.error(f"Ошибка настройки таблицы 'telemetry' в {CENTRAL_DB_PATH}: {e}")
        finally:
            conn_central.close()


def get_unreplicated_data(conn_central, target_id, limit):
    cursor = conn_central.cursor()
    sql = """
        SELECT id, timestamp, sensor_id, value, target_territory_id
        FROM telemetry
        WHERE target_territory_id = ? AND replication_status = ?
        ORDER BY id ASC
        LIMIT ?
    """
    try:
        cursor.execute(sql, (target_id, STATUS_PENDING, limit))
        records = cursor.fetchall()
        records_as_dicts = [dict(row) for row in records]
        logger.debug(f"Найдено {len(records_as_dicts)} записей для репликации в узел {target_id}.")
        return records_as_dicts
    except sqlite3.Error as e:
        logger.error(f"Ошибка получения нереплицированных данных для узла {target_id}: {e}")
        return []


def update_replication_status(conn_central, record_ids, new_status):
    if not record_ids:
        logger.debug(f"Нет ID для обновления статуса на {new_status}.")
        return False
    cursor = conn_central.cursor()
    placeholders = ', '.join('?' * len(record_ids))
    sql = f"UPDATE telemetry SET replication_status = ? WHERE id IN ({placeholders})"
    try:
        params = [new_status] + record_ids
        cursor.execute(sql, params)
        updated_count = cursor.rowcount
        conn_central.commit()
        if updated_count > 0:
            logger.info(f"Обновлен статус (replication_status={new_status}) для {updated_count} записей.")
            return True
        else:
            logger.warning(f"Статус replication_status={new_status} не был обновлен (возможно, ID {record_ids} уже обработаны?).")
            return False
    except sqlite3.Error as e:
        logger.error(f"Ошибка обновления статуса на {new_status} в ЦБД для ID {record_ids}: {e}")
        conn_central.rollback()
        return False


def run_replication_cycle():
    logger.info("=== Начало цикла репликации ===")
    start_time = time.time()
    conn_central = connect_db(CENTRAL_DB_PATH)
    if not conn_central:
        logger.error("Не удалось подключиться к центральной БД. Цикл репликации прерван.")
        return
    # Инициализируем счетчик успешно реплицированных записей
    total_updated_count = 0
    # Обрабатываем каждый территориальный узел
    for target in TERRITORIAL_TARGETS:
        target_id = target['id']
        target_url = target['api_url']
        logger.info(f"--- Начало репликации для целевого узла: {target_id} ({target_url}) ---")
        try:
            records_to_replicate = get_unreplicated_data(conn_central, target_id, BATCH_SIZE)
            if not records_to_replicate:
                logger.info(f"Нет новых данных для репликации в узел {target_id}.")
                continue

            verified_records = []
            for record in records_to_replicate:
                if record['target_territory_id'] == target_id:
                    verified_records.append(record)
                else:
                    logger.error(f"[ПРОВЕРКА ФЕЙЛ] Запись {record['id']} для {target_id}, помечена для {record['target_territory_id']}!")
            if not verified_records:
                logger.warning(f"После проверки не осталось записей для узла {target_id}.")
                continue

            logger.info(f"Подготовлено {len(verified_records)} записей для отправки в {target_id} (после проверки).")
            response = None
            try:
                logger.debug(f"Отправка POST запроса на {target_url}...")
                response = requests.post(target_url, json=verified_records, timeout=REQUEST_TIMEOUT_SECONDS, headers={'Content-Type': 'application/json'})
                response.raise_for_status()
                logger.info(f"Узел {target_id} ответил статусом {response.status_code}.")
                try:
                    response_data = response.json()
                    if response_data.get("status") == "success" and "processed_ids" in response_data:
                        # Извлекаем список ID, которые узел подтвердил как обработанные
                        processed_ids = response_data["processed_ids"]
                        if isinstance(processed_ids, list):
                            logger.info(f"Узел {target_id} подтвердил обработку {len(processed_ids)} ID.")
                            if processed_ids:
                                sent_ids_set = {rec['id'] for rec in verified_records}
                                ids_to_update = [pid for pid in processed_ids if pid in sent_ids_set]
                                if ids_to_update:
                                    if update_replication_status(conn_central, ids_to_update, STATUS_SUCCESS):
                                        # Обновление статуса
                                        total_updated_count += len(ids_to_update)
                                else: logger.warning(f"Подтвержденные ID {processed_ids} не найдены среди отправленных {list(sent_ids_set)}.")
                            else: logger.info(f"Узел {target_id} вернул пустой список обработанных ID.")
                        else: logger.error(f"Некорректный формат processed_ids: {type(processed_ids)}")
                    else: logger.error(f"Узел {target_id} вернул status {response.status_code}, но ответ некорректен: {response.text}")
                except json.JSONDecodeError: logger.error(f"Не удалось декодировать JSON ответ от {target_id}: {response.text}")
                except Exception as e_parse: logger.error(f"Ошибка обработки ответа от {target_id}: {e_parse}")
            except requests.exceptions.Timeout: logger.error(f"Таймаут при отправке в {target_id} ({target_url}).")
            except requests.exceptions.ConnectionError: logger.error(f"Ошибка соединения с {target_id} ({target_url}).")
            except requests.exceptions.RequestException as e_http:
                logger.error(f"Ошибка HTTP при запросе к {target_id} ({target_url}): {e_http}")
                if response is not None: logger.error(f"Ответ сервера {target_id}: {response.status_code} - {response.text}")
            except Exception as e_send: logger.exception(f"Непредвиденная ошибка при отправке/обработке ответа для {target_id}: {e_send}")
        except Exception as e_outer: logger.exception(f"Непредвиденная ошибка в цикле обработки узла {target_id}: {e_outer}")
        logger.info(f"--- Завершение репликации для узла: {target_id} ---")
    if conn_central: conn_central.close()
    end_time = time.time()
    logger.info(f"=== Цикл репликации HTTP завершен за {end_time - start_time:.2f} секунд. Обновлено статусов на SUCCESS: {total_updated_count} ===")


def add_telemetry_data(conn_central, sensor_id, value, target_id):
    if not conn_central:
        logger.error("[Generator] Нет соединения с ЦБД"); return False
    try:
        cursor = conn_central.cursor()
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
        cursor.execute("INSERT INTO telemetry (timestamp, sensor_id, value, target_territory_id) VALUES (?, ?, ?, ?)", (timestamp, sensor_id, value, target_id))
        return True
    except sqlite3.Error as e: logger.error(f"[Generator] Ошибка добавления данных (target={target_id}): {e}"); return False


_stop_generation_event = threading.Event()


def data_generation_thread(db_path, interval, record_names_list, available_target_ids):
    logger.info(f"[Generator] Поток генерации данных запущен (для имен: {record_names_list}).")
    conn_gen = None
    if not available_target_ids:
        logger.error("[Generator] Нет доступных ID территорий! Поток остановлен.")
        return

    while not _stop_generation_event.is_set():
        if conn_gen is None:
            conn_gen = connect_db(db_path)
            if conn_gen is None:
                logger.error(f"[Generator] Не удалось подключиться к БД. Повтор через {interval} сек.")
                time.sleep(interval)
                continue

        logger.debug(f"[Generator] Генерация пачки записей: {record_names_list}")
        records_added_in_batch = 0
        try:
            conn_gen.execute('BEGIN TRANSACTION')
            for target_id in TERRITORY_IDS:
                for record_name in record_names_list:
                    value = round(random.uniform(10.0, 100.0), 2)

                    if add_telemetry_data(conn_gen, record_name, value, target_id):
                        logger.debug(f"[Generator] Подготовлена запись для {target_id}: Name={record_name}, Value={value}")
                        records_added_in_batch += 1
                    else:
                        logger.warning(f"[Generator] Не удалось подготовить запись {record_name}.")
                        raise sqlite3.Error(f"Failed to add record {record_name}") # Пример прерывания пачки

            conn_gen.commit()
            logger.info(f"[Generator] Успешно добавлена пачка из {records_added_in_batch} записей.")

        except sqlite3.Error as e:
            logger.error(f"[Generator] Ошибка SQLite во время генерации/коммита пачки: {e}. Откат пачки.")
            if conn_gen:
                try:
                    conn_gen.rollback()
                except sqlite3.Error as rb_err: logger.error(f"[Generator] Ошибка при откате: {rb_err}")
            if conn_gen:
                conn_gen.close()
            conn_gen = None
            logger.info("[Generator] Соединение сброшено из-за ошибки.")
        except Exception as e:
            logger.exception(f"[Generator] Непредвиденная ошибка в потоке генерации пачки: {e}")
            if conn_gen:
                try:
                    conn_gen.rollback()
                except:
                    pass
                conn_gen.close()
                conn_gen = None

        logger.debug(f"[Generator] Пауза {interval} секунд перед следующей пачкой.")
        time.sleep(interval)

    if conn_gen:
        conn_gen.close()
    logger.info("[Generator] Поток генерации данных остановлен.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Central Replicator (Targeted Records) with Fixed Record Names Generator')
    parser.add_argument('--generate', action='store_true', help='Enable internal data generator (record_1 to record_5)')
    args = parser.parse_args()

    logger.info("Запуск центрального сервиса репликации (HTTP Push, Таргет., Фикс. имена)...")
    if args.generate:
        logger.info("Внутренний генератор данных ВКЛЮЧЕН (record_1..5).")
    else:
        logger.info("Внутренний генератор данных ВЫКЛЮЧЕН.")

    setup_central_database()

    generator_thread = None
    if args.generate:
        generator_thread = threading.Thread(
            target=data_generation_thread,
            args=(CENTRAL_DB_PATH, GENERATION_INTERVAL_SECONDS, RECORD_NAMES, TERRITORY_IDS), # Передаем RECORD_NAMES
            daemon=True
        )
        generator_thread.start()

    # Запуск основного цикла репликации
    logger.info(f"Запуск основного цикла репликации с интервалом {REPLICATION_INTERVAL_SECONDS} секунд.")
    try:
        while True:
            run_replication_cycle()
            logger.debug(f"Пауза основного цикла {REPLICATION_INTERVAL_SECONDS} секунд...")
            time.sleep(REPLICATION_INTERVAL_SECONDS)
    except KeyboardInterrupt:
        logger.info("Центральный сервис репликации останавливается вручную...")
    except Exception as e:
        logger.exception(f"Критическая ошибка в главном потоке репликации: {e}")
    finally:
        if generator_thread and generator_thread.is_alive():
            logger.info("Отправка сигнала остановки потоку генерации...")
            _stop_generation_event.set()
            generator_thread.join(timeout=5)
            if generator_thread.is_alive(): logger.warning("Поток генерации не завершился штатно.")
        logger.info("Центральный сервис репликации завершил работу")
