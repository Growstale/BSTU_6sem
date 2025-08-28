import sqlite3
import sys
import time
import logging
import requests
from datetime import datetime, timezone

GO_DB_FILE = "go.db"
TO_SOURCES = {
    "TO1": "http://127.0.0.1:5001/get_data",
    "TO2": "http://127.0.0.1:5002/get_data",
}

REPLICATION_INTERVAL_SECONDS = 40 # Как часто опрашивать источники
FETCH_LIMIT = 1000 # Максимальное количество записей для запроса у источника за раз
REQUEST_TIMEOUT_SECONDS = 15 # Таймаут для HTTP запроса к источнику

logging.basicConfig(level=logging.INFO, format='%(asctime)s - Репликатор (Pull HTTP) %(levelname)s - %(message)s')


def get_db_connection(db_file):
    try:
        conn = sqlite3.connect(db_file, timeout=10.0)
        conn.row_factory = sqlite3.Row
        conn.execute("PRAGMA journal_mode=WAL;")
        logging.info(f"Успешное подключение к центральной БД: {db_file}")
        return conn
    except sqlite3.Error as e:
        logging.error(f"Ошибка подключения к центральной БД {db_file}: {e}")
        return None


def create_go_schema(go_conn):
    try:
        cursor = go_conn.cursor()
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS collected_readings (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                source_to_id TEXT NOT NULL,
                original_reading_id INTEGER NOT NULL,
                object_id TEXT NOT NULL,
                reading_value REAL NOT NULL,
                original_timestamp_utc TEXT NOT NULL,
                replication_timestamp_utc TEXT NOT NULL,
                UNIQUE(source_to_id, original_reading_id)
            )
        """)
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS replication_status (
                source_to_id TEXT PRIMARY KEY,
                last_pulled_timestamp_utc TEXT
            )
        """)
        go_conn.commit()
        logging.info("Схема центральной БД (таблицы collected_readings, replication_status) проверена/создана.")
    except sqlite3.Error as e:
        logging.error(f"Ошибка при создании схемы центральной БД: {e}")
        go_conn.rollback()


def get_last_pulled_timestamp(go_conn, source_id):
    # Получает последнюю метку времени для указанного источника из нашей БД
    last_ts = '1970-01-01 00:00:00.000'
    try:
        cursor = go_conn.cursor()
        cursor.execute("SELECT last_pulled_timestamp_utc FROM replication_status WHERE source_to_id = ?", (source_id,))
        result = cursor.fetchone()
        if result and result['last_pulled_timestamp_utc']:
            last_ts = result['last_pulled_timestamp_utc']
            logging.debug(f"Для {source_id} найдена последняя метка времени: {last_ts}")
        else:
            logging.info(f"Для {source_id} последняя метка времени не найдена, используется '{last_ts}'.")
        return last_ts
    except sqlite3.Error as e:
        logging.error(f"Ошибка при получении последней метки времени для {source_id}: {e}")
        return last_ts


def update_replication_status(go_conn, source_id, new_timestamp):
    # Обновляет последнюю метку времени для источника в нашей БД
    try:
        cursor = go_conn.cursor()
        cursor.execute("""
            INSERT OR REPLACE INTO replication_status (source_to_id, last_pulled_timestamp_utc)
            VALUES (?, ?)
        """, (source_id, new_timestamp))
        logging.debug(f"Статус репликации для {source_id} будет обновлен на {new_timestamp}")
    except sqlite3.Error as e:
        logging.error(f"Ошибка при подготовке обновления статуса репликации для {source_id}: {e}")
        raise


