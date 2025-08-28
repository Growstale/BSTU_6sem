# tests/test_demoqa_radio.py
import pytest
from pages.demoqa_radio_page import DemoQARadioPage


@pytest.mark.run(order=7)
@pytest.mark.demoqa
class TestDemoQARadioButtons:

    @pytest.fixture(autouse=True)
    def setup(self, driver):
        """Фикстура для инициализации страницы перед каждым тестом в классе."""
        self.radio_page = DemoQARadioPage(driver)

    def test_select_yes_radio(self):
        """Тест выбора радио-кнопки 'Yes'."""
        print("\n--- Тест: DemoQA - Выбор 'Yes' ---")
        self.radio_page.select_radio_button("yesRadio")
        result = self.radio_page.get_result_text()
        assert result == "Yes", f"Ожидали результат 'Yes', получили '{result}'"
        print(f"Результат '{result}' корректен.")
        self.radio_page.take_screenshot("demoqa_radio_yes.png")

    def test_select_impressive_radio(self):
        """Тест выбора радио-кнопки 'Impressive'."""
        print("\n--- Тест: DemoQA - Выбор 'Impressive' ---")
        self.radio_page.select_radio_button("impressiveRadio")
        result = self.radio_page.get_result_text()
        assert result == "Impressive", f"Ожидали результат 'Impressive', получили '{result}'"
        print(f"Результат '{result}' корректен.")
        self.radio_page.take_screenshot("demoqa_radio_impressive.png")

    def test_no_radio_is_disabled(self):
        """Тест проверки, что радио-кнопка 'No' неактивна."""
        print("\n--- Тест: DemoQA - Проверка неактивности 'No' ---")
        is_enabled = self.radio_page.is_no_radio_enabled()
        assert not is_enabled, "Радио-кнопка 'No' должна быть неактивна, но она активна."
        print("Радио-кнопка 'No' корректно неактивна.")
        self.radio_page.take_screenshot("demoqa_radio_no_disabled.png")

    print("Тесты для DemoQA Radio Buttons пройдены ✅")