# pages/delivery_page.py
from selenium.webdriver.common.by import By
from .base_page import BasePage
from selenium.webdriver.support import expected_conditions as EC


class DeliveryPage(BasePage):
    DELIVERY_URL_PART = 'https://www.21vek.by/order/?step=delivery'
    DELIVERY_SUBMIT_BUTTON = (By.CSS_SELECTOR, 'button[data-testid="delivery-submit"]')

    def __init__(self, driver):
        super().__init__(driver)
        self.wait_for_url_to_be(self.DELIVERY_URL_PART)

    def proceed_to_payment(self):
        self.click_element(self.DELIVERY_SUBMIT_BUTTON)
        from .payment_page import PaymentPage
        return PaymentPage(self.driver)