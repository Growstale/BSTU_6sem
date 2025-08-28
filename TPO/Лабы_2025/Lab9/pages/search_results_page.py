# pages/search_results_page.py
from selenium.webdriver.common.by import By
from .base_page import BasePage


class SearchResultsPage(BasePage):
    PRODUCT_ITEMS = (By.CSS_SELECTOR, "div[data-id^='product']")
    PRODUCT_TITLE_LINK = (By.CSS_SELECTOR, 'a[data-testid="card-info-a"]') # Внутри PRODUCT_ITEMS

    def __init__(self, driver):
        super().__init__(driver)

    def get_product_titles(self, limit=None):
        product_elements = self.find_elements(self.PRODUCT_ITEMS)
        titles = []
        count = 0
        for product in product_elements:
            if limit is not None and count >= limit:
                break
            try:
                title_link = product.find_element(*self.PRODUCT_TITLE_LINK)
                titles.append(title_link.text)
                count += 1
            except Exception as e:
                print(f"Не удалось получить заголовок для продукта: {e}")
        return titles

    def get_products_count(self):
        return len(self.find_elements(self.PRODUCT_ITEMS))