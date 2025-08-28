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

user_tools_button = wait.until(EC.element_to_be_clickable((By.CSS_SELECTOR, "[class*='userToolsToggle']")))
user_tools_button.click()

login_button = wait.until(EC.element_to_be_clickable((By.CSS_SELECTOR, "[data-testid='loginButton']")))
login_button.click()

username = wait.until(EC.presence_of_element_located((By.ID, "login-email")))
password = driver.find_element(By.ID, "login-password")
submit_button = driver.find_element(By.CSS_SELECTOR, "[data-testid='loginSubmit']")

username.send_keys("cleo2005254@gmail.com")
password.send_keys("Hello2612677")
submit_button.click()

wait.until(EC.staleness_of(username))

user_tools_button = wait.until(EC.element_to_be_clickable((By.CSS_SELECTOR, "[class*='userToolsToggle']")))
user_tools_button.click()

user_tools_title = wait.until(EC.visibility_of_element_located((By.CLASS_NAME, "userToolsTitle")))

actual_title = user_tools_title.text
expected_title = "Личный аккаунт"
assert actual_title == expected_title, f"Ожидали '{expected_title}', а получили '{actual_title}'"

print("Тест авторизации пройден ✅")

# тест-кейс 1


buttons = wait.until(EC.presence_of_all_elements_located((By.CSS_SELECTOR, '[data-testid="card-basket-action"]')))
assert len(buttons) >= 2, "Не найдено два элемента для добавления в корзину"
buttons[0].click()
buttons[1].click()

header_cart_box = WebDriverWait(driver, 10).until(
    EC.element_to_be_clickable((By.CSS_SELECTOR, '.headerCartBox'))
)
header_cart_box.click()

WebDriverWait(driver, 10).until(EC.url_to_be('https://www.21vek.by/order/'))
assert driver.current_url == 'https://www.21vek.by/order/', f"Ожидался URL https://www.21vek.by/order/, но перешли на {driver.current_url}"

basket_container = WebDriverWait(driver, 10).until(
    EC.presence_of_element_located((By.CSS_SELECTOR, '[data-testid="basket-container"]'))
)
basket_items = basket_container.find_elements(By.CSS_SELECTOR, '[class^="BasketItem_item_"]')
assert len(basket_items) == 2, f"Ожидалось 2 элемента, но найдено {len(basket_items)}"

delete_button = basket_items[0].find_element(By.CSS_SELECTOR, '[aria-label="Удалить товар"]')
delete_button.click()

button_text = WebDriverWait(driver, 10).until(
    EC.element_to_be_clickable((By.CSS_SELECTOR, '[data-testid="modal-confirmation-button"]'))
)
button_text.click()

wait.until(EC.staleness_of(button_text))


WebDriverWait(driver, 10).until(
    EC.presence_of_element_located((By.CSS_SELECTOR, '[data-testid="basket-container"]'))
)
basket_items = basket_container.find_elements(By.CSS_SELECTOR, '[class^="BasketItem_item_"]')
assert len(basket_items) == 1, f"Ожидался 1 элемент, но найдено {len(basket_items)}"

delete_button = basket_items[0].find_element(By.CSS_SELECTOR, '[aria-label="Удалить товар"]')
delete_button.click()

button_text = WebDriverWait(driver, 10).until(
    EC.element_to_be_clickable((By.CSS_SELECTOR, '[data-testid="modal-confirmation-button"]'))
)

button_text.click()

wait.until(EC.staleness_of(button_text))

basket_items = basket_container.find_elements(By.CSS_SELECTOR, '[class^="BasketItem_item_"]')
assert len(basket_items) == 0, f"Ожидалось 0 элементов, но найдено {len(basket_items)}"

print("Тест-кейс 1 пройден ✅")

driver.quit()
