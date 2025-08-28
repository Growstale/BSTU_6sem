import numpy as np
import matplotlib.pyplot as plt
import sklearn
import mglearn
from sklearn.model_selection import train_test_split
from sklearn.neighbors import KNeighborsClassifier, KNeighborsRegressor
from sklearn.datasets import load_breast_cancer, fetch_california_housing

# для классификации
# синтетический набор, всего два признака
X, y = mglearn.datasets.make_forge()
# диаграмма рассеяния
mglearn.discrete_scatter(X[:, 0], X[:, 1], y)
plt.legend(["Класс 0", "Класс 1"], loc=4)
plt.xlabel("Первый признак")
plt.ylabel("Второй признак")
print("Форма массива X: {}".format(X.shape))
plt.show()

# для регрессии
# синтетический набор, всего один признак
X, y = mglearn.datasets.make_wave(n_samples=40)
plt.plot(X, y, 'o')
plt.ylim(-3, 3)
plt.xlabel("Признак")
plt.ylabel("Целевая переменная")
plt.show()

# для классификации
# реальный набор
cancer = load_breast_cancer()
print("Ключи cancer: \n{}".format(cancer.keys()))
print("Форма массива data для набора cancer: {}".format(cancer.data.shape))
print("Имена признаков:\n{}".format(cancer.feature_names))
print("Количество примеров для каждого класса:\n{}".format(
    {n: v for n, v in zip(cancer.target_names, np.bincount(cancer.target))}))

# для регрессии
# реальный набор
# Boston Housing  -> California Housing Dataset

california = fetch_california_housing()
X, y = california.data, california.target

print("Форма массива data для набора данных california : {}".format(california.data.shape))
print("Названия признаков:", california.feature_names)

# KNN для Классификации
# готовые картинки из mglearn
mglearn.plots.plot_knn_classification(n_neighbors=1)
plt.show()

mglearn.plots.plot_knn_classification(n_neighbors=3)
plt.show()

# Первое применение (на forge)
X, y = mglearn.datasets.make_forge()
X_train, X_test, y_train, y_test = train_test_split(X, y, random_state=0)

clf = KNeighborsClassifier(n_neighbors=3) # создаем модель KNN с k=3
clf.fit(X_train, y_train)
print("Прогнозы на тестовом наборе: {}".format(clf.predict(X_test)))
print("Правильность на тестовом наборе: {:.2f}".format(clf.score(X_test, y_test)))

# Визуализация границ решений (на forge)
fig, axes = plt.subplots(1, 3, figsize=(10, 3)) # строим 3 графика рядом
for n_neighbors, ax in zip([1, 3, 9], axes):
    clf = KNeighborsClassifier(n_neighbors=n_neighbors).fit(X, y)
    #  Рисует фоновую заливку. Цвет фона в каждой точке плоскости соответствует классу, который предсказала бы модель для этой точки
    mglearn.plots.plot_2d_separator(clf, X, fill=True, eps=0.5, ax=ax, alpha=.4)
    # Поверх фона рисуются сами точки данных forge
    mglearn.discrete_scatter(X[:, 0], X[:, 1], y, ax=ax)
    ax.set_title("количество соседей: {}".format(n_neighbors))
    ax.set_xlabel("Признак 0")
    ax.set_ylabel("Признак 1")
axes[0].legend(loc=3)
plt.show()
'''
Видим, что при k=1 граница "изломанная" (подстраивается под каждую точку, возможно, переобучение), а при k=9 - "гладкая" (обобщает лучше, но может стать слишком простой)
'''


# Поиск оптимального k (на cancer)
X_train, X_test, y_train, y_test = train_test_split(
    cancer.data, cancer.target, stratify=cancer.target, random_state=66)

training_accuracy = []
test_accuracy = []
neighbors_settings = range(1, 11)

for n_neighbors in neighbors_settings:
    clf = KNeighborsClassifier(n_neighbors=n_neighbors)
    clf.fit(X_train, y_train)
    # записываем правильность на обучающем наборе
    training_accuracy.append(clf.score(X_train, y_train))
    # записываем правильность на тестовом наборе
    test_accuracy.append(clf.score(X_test, y_test))

plt.plot(neighbors_settings, training_accuracy, label="Обучающая точность")
plt.plot(neighbors_settings, test_accuracy, label="Тестовая точность")
plt.ylabel("Правильность")
plt.xlabel("Количество соседей")
plt.legend()
plt.show()


# KNN для Регрессии

mglearn.plots.plot_knn_regression(n_neighbors=1)
plt.show()

mglearn.plots.plot_knn_regression(n_neighbors=3)
plt.show()


# Визуальная демонстрация (на wave)

X, y = mglearn.datasets.make_wave(n_samples=40)
X_train, X_test, y_train, y_test = train_test_split(X, y, random_state=0)
reg = KNeighborsRegressor(n_neighbors=3)
reg.fit(X_train, y_train)

print("Прогнозы для тестового набора:\n{}".format(reg.predict(X_test)))
print("R^2 на тестовом наборе: {:.2f}".format(reg.score(X_test, y_test)))
'''
Для регрессии score выдает R². R² показывает, насколько хорошо наша модель объясняет изменения в данных (1 - идеально, 0 - не лучше среднего, <0 - ужасно).
'''

# Визуализация линий регрессии (на wave)
fig, axes = plt.subplots(1, 3, figsize=(15, 4)) # места для 3 графиков рядом
# Генерируем равномерно распределённые значения признака от -3 до 3
# reshape(-1, 1) нужен, чтобы данные были в формате (1000 строк, 1 столбец), как ожидает модель
line = np.linspace(-3, 3, 1000).reshape(-1, 1)

for n_neighbors, ax in zip([1, 3, 9], axes):
    reg = KNeighborsRegressor(n_neighbors=n_neighbors)
    reg.fit(X_train, y_train)
    # просим уже обученную модель (reg) сделать предсказания для наших 1000 пробных точек
    ax.plot(line, reg.predict(line), label="Прогнозы модели")
    ax.plot(X_train, y_train, '^', c=mglearn.cm2(0), markersize=8, label="Обучающие данные")
    ax.plot(X_test, y_test, 'v', c=mglearn.cm2(1), markersize=8, label="Тестовые данные")
    ax.set_title(f"{n_neighbors} сосед(ей)\n"
                 f"Обуч. score: {reg.score(X_train, y_train):.2f}, "
                 f"Тест. score: {reg.score(X_test, y_test):.2f}")
    ax.set_xlabel("Признак")
    ax.set_ylabel("Целевая переменная")
axes[0].legend(loc="best")
plt.show()

'''
Видим, как при k=1 линия "скачет" за обучающими точками (переобучение), а при k=9 становится очень гладкой (возможно, недообучение).
'''
