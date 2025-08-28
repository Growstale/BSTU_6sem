import time
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.chrome.service import Service
from webdriver_manager.chrome import ChromeDriverManager
from selenium.common.exceptions import TimeoutException

service = Service(executable_path=ChromeDriverManager().install())
driver = webdriver.Chrome(service=service)
driver.maximize_window()

wait = WebDriverWait(driver, 10)
driver.get("https://demoqa.com/radio-button")


def check_radio_button(option_id, expected_text):
    label_selector = (By.CSS_SELECTOR, f"label[for='{option_id}']")
    label = wait.until(EC.presence_of_element_located(label_selector))

    try:
        driver.execute_script("arguments[0].scrollIntoView(true);", label)
        time.sleep(0.5)
        print(f"Попытка клика на '{option_id}' через JavaScript...")
        driver.execute_script("arguments[0].click();", label)
    except Exception as e:
        print(f"Не удалось кликнуть на {option_id} через JavaScript: {e}")
        raise

    result_selector = (By.CLASS_NAME, "text-success")
    try:
        wait.until(EC.text_to_be_present_in_element(result_selector, expected_text))
    except TimeoutException:
        current_text = "Элемент не найден"
        try:
            current_text = driver.find_element(*result_selector).text
        except:
            pass
        print(f"Ошибка ожидания текста '{expected_text}'. Текущий текст: '{current_text}'")
        raise

    result = driver.find_element(*result_selector)

    print(f"✔ Кликнули '{option_id}', получили: '{result.text}'")
    assert result.text == expected_text, f"Ожидали '{expected_text}', получили '{result.text}'"


check_radio_button("yesRadio", "Yes")
check_radio_button("impressiveRadio", "Impressive")

no_radio_input_selector = (By.ID, "noRadio")
no_radio = wait.until(EC.presence_of_element_located(no_radio_input_selector))

assert not no_radio.is_enabled(), "❌ 'No' radio button должна быть активной (ошибка в проверке!)"
print("✔ 'No' радио-кнопка отключена")

driver.quit()