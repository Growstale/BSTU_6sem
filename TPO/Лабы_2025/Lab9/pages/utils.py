# utils.py
import json
import os

COOKIES_DIR = "cookies"
AUTH_COOKIE_FILE = os.path.join(COOKIES_DIR, "auth_cookies.json")


def save_cookies(driver, filename=AUTH_COOKIE_FILE):
    """Сохраняет куки из текущей сессии драйвера в JSON файл."""
    if not os.path.exists(COOKIES_DIR):
        os.makedirs(COOKIES_DIR)
    cookies = driver.get_cookies()
    with open(filename, 'w') as file:
        json.dump(cookies, file, indent=4)
    print(f"Куки сохранены в {filename}")


def load_cookies(filename=AUTH_COOKIE_FILE):
    """Загружает куки из JSON файла."""
    if not os.path.exists(filename):
        print(f"Файл с куками {filename} не найден.")
        return None
    try:
        with open(filename, 'r') as file:
            cookies = json.load(file)
        print(f"Куки загружены из {filename}")
        return cookies
    except Exception as e:
        print(f"Не удалось загрузить куки из {filename}: {e}")
        return None


def add_cookies_to_driver(driver, cookies):
    """Добавляет загруженные куки в драйвер."""
    if not cookies:
        print("Нет кук для добавления.")
        return
    import copy
    cookies_copy = copy.deepcopy(cookies)

    for cookie in cookies_copy:
        try:
            if 'expiry' in cookie and isinstance(cookie['expiry'], float):
                cookie['expiry'] = int(cookie['expiry'])

            driver.add_cookie(cookie)
        except Exception as e:
            print(f"Не удалось добавить куку: {cookie.get('name', cookie)}. Ошибка: {e}")
            if 'expiry' in cookie:
                try:
                    print(f"Попытка добавить {cookie.get('name')} без expiry...")
                    cookie.pop('expiry')
                    driver.add_cookie(cookie)
                except Exception as e2:
                    print(f"Повторная попытка добавить куку {cookie.get('name')} не удалась: {e2}")

    print(f"Завершена попытка добавления кук.")