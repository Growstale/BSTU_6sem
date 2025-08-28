# tests/test_login.py
import pytest

from pages.base_page import BasePage
from pages.home_page import HomePage
from pages.login_page import LoginPage
from pages.account_page import AccountPage
import time # Для демонстрации работы с куки
from pages.utils import save_cookies, load_cookies, add_cookies_to_driver


@pytest.mark.run(order=1)
@pytest.mark.auth
def test_successful_login(driver):
    print("\n--- Тест: Успешная авторизация ---")
    home_page = HomePage(driver)
    home_page.accept_cookies()

    # Демонстрация работы с куками: Вывод до логина
    print("\nКуки до авторизации:")
    cookies_before = home_page.get_cookies()
    for cookie in cookies_before:
        print(f"- {cookie}")

    home_page.open_login_form()
    login_page = LoginPage(driver)
    account_page = login_page.login("cleo2005254@gmail.com", "#")

    expected_title = "Личный аккаунт"
    actual_title = account_page.get_account_title()

    print("\nКуки после авторизации:")
    cookies_after = account_page.get_cookies()
    for cookie in cookies_after:
        if 'sessionid' in cookie['name'].lower() or 'auth' in cookie['name'].lower():
             print(f"- {cookie} (Похоже на сессионную)")
        else:
             print(f"- {cookie}")

    assert actual_title == expected_title, f"Ожидали '{expected_title}', получили '{actual_title}'"
    print(f"Проверка заголовка аккаунта '{actual_title}' прошла успешно.")

    account_page.take_screenshot("login_success.png")
    print("Тест авторизации пройден ✅")


# Пример ожидаемо падающего теста (xfail)
@pytest.mark.run(order=2)
@pytest.mark.auth
@pytest.mark.xfail(reason="Ожидаем падение при неверных данных")
def test_failed_login(driver):
    print("\n--- Тест: Неуспешная авторизация (ожидаемое падение) ---")
    home_page = HomePage(driver)
    home_page.accept_cookies()

    # Демонстрация работы с куками: Вывод до логина
    print("\nКуки до авторизации:")
    cookies_before = home_page.get_cookies()
    for cookie in cookies_before:
        print(f"- {cookie['name']}")

    home_page.open_login_form()
    login_page = LoginPage(driver)
    account_page = login_page.login("cleo2005254@gmail.com", "123")

    expected_title = "Личный аккаунт"
    actual_title = account_page.get_account_title()

    print("\nКуки после авторизации:")
    cookies_after = account_page.get_cookies()
    for cookie in cookies_after:
        # Проверяем, появились ли сессионные куки
        if 'sessionid' in cookie['name'].lower() or 'auth' in cookie['name'].lower():
             print(f"- {cookie['name']} (Похоже на сессионную)")
        else:
             print(f"- {cookie['name']}")

    assert actual_title == expected_title, f"Ожидали '{expected_title}', получили '{actual_title}'"
    print(f"Проверка заголовка аккаунта '{actual_title}' прошла успешно.")

    account_page.take_screenshot("login_success.png")
    print("Тест неуспешной авторизации (xfail) завершен.")


@pytest.mark.skip(reason="Эта функциональность еще не реализована")
@pytest.mark.auth
@pytest.mark.run(order=3)
def test_new_feature():
    assert False


@pytest.mark.cookie_setup
@pytest.mark.run(order=8)
def test_save_auth_cookies(driver):
    """
    Тест для выполнения входа и сохранения кук аутентификации.
    """
    print("\n--- Тест: Сохранение кук аутентификации ---")
    home_page = HomePage(driver)
    home_page.accept_cookies()
    home_page.open_login_form()
    login_page = LoginPage(driver)
    account_page = login_page.login("cleo2005254@gmail.com", "#")
    expected_title = "Личный аккаунт"
    try:
        account_page.click_element(account_page.USER_TOOLS_TOGGLE)
        actual_title = account_page.get_element_text(account_page.USER_TOOLS_TITLE)
        assert actual_title == expected_title
        print(f"Авторизация прошла успешно (заголовок: '{actual_title}'). Сохраняем куки...")
        # Сохраняем куки после успешного логина
        save_cookies(driver)
    except Exception as e:
        pytest.fail(f"Не удалось успешно авторизоваться для сохранения кук: {e}")
    print("Тест сохранения кук завершен.")


@pytest.mark.auth_cookie
@pytest.mark.run(order=9)
def test_login_with_cookies(driver):
    """Тест авторизации с использованием сохраненных кук."""
    print("\n--- Тест: Авторизация через куки ---")

    # 1. Загружаем куки из файла
    cookies = load_cookies()
    if cookies is None:
        pytest.skip("Файл с куками не найден или пуст. Запустите 'test_save_auth_cookies' сначала.")

    print("Открываем главную страницу...")
    home_page = BasePage(driver, url="https://www.21vek.by/") # Используем BasePage чтобы просто открыть URL

    print("Очищаем существующие куки...")
    driver.delete_all_cookies()

    print("Добавляем сохраненные куки...")
    add_cookies_to_driver(driver, cookies)

    print("Обновляем страницу...")
    driver.refresh()

    print("Ожидание после обновления...")
    time.sleep(3)

    print("Проверяем статус авторизации...")
    home_page_after_cookies = HomePage(driver)
    try:
        home_page_after_cookies.click_element(home_page_after_cookies.USER_TOOLS_TOGGLE)
        account_page = AccountPage(driver)
        actual_title = account_page.get_element_text(account_page.USER_TOOLS_TITLE)
        expected_title = "Личный аккаунт"
        print(f"Получен заголовок после входа по кукам: '{actual_title}'")

        assert actual_title == expected_title, f"Ожидали '{expected_title}', получили '{actual_title}' после входа по кукам."
        print("Авторизация с использованием кук прошла успешно! ✅")
        account_page.take_screenshot("login_via_cookies_success.png")

    except Exception as e:
        print(f"Авторизация по кукам не удалась. Ошибка: {e}")
