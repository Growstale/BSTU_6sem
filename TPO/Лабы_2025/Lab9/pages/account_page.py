# pages/account_page.py
from selenium.webdriver.common.by import By
from .home_page import HomePage


class AccountPage(HomePage):
    USER_TOOLS_TITLE = (By.CLASS_NAME, "userToolsTitle")

    def __init__(self, driver):
        super().__init__(driver)

    def get_account_title(self):
        self.click_element(self.USER_TOOLS_TOGGLE)
        return self.get_element_text(self.USER_TOOLS_TITLE)