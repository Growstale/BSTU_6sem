# pages/cart_page.py
from selenium.webdriver.common.by import By
from .base_page import BasePage
from selenium.common.exceptions import NoSuchElementException, TimeoutException


class CartPage(BasePage):
    CART_URL = 'https://www.21vek.by/order/'
    BASKET_CONTAINER = (By.CSS_SELECTOR, '[data-testid="basket-container"]')
    BASKET_ITEMS = (By.CSS_SELECTOR, '[class^="BasketItem_item_"]') # Внутри BASKET_CONTAINER
    DELETE_ITEM_BUTTON = (By.CSS_SELECTOR, '[aria-label="Удалить товар"]') # Внутри BASKET_ITEMS
    CONFIRM_DELETE_BUTTON = (By.CSS_SELECTOR, '[data-testid="modal-confirmation-button"]')
    PROCEED_CHECKOUT_BUTTON = (By.CLASS_NAME, "Button-module__buttonText") # Может быть неточным, уточнить

    def __init__(self, driver):
        super().__init__(driver)
        self.wait_for_url_to_be(self.CART_URL)

    def get_items_count(self):
        try:
            container = self.find_element(self.BASKET_CONTAINER, timeout=5)
            items = container.find_elements(*self.BASKET_ITEMS)
            return len(items)
        except TimeoutException:
             return 0
        except NoSuchElementException:
            return 0

    def remove_item(self, index=0):
        items = self.find_elements(self.BASKET_ITEMS)
        if not items or index >= len(items):
            raise IndexError(f"Невозможно удалить элемент с индексом {index}, всего элементов: {len(items)}")

        item_to_delete = items[index]
        delete_button = item_to_delete.find_element(*self.DELETE_ITEM_BUTTON)
        item_element_id = item_to_delete.get_attribute('data-item_id')
        print(f"Удаляем товар с ID (примерно): {item_element_id}")

        delete_button.click()
        self.click_element(self.CONFIRM_DELETE_BUTTON)

        # Ждем, пока количество элементов не уменьшится (более надежно)
        expected_count = len(items) - 1
        try:
            self.wait.until(lambda d: self.get_items_count() == expected_count)
            print(f"Количество товаров обновилось до {expected_count}.")
        except TimeoutException:
            print(f"Предупреждение: Количество товаров не обновилось до {expected_count} за {self.timeout} сек.")

    def proceed_to_checkout(self):
        self.click_element(self.PROCEED_CHECKOUT_BUTTON)
        from .delivery_page import DeliveryPage
        return DeliveryPage(self.driver)