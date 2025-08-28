import time

# модуль, который позволяет управлять браузером программно
from selenium import webdriver
from webdriver_manager.chrome import ChromeDriverManager
# класс отвечает за установку драйвера, его открытие и закрытие
from selenium.webdriver.chrome.service import Service
# класс для явных ожиданий
from selenium.webdriver.support.ui import WebDriverWait
# используется, чтобы указать способ поиска элемента
from selenium.webdriver.common.by import By

service = Service(executable_path=ChromeDriverManager().install())
# создаётся объект driver, через который мы будем управлять браузером Chrome
driver = webdriver.Chrome(service=service)
driver.maximize_window()

driver.get("https://www.21vek.by/")
driver.implicitly_wait(10)

try:
    url = driver.current_url
    current_title = driver.title
    print("URL страницы: ", url)
    print("Текущий заголовок: ", current_title)

    # === By.ID ===
    search_input = driver.find_element(By.ID, "catalogSearch")
    print("Найден элемент по ID. Tag: ", search_input.tag_name, ". Placeholder: ", search_input.get_attribute("placeholder"))

    # === By.NAME ===
    search_input_name = driver.find_element(By.NAME, "email")
    print("Найден элемент по NAME. Tag: ", search_input_name.tag_name, ". Placeholder: ", search_input_name.get_attribute("placeholder"))

    # === CSS-селектор: составной 1 ===
    span = driver.find_element(By.CSS_SELECTOR, "div.styles_headerReactWrapper__TTCde button#catalogButton span")
    print("CSS-селектор (1):", span.text)

    # === CSS-селектор: составной 2 ===
    img = driver.find_element(By.CSS_SELECTOR, "div[class^='Banners'] img[alt='Xiaomi']")
    print("CSS-селектор (2):", img.get_attribute("src"))

    # === XPath: составной 1 ===
    xpath_elem1 = driver.find_element(By.XPATH, "//header[@id='header']//span[contains(text(), 'г.')]")
    print("XPath (1):", xpath_elem1.text)

    # === XPath: составной 2 ===
    xpath_elem2 = driver.find_element(By.XPATH, "//input[@type='text' and contains(@class, 'search')]")
    print("XPath (2):", xpath_elem2.get_attribute("placeholder"))

    # === По частичному тексту ссылки ===
    partial_link = driver.find_element(By.PARTIAL_LINK_TEXT, "Розыгрыш")
    print("Найдена ссылка по частичному тексту:", partial_link.text)

    # === Найти список элементов ===
    product_list = driver.find_elements(By.XPATH, "//div[contains(@class, 'PopularsList')]")

    print(f"Найдено товаров: {len(product_list)}")
    for product in product_list:
        print(" -", product.get_attribute("data-id"))


except Exception as e:
    print("Ошибка:", e)

finally:
    input("\nНажмите Enter для выхода и закрытия браузера...")
    driver.quit()