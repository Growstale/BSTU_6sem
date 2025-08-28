# pages/demoqa_radio_page.py
from selenium.common import TimeoutException
from selenium.webdriver.common.by import By
from .base_page import BasePage
from selenium.webdriver.support import expected_conditions as EC
import time


class DemoQARadioPage(BasePage):
    URL = "https://demoqa.com/radio-button"
    YES_RADIO_LABEL = (By.CSS_SELECTOR, "label[for='yesRadio']")
    IMPRESSIVE_RADIO_LABEL = (By.CSS_SELECTOR, "label[for='impressiveRadio']")
    NO_RADIO_INPUT = (By.ID, "noRadio")
    RESULT_TEXT = (By.CLASS_NAME, "text-success")

    def __init__(self, driver):
        super().__init__(driver, self.URL)

    def select_radio_button(self, option_id):
        """Кликает на label радио-кнопки по for атрибуту."""
        label_locator = (By.CSS_SELECTOR, f"label[for='{option_id}']")
        label = self.find_element(label_locator)
        self.scroll_into_view(label_locator)
        time.sleep(0.5)
        # Используем JS клик, если обычный не сработает
        try:
            self.click_element(label_locator)
        except Exception as e:
            print(f"Обычный клик не удался для {option_id}, пробую JS клик: {e}")
            self.execute_script("arguments[0].click();", label)

    def get_result_text(self):
        try:
            return self.get_element_text(self.RESULT_TEXT)
        except TimeoutException:
            return "Результат не найден"

    def is_no_radio_enabled(self):
        no_radio = self.find_element(self.NO_RADIO_INPUT, EC.presence_of_element_located)
        return no_radio.is_enabled()