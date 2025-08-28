import pytest
from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver.chrome.options import Options as ChromeOptions
from selenium.webdriver.firefox.options import Options as FirefoxOptions
from webdriver_manager.firefox import GeckoDriverManager


# Добавление кастомных опций командной строки
def pytest_addoption(parser):
    parser.addoption("--browser", action="store", default="chrome", help="Браузер для запуска тестов: "
                                                                         "chrome или firefox")
    parser.addoption("--headless", action="store_true", default=False,
                     help="Запуск браузера в headless режиме (без графического интерфейса)")


@pytest.fixture(scope="function")
def driver(request):
    # 'request' - специальный объект pytest, через который можно получить доступ к
    # контексту теста, включая опции командной строки
    browser_name = request.config.getoption("browser")
    headless_mode = request.config.getoption("headless")
    print(f"\nЗапуск браузера: {browser_name}, Headless: {headless_mode}")

    driver_instance = None

    if browser_name.lower() == "chrome":
        options = ChromeOptions()
        if headless_mode:
            options.add_argument("--headless")
            options.add_argument("--window-size=1920,1080")
        options.add_argument("--incognito")
        options.add_argument("--disable-gpu")
        options.add_argument("--no-sandbox")
        options.add_argument("--disable-dev-shm-usage")
        service = Service(executable_path=ChromeDriverManager().install())
        # Создаем экземпляр браузера Chrome с указанными настройками и сервисом
        driver_instance = webdriver.Chrome(service=service, options=options)

    elif browser_name.lower() == "firefox":
        options = FirefoxOptions()
        if headless_mode:
            options.add_argument("--headless")
        options.add_argument("-private")
        service = Service(executable_path=GeckoDriverManager().install())
        driver_instance = webdriver.Firefox(service=service, options=options)

    else:
        raise pytest.UsageError(f"--browser должен быть chrome или firefox, получено: {browser_name}")

    driver_instance.maximize_window()
    yield driver_instance # Передаем драйвер в тест

    # Teardown - выполняется после теста
    print("\nЗакрытие браузера")
    driver_instance.quit()