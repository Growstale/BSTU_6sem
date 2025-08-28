# pages/checkout_page.py
from selenium.webdriver.common.by import By
from .base_page import BasePage

class CheckoutPage(BasePage):
    DELIVERY_URL_PART = "step=delivery"
    PAYMENT_URL_PART = "step=payment"

    # Delivery Step
    DELIVERY_SUBMIT_BUTTON = (By.CSS_SELECTOR, 'button[data-testid="delivery-submit"]')

    # Payment Step
    CASH_PAYMENT_CONFIRMATION = (By.CSS_SELECTOR, 'div[aria-label^="Выбрана оплата при получении"]') # Example for cash

    def __init__(self, driver):
        super().__init__(driver)

    def confirm_delivery(self):
        self.wait_for_url_change(self.DELIVERY_URL_PART)
        self.click_element(self.DELIVERY_SUBMIT_BUTTON)
        self.wait_for_url_change(self.PAYMENT_URL_PART) # Wait for payment step

    def is_cash_payment_selected(self) -> bool:
        self.wait_for_url_change(self.PAYMENT_URL_PART)
        return self.is_element_present(self.CASH_PAYMENT_CONFIRMATION)

    def get_payment_confirmation_text(self) -> str:
         element = self.find_element(self.CASH_PAYMENT_CONFIRMATION)
         return element.get_attribute("aria-label")