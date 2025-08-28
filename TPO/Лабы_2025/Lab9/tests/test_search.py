# tests/test_search.py
import pytest
from pages.home_page import HomePage

# Параметризация теста
search_data = [
    ("Шкафы", "Шкаф", 8),
    ("Столы", "Стол", 7),
    ("Телевизоры", "Телевизор", 6),
]


@pytest.mark.run(order=4)
@pytest.mark.search
@pytest.mark.parametrize("query, expected_prefix, amount", search_data)
def test_product_search_results(driver, query, expected_prefix, amount):
    """Тест поиска товаров и проверки заголовков в результатах."""
    print(f"\n--- Тест: Поиск по запросу '{query}' ---")
    home_page = HomePage(driver)
    home_page.accept_cookies()

    search_results_page = home_page.search_for(query)
    print(f"Перешли на страницу результатов поиска: {search_results_page.get_current_url()}")

    products_count = search_results_page.get_products_count()
    assert products_count > 0, f"По запросу '{query}' не найдено ни одного товара."
    print(f"Найдено товаров: {products_count}")

    titles = search_results_page.get_product_titles(limit=amount)
    print(f"Первые {len(titles)} заголовков: {titles}")
    assert len(titles) > 0, "Не удалось получить заголовки товаров."

    # Проверяем, что заголовки начинаются с ожидаемого префикса
    for i, title in enumerate(titles):
        assert expected_prefix.lower() in title.lower(), \
            f"Ожидаемое слово '{expected_prefix}' не найдено в заголовке '{title}' " \
            f"(продукт #{i+1}) для запроса '{query}'"

    print(f"Проверка заголовков для запроса '{query}' прошла успешно.")
    search_results_page.take_screenshot(f"search_{query}.png")
    print(f"Тест поиска для '{query}' пройден ✅")