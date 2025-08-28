import requests # Библиотека для отправки веб-запросов
import time
import logging
from datetime import datetime, timezone

logging.basicConfig(level=logging.INFO, format='%(asctime)s --- %(message)s')

SOURCE1_URL_BASE = "http://localhost:5001"
SOURCE2_URL_BASE = "http://localhost:5002"

# Сервисы-источники
sources_to_poll = [
    {"id": "ТИ_1", "url": SOURCE1_URL_BASE},
    {"id": "ТИ_2", "url": SOURCE2_URL_BASE},
]

# Словарь для хранения последнего известного состояния источников
sources_status = {}


# Опрос одного источника
def poll_source(source_info):
    source_id = source_info["id"]
    base_url = source_info["url"]
    status_url = f"{base_url}/status"
    data_url = f"{base_url}/data"

    logging.info(f"Начинаю опрос источника: {source_id} по адресу {base_url}")

    # для хранения результатов текущего опроса этого конкретного источника
    current_status = {
        "status": "UNKNOWN",
        "last_data": None,
        "last_error": None,
        "last_polled_utc": datetime.now(timezone.utc).isoformat()
    }

    # --- 1. Проверяем статус ---
    try:
        response = requests.get(status_url, timeout=3)

        if response.status_code == 200:
            status_data = response.json()
            current_status["status"] = status_data.get("status", "ОТВЕТ НЕ ПОНЯТЕН")
            logging.info(f"Источник {source_id}: Статус получен - {current_status['status']}")
        elif response.status_code == 503:
             current_status["status"] = "ВРЕМЕННО НЕДОСТУПЕН (503)"
             current_status["last_error"] = "Сервис сообщил о временной недоступности."
             logging.warning(f"Источник {source_id}: Сервис сообщил о временной недоступности (503).")
        else:
            current_status["status"] = f"ОШИБКА HTTP {response.status_code}"
            current_status["last_error"] = f"Ошибка при запросе статуса: {response.status_code}"
            logging.error(f"Источник {source_id}: Ошибка HTTP при запросе статуса: {response.status_code}")

    except requests.exceptions.Timeout:
        # Если источник не ответил за 3 секунды
        current_status["status"] = "НЕ ОТВЕЧАЕТ (Таймаут)"
        current_status["last_error"] = "Источник не ответил на запрос статуса вовремя."
        logging.error(f"Источник {source_id}: Не отвечает (таймаут) при запросе статуса.")
    except requests.exceptions.ConnectionError:
        # Если не удалось даже соединиться с источником
        current_status["status"] = "НЕ ДОСТУПЕН (Ошибка соединения)"
        current_status["last_error"] = "Не удалось подключиться к источнику для запроса статуса."
        logging.error(f"Источник {source_id}: Не доступен (ошибка соединения) при запросе статуса.")
    except Exception as e:
        current_status["status"] = "НЕИЗВЕСТНАЯ ОШИБКА СТАТУСА"
        current_status["last_error"] = f"Неожиданная ошибка при запросе статуса: {e}"
        logging.exception(f"Источник {source_id}: Неожиданная ошибка при запросе статуса!") # Выведет полную ошибку

    # --- 2. Запрашиваем данные ---
    if current_status["status"] not in ["НЕ ОТВЕЧАЕТ (Таймаут)", "НЕ ДОСТУПЕН (Ошибка соединения)"]:
        try:
            response = requests.get(data_url, timeout=3)
            if response.status_code == 200:
                current_status["last_data"] = response.json()
                logging.info(f"Источник {source_id}: Данные получены: {current_status['last_data']}")
            else:
                error_msg = f"Ошибка при запросе данных: HTTP {response.status_code}"
                current_status["last_error"] = error_msg
                if current_status["status"] == "OK":
                    current_status["status"] = f"ОШИБКА ДАННЫХ {response.status_code}"
                logging.error(f"Источник {source_id}: {error_msg}")

        except requests.exceptions.Timeout:
            error_msg = "Источник не ответил на запрос данных вовремя."
            current_status["last_error"] = error_msg
            if current_status["status"] != "НЕ ОТВЕЧАЕТ (Таймаут)":
                current_status["status"] = "НЕ ОТВЕЧАЕТ (Таймаут данных)"
            logging.error(f"Источник {source_id}: {error_msg}")
        except requests.exceptions.ConnectionError:
            error_msg = "Не удалось подключиться к источнику для запроса данных."
            current_status["last_error"] = error_msg
            if current_status["status"] != "НЕ ДОСТУПЕН (Ошибка соединения)":
                current_status["status"] = "НЕ ДОСТУПЕН (Ошибка соединения данных)"
            logging.error(f"Источник {source_id}: {error_msg}")
        except Exception as e:
            error_msg = f"Неожиданная ошибка при запросе данных: {e}"
            current_status["last_error"] = error_msg
            current_status["status"] = "НЕИЗВЕСТНАЯ ОШИБКА ДАННЫХ"
            logging.exception(f"Источник {source_id}: Неожиданная ошибка при запросе данных!")

    sources_status[source_id] = current_status
    logging.info(f"Опрос источника {source_id} завершен. Текущее состояние: {current_status['status']}")


# --- Основной цикл программы ---
if __name__ == "__main__":
    logging.info("=== Центральный Опросчик Запущен ===")
    while True:
        logging.info("--- Начинаю новый цикл опроса ---")

        for source in sources_to_poll:
            poll_source(source)
            time.sleep(1)

        print("\n--- Текущее состояние источников ---")
        for source_id, status_info in sources_status.items():
            print(f"Источник: {source_id}")
            print(f"  Статус:       {status_info['status']}")
            print(f"  Последние данные: {status_info['last_data']}")
            print(f"  Последняя ошибка: {status_info['last_error']}")
            print(f"  Время опроса: {status_info['last_polled_utc']}")
        print("-----------------------------------\n")

        logging.info(f"--- Цикл опроса завершен. Пауза 10 секунд ---")
        time.sleep(10)
