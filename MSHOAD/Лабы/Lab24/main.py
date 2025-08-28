import mglearn                     # Вспомогательная библиотека для визуализации и примеров из книги
import sklearn                     # Основная библиотека машинного обучения (используется неявно через подмодули)
import matplotlib.pyplot as plt    # Стандартная библиотека для построения графиков
import numpy as np                 # Фундаментальный пакет для численных вычислений
import pandas as pd                # Библиотека для обработки и анализа данных (используется для цен на RAM)
import graphviz                    # Библиотека для рендеринга описаний графов на языке DOT
import pydotplus                   # Интерфейс к языку Dot из Graphviz, используется для экспорта в PDF/PNG

# Импорт конкретных модулей из sklearn
from sklearn.model_selection import train_test_split # Функция для разделения данных
from sklearn.datasets import load_breast_cancer      # Функция для загрузки датасета рака груди
from sklearn.tree import DecisionTreeClassifier      # Модель классификатора "Дерево решений"
from sklearn.tree import DecisionTreeRegressor       # Модель регрессора "Дерево решений"
from sklearn.tree import export_graphviz             # Функция для экспорта дерева в формат DOT
from sklearn.linear_model import LinearRegression     # Модель Линейной регрессии (для сравнения)

from IPython.display import Image, display

# --- Секция 1: Начальные примеры и визуализации ---

# Пример простых данных (пример для Наивного Байеса)
X_dummy = np.array([[0, 1, 0, 1], [1, 0, 1, 1], [0, 0, 0, 1], [1, 0, 1, 0]])
y_dummy = np.array([0, 1, 0, 1])

counts = {}
for label in np.unique(y_dummy):
    counts[label] = X_dummy[y_dummy == label].sum(axis=0)
print("Частоты признаков (dummy data):\n{}".format(counts))
print("-" * 30)

# Построение концептуального дерева решений для классификации животных
print("Строим концептуальное дерево 'Animal Tree'...")
try:
    mglearn.plots.plot_animal_tree()
    plt.show()
except graphviz.backend.execute.ExecutableNotFound as e:
    print(f"Исполняемый файл Graphviz не найден: {e}. Пропускаем построение 'animal tree'.")
    print("Пожалуйста, установите Graphviz и добавьте его в системную переменную PATH.")
print("-" * 30)

# Построение графика, показывающего пошаговое построение дерева решений на датасете 'two_moons'
# На каждом шаге находится "лучший" вопрос (тест), который разделяет данные
# в текущем узле так, чтобы дочерние узлы стали как можно чище

print("Строим график прогрессивного построения дерева...")
mglearn.plots.plot_tree_progressive()
plt.show()
print("-" * 30)


# --- Секция 2: Классификатор Дерево Решений на данных Breast Cancer ---

print("Пример классификатора Дерево Решений (Breast Cancer)")
# Загрузка датасета
cancer = load_breast_cancer()

X_train, X_test, y_train, y_test = train_test_split(
    cancer.data, cancer.target, stratify=cancer.target, random_state=42
)

# --- Обучение и оценка Дерева Решений ---
# Это дерево, скорее всего, переобучится на обучающих данных
print("Обучаем полное Дерево Решений...")
tree_full = DecisionTreeClassifier(random_state=0)
tree_full.fit(X_train, y_train)

acc_train_full = tree_full.score(X_train, y_train)
acc_test_full = tree_full.score(X_test, y_test)
print(f"Точность на обучающей выборке (Полное дерево): {acc_train_full:.3f}")
print(f"Точность на тестовой выборке (Полное дерево): {acc_test_full:.3f}")
print("-" * 15)

# --- Обучение и оценка ОБРЕЗАННОГО Дерева Решений (max_depth=4) ---
# Предварительная обрезка (pre-pruning) путем ограничения глубины помогает бороться с переобучением
print("Обучаем обрезанное Дерево Решений (max_depth=4)...")
tree_pruned = DecisionTreeClassifier(max_depth=4, random_state=0)
tree_pruned.fit(X_train, y_train)

acc_train_pruned = tree_pruned.score(X_train, y_train)
acc_test_pruned = tree_pruned.score(X_test, y_test)
print(f"Точность на обучающей выборке (Обрезанное дерево): {acc_train_pruned:.3f}")
print(f"Точность на тестовой выборке (Обрезанное дерево): {acc_test_pruned:.3f}")
print("-" * 30)


# --- Секция 3: Визуализация обрезанного Дерева Решений ---

dot_filename = "tree_pruned.dot"

