import sklearn                     # Основная библиотека машинного обучения (используется неявно)
import mglearn                     # Вспомогательная библиотека для визуализации и примеров
import matplotlib.pyplot as plt    # Стандартная библиотека для построения графиков
import numpy as np                 # Библиотека для численных вычислений

from sklearn.ensemble import RandomForestClassifier      # Классификатор Случайный Лес
from sklearn.ensemble import GradientBoostingClassifier  # Классификатор Градиентный Бустинг
from sklearn.datasets import make_moons, load_breast_cancer # Функции для генерации и загрузки датасетов
from sklearn.model_selection import train_test_split     # Функция для разделения данных

print("--- Пример Случайного Леса на 'two_moons' ---")

# Генерируем синтетический датасет 'two_moons'
# n_samples: количество точек
# noise: уровень шума (разброс точек)
X_moons, y_moons = make_moons(n_samples=100, noise=0.25, random_state=3)

X_train_m, X_test_m, y_train_m, y_test_m = train_test_split(
    X_moons, y_moons, stratify=y_moons, random_state=42
)

# Создаем модель Случайного Леса
# n_estimators=5: количество деревьев в лесу
# random_state=2: для воспроизводимости построения леса
forest_moons = RandomForestClassifier(n_estimators=5, random_state=2)

forest_moons.fit(X_train_m, y_train_m)

# Создаем сетку для графиков (2 строки, 3 столбца)
fig, axes = plt.subplots(2, 3, figsize=(20, 10))

# Проходим по каждому дереву в лесу
for i, (ax, tree) in enumerate(zip(axes.ravel(), forest_moons.estimators_)):
    ax.set_title("Дерево {}".format(i))
    # Строим границу решений для ОДНОГО дерева
    mglearn.plots.plot_tree_partition(X_train_m, y_train_m, tree, ax=ax)

# Строим границу решений для ВСЕГО леса
mglearn.plots.plot_2d_separator(forest_moons, X_train_m, fill=True, ax=axes[-1, -1], alpha=.4)
axes[-1, -1].set_title("Случайный лес")

# Отображаем обучающие точки поверх графика итогового леса
mglearn.discrete_scatter(X_train_m[:, 0], X_train_m[:, 1], y_train_m)
plt.show()
print("-" * 30)


# --- Секция 2: Случайный Лес на датасете 'Breast Cancer' ---
print("--- Пример Случайного Леса на 'Breast Cancer' ---")

cancer = load_breast_cancer()

X_train_bc, X_test_bc, y_train_bc, y_test_bc = train_test_split(
    cancer.data, cancer.target, random_state=0
)

# Создаем модель Случайного Леса со 100 деревьями
forest_bc = RandomForestClassifier(n_estimators=100, random_state=0)

forest_bc.fit(X_train_bc, y_train_bc)

acc_train_rf = forest_bc.score(X_train_bc, y_train_bc)
acc_test_rf = forest_bc.score(X_test_bc, y_test_bc)
print(f"Точность на обучающей выборке (RF): {acc_train_rf:.3f}")
print(f"Точность на тестовой выборке (RF): {acc_test_rf:.3f}")
print("-" * 15)


def plot_feature_importances_cancer(model):
    """Строит график важности признаков для датасета рака груди"""
    n_features = cancer.data.shape[1] # Получаем количество признаков
    plt.figure(figsize=(10, 8)) # Устанавливаем размер графика
    plt.barh(range(n_features), model.feature_importances_, align='center')
    plt.yticks(np.arange(n_features), cancer.feature_names)
    plt.xlabel("Важность признака")
    plt.ylabel("Признак")
    plt.ylim(-1, n_features)
    plt.title(f"Важность признаков ({type(model).__name__})")
    plt.tight_layout()
    plt.show()


print("Важность признаков (Random Forest):")
plot_feature_importances_cancer(forest_bc)
print("-" * 30)


# --- Секция 3: Градиентный Бустинг на датасете 'Breast Cancer' ---
print("--- Пример Градиентного Бустинга на 'Breast Cancer' ---")

X_train_gb, X_test_gb, y_train_gb, y_test_gb = train_test_split(
    cancer.data, cancer.target, random_state=0
)

# По умолчанию: n_estimators=100, learning_rate=0.1, max_depth=3
print("Обучаем Gradient Boosting (параметры по умолчанию)...")
gbrt_default = GradientBoostingClassifier(random_state=0)
gbrt_default.fit(X_train_gb, y_train_gb)

acc_train_gb_def = gbrt_default.score(X_train_gb, y_train_gb)
acc_test_gb_def = gbrt_default.score(X_test_gb, y_test_gb)
print(f"Точность на обучающей выборке (GB Default): {acc_train_gb_def:.3f}")
print(f"Точность на тестовой выборке (GB Default): {acc_test_gb_def:.3f}")
print("-" * 15)

# --- Обучение и оценка Градиентного Бустинга с ОГРАНИЧЕННОЙ ГЛУБИНОЙ ---
print("Обучаем Gradient Boosting (max_depth=1)...")
gbrt_depth1 = GradientBoostingClassifier(random_state=0, max_depth=1)
gbrt_depth1.fit(X_train_gb, y_train_gb)

acc_train_gb_d1 = gbrt_depth1.score(X_train_gb, y_train_gb)
acc_test_gb_d1 = gbrt_depth1.score(X_test_gb, y_test_gb)
print(f"Точность на обучающей выборке (GB max_depth=1): {acc_train_gb_d1:.3f}")
print(f"Точность на тестовой выборке (GB max_depth=1): {acc_test_gb_d1:.3f}")
print("-" * 15)

# --- Обучение и оценка Градиентного Бустинга с УМЕНЬШЕННОЙ СКОРОСТЬЮ ОБУЧЕНИЯ ---
# Уменьшаем learning_rate, что также помогает бороться с переобучением
print("Обучаем Gradient Boosting (learning_rate=0.01)...")
gbrt_lr001 = GradientBoostingClassifier(random_state=0, learning_rate=0.01)
gbrt_lr001.fit(X_train_gb, y_train_gb)

acc_train_gb_lr = gbrt_lr001.score(X_train_gb, y_train_gb)
acc_test_gb_lr = gbrt_lr001.score(X_test_gb, y_test_gb)
print(f"Точность на обучающей выборке (GB learning_rate=0.01): {acc_train_gb_lr:.3f}")
print(f"Точность на тестовой выборке (GB learning_rate=0.01): {acc_test_gb_lr:.3f}")
print("-" * 15)

# --- Визуализация важности признаков для модели Градиентного Бустинга (с max_depth=1) ---
print("Важность признаков (Gradient Boosting, max_depth=1):")
plot_feature_importances_cancer(gbrt_depth1)
print("-" * 30)

print("Скрипт завершен.")