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


driver.get("https://www.21vek.by/")


basket_buttons = wait.until(EC.presence_of_all_elements_located((By.CSS_SELECTOR, 'button[data-testid="card-basket-action"]')))
basket_buttons[0].click()
basket_buttons = wait.until(EC.presence_of_all_elements_located((By.CSS_SELECTOR, 'button[data-testid="card-basket-action"]')))
basket_buttons[0].click()

wait.until(EC.url_to_be("https://www.21vek.by/order/"))

basket_items = wait.until(EC.presence_of_all_elements_located((By.CSS_SELECTOR, 'div[data-testid="basket-container"] div[class^="BasketItem_item_"]')))
assert len(basket_items) == 1, f"Ожидался один элемент BasketItem_item_, найдено: {len(basket_items)}"

continue_button = wait.until(EC.element_to_be_clickable((By.CLASS_NAME, "Button-module__buttonText")))
continue_button.click()

wait.until(EC.url_to_be("https://www.21vek.by/order/?step=delivery"))

delivery_submit_button = wait.until(EC.element_to_be_clickable((By.CSS_SELECTOR, 'button[data-testid="delivery-submit"]')))
delivery_submit_button.click()

wait.until(EC.url_to_be("https://www.21vek.by/order/?step=payment"))

confirmation_div = wait.until(EC.presence_of_element_located((
    By.CSS_SELECTOR,
    'div[aria-label^="Выбрана оплата при получении"]'
)))

assert confirmation_div is not None, "Не найден div с подтверждением выбранной оплаты наличными"

aria_label_text = confirmation_div.get_attribute("aria-label")
print(f"Подтверждение оплаты: {aria_label_text}")

print("Тест на оформление заказа и выбор оплаты прошёл успешно ✅")
driver.quit()