print(f"Экспортируем структуру обрезанного дерева в {dot_filename}...")
export_graphviz(
    tree_pruned,                               # Обученная модель дерева
    out_file=dot_filename,                     # Имя выходного файла
    class_names=["malignant", "benign"],       # Имена для классов (0 и 1)
    feature_names=cancer.feature_names,        # Имена признаков
    impurity=False,                            # Не показывать меру неопределенности (impurity)
    filled=True                                # Закрашивать узлы цветами в зависимости от класса
)

# --- Визуализация с использованием библиотеки graphviz'
print("Визуализируем дерево из .dot файла с помощью 'graphviz'...")
try:
    # Читаем содержимое .dot файла
    with open(dot_filename) as f:
        dot_graph_data = f.read()
    display(graphviz.Source(dot_graph_data))
except graphviz.backend.execute.ExecutableNotFound as e:
    print(f"Исполняемый файл Graphviz не найден: {e}. Пропускаем отображение graphviz.Source.")
except FileNotFoundError:
    print(f"Ошибка: Не удалось найти файл {dot_filename} для визуализации.")
print("-" * 15)

# Каждый узел показывает:
# - Условие разделения (признак <= порог).
# - samples: Количество обучающих точек, попавших в этот узел.
# - value: Распределение точек по классам [класс 0, класс 1] в этом узле.
# - class: Имя класса, преобладающего в этом узле.


# --- Визуализация с использованием pydotplus для создания PDF и PNG ---
# Используем уже обученную модель tree_pruned
clf = tree_pruned

print("Экспортируем обрезанное дерево в память для pydotplus...")
dot_data_pdf = export_graphviz(clf, out_file=None,
                               feature_names=cancer.feature_names,
                               class_names=["malignant", "benign"],
                               filled=True)

pdf_filename = "cancer_tree_pruned.pdf"
print(f"Создаем PDF визуализацию ({pdf_filename}) с помощью pydotplus...")
try:
    graph_pdf = pydotplus.graph_from_dot_data(dot_data_pdf)
    graph_pdf.write_pdf(pdf_filename)
    print(f"Успешно создан файл {pdf_filename}")
except ImportError:
    print("Библиотека pydotplus не установлена. Пропускаем создание PDF.")
except FileNotFoundError:
    print("pydotplus не смог найти исполняемый файл 'dot'. Пропускаем создание PDF.")
    print("Убедитесь, что Graphviz установлен и добавлен в PATH.")
print("-" * 15)


print("Создаем PNG визуализацию для отображения в IPython с помощью pydotplus...")
try:
    dot_data_png = export_graphviz(clf, out_file=None, # В память
                                   feature_names=cancer.feature_names,
                                   class_names=cancer.target_names,
                                   filled=True, rounded=True,
                                   special_characters=True)
    graph_png = pydotplus.graph_from_dot_data(dot_data_png)
    # Отображаем изображение напрямую в IPython
    display(Image(graph_png.create_png()))
except ImportError:
    print("Библиотеки pydotplus или IPython не установлены/недоступны. Пропускаем отображение PNG.")
except FileNotFoundError:
    print("pydotplus не смог найти исполняемый файл 'dot'. Пропускаем отображение PNG.")
    print("Убедитесь, что Graphviz установлен и добавлен в PATH.")
print("-" * 30)


# --- Секция 4: Важность Признаков ---

print("Вычисление и построение графика важности признаков")
tree_importance = DecisionTreeClassifier(random_state=0).fit(X_train, y_train)

# Получаем важности признаков (сумма равна 1)
importances = tree_importance.feature_importances_
print("Важности признаков (Полное дерево):\n{}".format(importances))
print("-" * 15)


def plot_feature_importances_cancer(model):
    n_features = cancer.data.shape[1] # Количество признаков
    plt.figure(figsize=(10, 8))
    plt.barh(np.arange(n_features), model.feature_importances_, align='center')
    plt.yticks(np.arange(n_features), cancer.feature_names)
    plt.xlabel("Важность признака")
    plt.ylabel("Признак")
    plt.ylim(-1, n_features)
    plt.title("Важность признаков из Дерева Решений")
    plt.tight_layout()
    plt.show()


# Строим график важности признаков
plot_feature_importances_cancer(tree_importance)
print("-" * 30)


# --- Секция 5: Пример немонотонной зависимости ---

# Функция mglearn.plots.plot_tree_not_monotone() генерирует специальный 2D датасет,
# где зависимость между классом и одним из признаков (X[1]) сложная:
# низкие и высокие значения X[1] соответствуют одному классу, а средние - другому.

print("Визуализация дерева для немонотонной зависимости признака")
tree_non_mono = mglearn.plots.plot_tree_not_monotone()
plt.show()

