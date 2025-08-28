# pages/payment_page.py
from selenium.webdriver.common.by import By
from .base_page import BasePage
from selenium.webdriver.support import expected_conditions as EC


class PaymentPage(BasePage):
    PAYMENT_URL_PART = '/order/?step=payment'
    CASH_PAYMENT_CONFIRMATION = (By.CSS_SELECTOR, 'div[aria-label*="Выбрана оплата"]')

    def __init__(self, driver):
        super().__init__(driver)
        self.wait.until(EC.url_contains(self.PAYMENT_URL_PART))

    def get_payment_confirmation_text(self):
        element = self.find_element(self.CASH_PAYMENT_CONFIRMATION)
        return element.get_attribute("aria-label")