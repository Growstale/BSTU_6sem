from selenium.common import TimeoutException
from selenium.webdriver.common.by import By
from .base_page import BasePage


class HomePage(BasePage):
    ACCEPT_COOKIES_BUTTON = (By.CSS_SELECTOR, '.Button-module__button.Button-module__blue-primary')
    USER_TOOLS_TOGGLE = (By.CSS_SELECTOR, "[class*='userToolsToggle']")
    LOGIN_BUTTON_MODAL = (By.CSS_SELECTOR, "[data-testid='loginButton']")
    SEARCH_INPUT = (By.ID, "catalogSearch")
    SEARCH_BUTTON = (By.CSS_SELECTOR, "[class^='Search_searchBtn']")
    ADD_TO_CART_BUTTONS = (By.CSS_SELECTOR, 'button[data-testid="card-basket-action"]')
    HEADER_CART_LINK = (By.CSS_SELECTOR, '.headerCartBox')

    def __init__(self, driver):
        super().__init__(driver, "https://www.21vek.by/")

    def accept_cookies(self):
        try:
            self.click_element(self.ACCEPT_COOKIES_BUTTON)
            print("Куки приняты.")
        except TimeoutException:
            print("Кнопка принятия куки не найдена или уже приняты.")

    def open_login_form(self):
        self.click_element(self.USER_TOOLS_TOGGLE)
        self.click_element(self.LOGIN_BUTTON_MODAL)

    def search_for(self, query):
        self.send_keys_to_element(self.SEARCH_INPUT, query)
        self.click_element(self.SEARCH_INPUT)
        self.click_element(self.SEARCH_BUTTON)
        # Возвращаем объект страницы результатов поиска
        from .search_results_page import SearchResultsPage
        return SearchResultsPage(self.driver)

    def add_first_n_items_to_cart(self, n=2):
        for i in range(n):
            print(f"Добавление товара #{i+1} из {n} (индекс кнопки {i})...")
            current_buttons = self.find_elements(self.ADD_TO_CART_BUTTONS)
            if len(current_buttons) <= i:
                raise IndexError(
                    f"Не удалось найти кнопку с индексом {i} для добавления товара #{i+1}. "
                    f"Найдено только {len(current_buttons)} кнопок на этой итерации."
                )
            button_to_click = current_buttons[i]
            try:
                # Используем JS-клик
                self.execute_script("arguments[0].scrollIntoView(true);", button_to_click)
                import time
                time.sleep(0.5)
                self.execute_script("arguments[0].click();", button_to_click)
                print(f"Успешно кликнули по кнопке #{i+1} (индекс {i}).")

            except Exception as e:
                self.take_screenshot(f"error_clicking_button_index_{i}.png")
                print(f"Ошибка при клике на кнопку с индексом {i}: {e}")
                raise
            import time
            time.sleep(2)

        print(f"Успешно добавлено (или сделана попытка добавить) {n} товаров.")

    def go_to_cart(self):
        self.click_element(self.HEADER_CART_LINK)
        from .cart_page import CartPage
        return CartPage(self.driver)