# Демонстрация способности деревьев улавливать немонотонные связи путем многократных
# разбиений по одному и тому же признаку с разными порогами.
print("Структура дерева для немонотонных данных:")
display(tree_non_mono)
print("-" * 30)


# --- Секция 6: Пример Регрессора Дерево Решений (Цены на RAM) ---

print("Пример Регрессора Дерево Решений (Цены на RAM)")
ram_price_csv_path = "ram_price.csv"

try:
    ram_prices = pd.read_csv(ram_price_csv_path)

    # Построение графика исторических цен с логарифмической шкалой по оси Y
    plt.figure(figsize=(10, 6))
    # plt.semilogy используется для логарифмической шкалы по оси Y.
    plt.semilogy(ram_prices.date, ram_prices.price)
    plt.xlabel("Год")
    plt.ylabel("Цена $/Мбайт (Логарифмическая шкала)")
    plt.title("Исторические цены на RAM")
    plt.grid(True, which="both", linestyle='--', linewidth=0.5)
    plt.show()

    # Подготовка данных для моделирования: прогнозируем цены после 2000 года
    data_train = ram_prices[ram_prices.date < 2000]
    data_test = ram_prices[ram_prices.date >= 2000]

    y_train_log = np.log(data_train.price)
    # Модели sklearn ожидают X в виде 2D массива (даже если признак один).
    # .values превращает столбец pandas в numpy массив.
    # .reshape(-1, 1) преобразует 1D массив в 2D массив с одним столбцом.
    X_train_date = data_train.date.values.reshape(-1, 1)

    print("Размер обучающих данных (X):", X_train_date.shape)
    print("Размер обучающих данных (y_log):", y_train_log.shape)

    # Обучение Регрессора Дерево Решений
    print("Обучаем Регрессор Дерево Решений...")
    tree_reg = DecisionTreeRegressor().fit(X_train_date, y_train_log)

    # Обучение модели Линейной Регрессии для сравнения
    print("Обучаем Линейную Регрессию...")
    linear_reg = LinearRegression().fit(X_train_date, y_train_log)

    # Подготовка всего диапазона дат для предсказания
    X_all_date = ram_prices.date.values.reshape(-1, 1)

    # Получение прогнозов от обеих моделей (прогнозы в логарифмической шкале)
    pred_tree_log = tree_reg.predict(X_all_date)
    pred_lr_log = linear_reg.predict(X_all_date)

    # Преобразование прогнозов обратно в исходную шкалу цен с помощью экспоненты (np.exp)
    price_tree = np.exp(pred_tree_log)
    price_lr = np.exp(pred_lr_log)

    # Построение графика результатов: обучающие, тестовые данные и прогнозы моделей
    print("Строим график обучающих, тестовых данных и прогнозов моделей...")
    plt.figure(figsize=(10, 6))
    # Отображаем точки обучающих и тестовых данных разными маркерами
    plt.semilogy(data_train.date, data_train.price, '^', label="Обучающие данные", markersize=5, alpha=0.7)
    plt.semilogy(data_test.date, data_test.price, 'v', label="Тестовые данные", markersize=5, alpha=0.7)
    # Отображаем прогнозы дерева (не может экстраполировать)
    plt.semilogy(ram_prices.date, price_tree, label="Прогноз дерева", linestyle='--')
    # Отображаем прогнозы линейной регрессии (экстраполирует тренд)
    plt.semilogy(ram_prices.date, price_lr, label="Прогноз лин. регрессии", linestyle=':')
    plt.xlabel("Год")
    plt.ylabel("Цена $/Мбайт (Логарифмическая шкала)")
    plt.title("Сравнение прогнозов цен на RAM")
    plt.legend() # Показываем легенду
    plt.grid(True, which="both", linestyle='--', linewidth=0.5) # Добавляем сетку
    plt.show()

except FileNotFoundError:
    # Обработка ошибки, если CSV файл не найден
    print(f"Ошибка: Не удалось найти файл с ценами на RAM по пути '{ram_price_csv_path}'.")
    print("Пропускаем секцию Регрессора Дерево Решений.")
except Exception as e:
    # Обработка других возможных ошибок в этой секции
    print(f"Произошла ошибка в секции Регрессора: {e}")
    print("Пропускаем оставшуюся часть секции Регрессора.")

print("-" * 30)
print("Скрипт завершен.")

# они не умеют экстраполировать, т.е. предсказывать значения за пределами диапазона целевой переменной,
# который они видели при обучении. Они могут только интерполировать или предсказывать среднее/значение
# в листе, соответствующем входным признакам. Линейные модели, напротив, могут экстраполировать тренд