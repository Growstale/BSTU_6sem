import os

# --- НАСТРОЙКИ ---

# 1. Укажите путь к папке, где лежат ваши скрипты Unity.
#    ВАЖНО: Используйте прямые слеши '/' даже в Windows для избежания ошибок.
#    Пример: 'C:/MyProjects/UnityGame/Assets/Scripts'
SOURCE_FOLDER = 'D:\\6sem_BSTU\\Saber3\\FarmTrain\\Assets\\Scripts'

# 2. Укажите имя файла, в который будут сохранены все скрипты.
#    Этот файл будет создан в той же папке, где вы запустите скрипт.
OUTPUT_FILE = 'D:\\6sem_BSTU\Saber\\all_unity_scripts.txt'


# --- КОНЕЦ НАСТРОЕК ---
def combine_scripts_in_folder(source_path, output_filename):
    """
    Находит все .cs файлы в указанной папке и ее подпапках,
    и записывает их содержимое в один выходной файл,
    автоматически определяя кодировку (UTF-8 или cp1251).
    """
    print(f"Начинаю поиск скриптов в папке: {source_path}")

    # Проверка, существует ли указанная папка
    if not os.path.isdir(source_path):
        print(f"Ошибка: Папка не найдена по пути '{source_path}'")
        print("Пожалуйста, укажите правильный путь в переменной 'SOURCE_FOLDER'.")
        return

    script_count = 0

    # Создаем директорию для выходного файла, если ее нет
    output_dir = os.path.dirname(output_filename)
    if output_dir and not os.path.exists(output_dir):
        os.makedirs(output_dir)
        print(f"Создана папка для выходного файла: {output_dir}")

    # Открываем файл для записи
    with open(output_filename, 'w', encoding='utf-8') as outfile:
        # Рекурсивно обходим все папки
        for root, dirs, files in os.walk(source_path):
            for filename in files:
                # Проверяем, что файл является C# скриптом
                if filename.endswith('.cs'):
                    script_count += 1
                    full_path = os.path.join(root, filename)

                    print(f"Добавляю файл: {full_path}")

                    # --- Запись в файл с обработкой кодировок ---

                    outfile.write(f"{filename}:\n")

                    content = ""
                    try:
                        # Сначала пытаемся прочитать как UTF-8 (стандарт)
                        with open(full_path, 'r', encoding='utf-8') as infile:
                            content = infile.read()
                    except UnicodeDecodeError:
                        # Если не получилось, пробуем Windows-1251 (cp1251)
                        try:
                            print(f"  -> Файл '{filename}' не в UTF-8. Пробую кодировку cp1251...")
                            with open(full_path, 'r', encoding='cp1251') as infile:
                                content = infile.read()
                        except Exception as e:
                            # Если и это не помогло, записываем ошибку
                            error_message = f"!!! ОШИБКА: Не удалось прочитать файл {full_path} ни в одной из кодировок: {e} !!!"
                            print(error_message)
                            content = f"\n{error_message}\n"
                    except Exception as e:
                        # Обработка других возможных ошибок чтения файла
                        error_message = f"!!! ОШИБКА: Не удалось прочитать файл {full_path}: {e} !!!"
                        print(error_message)
                        content = f"\n{error_message}\n"

                    # Записываем содержимое (или сообщение об ошибке)
                    outfile.write(content)

                    # Добавляем разделитель
                    outfile.write("\n\n" + "=" * 80 + "\n\n")

    if script_count > 0:
        print(f"\nГотово! {script_count} скрипт(ов) было обработано и сохранено в файл '{output_filename}'.")
    else:
        print(f"\nВ папке '{source_path}' не найдено ни одного .cs файла.")


if __name__ == "__main__":
    combine_scripts_in_folder(SOURCE_FOLDER, OUTPUT_FILE)
