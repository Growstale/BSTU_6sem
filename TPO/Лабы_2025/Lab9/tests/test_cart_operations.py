# tests/test_cart_operations.py
import pytest
from pages.home_page import HomePage
from pages.login_page import LoginPage
import time


@pytest.mark.run(order=5)
@pytest.mark.cart
def test_add_and_remove_items_from_cart(driver):
    """Тест добавления двух товаров в корзину и их последующего удаления."""
    print("\n--- Тест: Добавление и удаление из корзины ---")
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
    home_page.add_first_n_items_to_cart(n=2)
    print("Товары добавлены.")

    cart_page = home_page.go_to_cart()
    print(f"Перешли в корзину: {cart_page.get_current_url()}")

    initial_items_count = cart_page.get_items_count()
    assert initial_items_count == 2, f"Ожидали 2 товара в корзине, найдено: {initial_items_count}"
    print(f"В корзине {initial_items_count} товара.")

    print("Удаляем первый товар...")
    cart_page.remove_item(index=0)
    items_after_first_removal = cart_page.get_items_count()
    assert items_after_first_removal == 1, f"Ожидали 1 товар после первого удаления, найдено: {items_after_first_removal}"
    print(f"После первого удаления в корзине {items_after_first_removal} товар.")
    cart_page.take_screenshot("cart_after_first_removal.png")

    # Удаляем оставшийся товар
    print("Удаляем второй товар...")
    cart_page.remove_item(index=0) # Он теперь снова первый (индекс 0)
    items_after_second_removal = cart_page.get_items_count()
    assert items_after_second_removal == 0, f"Ожидали 0 товаров после второго удаления, найдено: {items_after_second_removal}"
    print(f"После второго удаления в корзине {items_after_second_removal} товаров.")
    cart_page.take_screenshot("cart_after_second_removal.png")

    print("Тест добавления и удаления товаров пройден ✅")