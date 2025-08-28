import time
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.chrome.service import Service
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver.common.keys import Keys

service = Service(executable_path=ChromeDriverManager().install())
driver = webdriver.Chrome(service=service)
driver.maximize_window()

driver.get("https://www.21vek.by/")
wait = WebDriverWait(driver, 10)

accept_button = wait.until(
    EC.element_to_be_clickable((By.CSS_SELECTOR, '.Button-module__button.Button-module__blue-primary')))
accept_button.click()

search_input = wait.until(EC.presence_of_element_located((By.ID, "catalogSearch")))
search_input.send_keys("Шкафы")
search_input.click()

search_button_locator = (By.CSS_SELECTOR, "[class^='Search_searchBtn']")
search_button = wait.until(EC.element_to_be_clickable(search_button_locator))
search_button.click()

content_div = WebDriverWait(driver, 10).until(
    EC.presence_of_element_located((By.ID, "content"))
)

product_divs = content_div.find_elements(By.CSS_SELECTOR, "div[data-id^='product']")
assert len(product_divs) >= 8, f"Ожидалось найти 8 элементов, но найдено {len(product_divs)}"

for product in product_divs[:8]:
    card_link = product.find_element(By.CSS_SELECTOR, 'a[data-testid="card-info-a"]')
    card_text = card_link.text
    assert card_text.startswith("Шкаф"), f"Ожидалось, что текст будет начинаться с 'Шкаф', но найден {card_text}"

print("Тест-кейс 2 пройден ✅")

driver.quit()
