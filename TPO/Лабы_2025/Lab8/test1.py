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

driver.quit()