def replicate_from_source(go_conn, source_id, source_api_url):
    logging.info(f"Начинаю репликацию из источника: {source_id} (API: {source_api_url})")
    records_replicated = 0
    latest_ts_in_batch = None
    all_processed_successfully = False

    try:
        last_pulled_ts = get_last_pulled_timestamp(go_conn, source_id)

        # Сделать HTTP GET запрос к API источника
        params = {
            'last_timestamp_utc': last_pulled_ts,
            'limit': FETCH_LIMIT
        }
        logging.debug(f"Запрос данных к {source_api_url} с параметрами: {params}")
        try:
            response = requests.get(source_api_url, params=params, timeout=REQUEST_TIMEOUT_SECONDS)
            # Проверка на HTTP ошибки 4xx/5xx
            response.raise_for_status()
            # Ожидаем JSON ответ
            data = response.json()
            # Ищем ключ 'records'
            new_records = data.get('records', [])

        except requests.exceptions.RequestException as e:
            logging.error(f"Ошибка сети при запросе к источнику {source_id} ({source_api_url}): {e}")
            return
        except ValueError as e:
            logging.error(f"Некорректный JSON ответ от источника "
                           f"{source_id} ({source_api_url}): {e}. Ответ: {response.text[:200]}")
            return
        except Exception as e:
            logging.error(f"Неожиданная ошибка при запросе к {source_id}: {e}")
            return

        if not new_records:
            logging.info(f"Нет новых данных для репликации от {source_id}.")
            return

        logging.info(f"Получено {len(new_records)} новых записей от {source_id}.")

        # Вставить новые данные обновить статус
        go_cursor = go_conn.cursor()
        replication_time = datetime.now(timezone.utc).isoformat(sep=' ', timespec='milliseconds')
        # Начинаем транзакцию для вставки и обновления статуса
        go_conn.execute("BEGIN TRANSACTION")

        try:
            for record in new_records:
                # Проверяем формат записи от клиента
                if not all(k in record for k in ('original_id', 'object_id', 'reading_value', 'timestamp_utc')):
                    logging.warning(f"Пропущена невалидная запись от {source_id}: {record}")
                    continue

                original_id = record['original_id']
                original_ts = record['timestamp_utc']

                go_cursor.execute("""
                    INSERT OR IGNORE INTO collected_readings
                    (source_to_id, original_reading_id, object_id, reading_value, original_timestamp_utc, replication_timestamp_utc)
                    VALUES (?, ?, ?, ?, ?, ?)
                """, (source_id, original_id, record['object_id'], record['reading_value'], original_ts, replication_time))

                if go_cursor.rowcount > 0:
                    records_replicated += 1
                    # Обновляем latest_ts_in_batch самой последней меткой из *успешно вставленных*
                    if latest_ts_in_batch is None or original_ts > latest_ts_in_batch:
                        latest_ts_in_batch = original_ts
                    logging.debug(f"Реплицирована запись: Источник={source_id}, OrigID={original_id}, TS={original_ts}")
                else:
                    logging.warning(f"Запись из {source_id} с OrigID={original_id} уже существует (дубликат), проигнорирована.")
                    # Даже если дубликат, обновляем latest_ts_in_batch, чтобы продвинуться дальше
                    if latest_ts_in_batch is None or original_ts > latest_ts_in_batch:
                        latest_ts_in_batch = original_ts

            # Обновить последнюю метку времени, если были обработаны записи
            if latest_ts_in_batch:
                update_replication_status(go_conn, source_id, latest_ts_in_batch)
                go_conn.commit()
                all_processed_successfully = True
                logging.info(f"Успешно обработано {len(new_records)} записей от {source_id}. Вставлено новых: {records_replicated}. Статус обновлен на {latest_ts_in_batch}.")
            else:
                go_conn.rollback()
                logging.info(f"Не было валидных записей для обновления статуса {source_id}.")

            # 5. Если получили полный пакет, сразу пробуем запросить еще
            if all_processed_successfully and len(new_records) == FETCH_LIMIT:
                logging.info(f"Получен полный пакет от {source_id}")
                replicate_from_source(go_conn, source_id, source_api_url)

        except sqlite3.Error as e:
            logging.error(f"Ошибка SQLite во время записи данных от {source_id}: {e}")
            go_conn.rollback()
        except Exception as e:
            logging.error(f"Неожиданная ошибка при обработке данных от {source_id}: {e}")
            go_conn.rollback()

    except Exception as e:
        logging.error(f"Неожиданная общая ошибка во время репликации из {source_id}: {e}", exc_info=True)
        try:
            if go_conn.in_transaction:
                go_conn.rollback()
                logging.warning("Транзакция была отменена из-за общей ошибки.")
        except Exception as re:
            logging.error(f"Ошибка при попытке отката транзакции: {re}")


if __name__ == "__main__":
    logging.info("=== Репликатор (Pull HTTP) Запущен ===")

    go_connection = get_db_connection(GO_DB_FILE)
    if not go_connection:
        logging.critical("Не удалось подключиться к центральной БД ГО. Репликатор не может работать.")
        sys.exit(1)

    create_go_schema(go_connection)

    try:
        while True:
            logging.info("--- Начинаю новый цикл опроса источников ---")
            start_cycle_time = time.time()

            for source_id, source_url in TO_SOURCES.items():
                replicate_from_source(go_connection, source_id, source_url)
                time.sleep(0.5)

            end_cycle_time = time.time()
            cycle_duration = end_cycle_time - start_cycle_time
            sleep_time = max(0, REPLICATION_INTERVAL_SECONDS - cycle_duration)

            logging.info(f"--- Цикл опроса завершен за {cycle_duration:.2f} сек. Пауза {sleep_time:.2f} секунд ---")
            time.sleep(sleep_time)

    except KeyboardInterrupt:
        logging.info("Репликатор остановлен пользователем.")
    finally:
        if go_connection:
            go_connection.close()
            logging.info("Соединение с центральной БД ГО закрыто.")