# pages/login_page.py
from selenium.webdriver.common.by import By
from .base_page import BasePage


class LoginPage(BasePage):
    USERNAME_INPUT = (By.ID, "login-email")
    PASSWORD_INPUT = (By.ID, "login-password")
    SUBMIT_BUTTON = (By.CSS_SELECTOR, "[data-testid='loginSubmit']")

    # Этот локатор нужен, чтобы дождаться исчезновения формы после логина
    LOGIN_FORM_IDENTIFIER = USERNAME_INPUT

    def __init__(self, driver):
        super().__init__(driver)

    def login(self, username, password):
        self.send_keys_to_element(self.USERNAME_INPUT, username)
        self.send_keys_to_element(self.PASSWORD_INPUT, password)
        login_form_element = self.find_element(self.LOGIN_FORM_IDENTIFIER)
        self.click_element(self.SUBMIT_BUTTON)
        self.wait_for_staleness(login_form_element)
        # Возвращаем объект главной страницы или страницы аккаунта
        from .account_page import AccountPage
        return AccountPage(self.driver)