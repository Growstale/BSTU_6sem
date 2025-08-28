# tests/test_checkout.py
import pytest
from pages.home_page import HomePage
from pages.login_page import LoginPage # Нужен для логина перед оформлением
import time


@pytest.mark.run(order=6)
@pytest.mark.checkout
def test_checkout_process_to_payment(driver):
    """Тест процесса оформления заказа до выбора способа оплаты."""
    print("\n--- Тест: Оформление заказа до выбора оплаты ---")

    home_page = HomePage(driver)
    home_page.accept_cookies()

    home_page.open_login_form()
    login_page = LoginPage(driver)
    account_page = login_page.login("cleo2005254@gmail.com", "#")

    expected_title = "Личный аккаунт"
    actual_title = account_page.get_account_title()

    assert actual_title == expected_title, f"Ожидали '{expected_title}', получили '{actual_title}'"
    print(f"Проверка заголовка аккаунта '{actual_title}' прошла успешно.")

    print("Добавляем 2 товара в корзину...")
    home_page.add_first_n_items_to_cart(n=1)
    print("Товары добавлены.")

    cart_page = home_page.go_to_cart()
    print(f"Перешли в корзину: {cart_page.get_current_url()}")

    initial_items_count = cart_page.get_items_count()
    assert initial_items_count == 1, f"Ожидали 1 товар в корзине, найдено: {initial_items_count}"
    print(f"В корзине {initial_items_count} товара.")

    print("Переход к выбору доставки...")
    delivery_page = cart_page.proceed_to_checkout()
    print(f"Перешли на страницу доставки: {delivery_page.get_current_url()}")
    delivery_page.take_screenshot("checkout_delivery_step.png")

    print("Переход к выбору оплаты...")
    payment_page = delivery_page.proceed_to_payment()
    print(f"Перешли на страницу оплаты: {payment_page.get_current_url()}")
    payment_page.take_screenshot("checkout_payment_step.png")

    confirmation_text = payment_page.get_payment_confirmation_text()
    print(f"Текст подтверждения способа оплаты: '{confirmation_text}'")
    assert "при получении" in confirmation_text.lower() or "наличными" in confirmation_text.lower(), \
        f"Не найдено подтверждение оплаты наличными в тексте: '{confirmation_text}'"
    print("Подтверждение способа оплаты по умолчанию найдено.")

    print("Тест оформления заказа до шага оплаты пройден ✅")