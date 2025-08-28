# pip install sympy numpy matplotlib pandas scipy ipython scikit-learn mglearn

# sympy — это библиотека для символьных вычислений. Она позволяет работать с математическими выражениями как с
# символами, а не числами, решать уравнения, интегрировать, дифференцировать и т. д.

import sympy as sp

x = sp.symbols('x')
f = x**2 + 1
derivative_f = sp.diff(f, x)
print("Производная функции f(x):", derivative_f)

integral_f = sp.integrate(f, (x, 0, 1))
print("Интеграл функции f(x) на отрезке [0, 1]:", integral_f)

f = 1/(x**2 + 1)
f_limit = sp.limit(f, x, sp.oo)
print("Предел функции f(x) при x → ∞:", f_limit)

# numpy — основная библиотека для работы с числовыми данными и многомерными массивами (матрицами). Она включает
# в себя функции для математических операций, линейной алгебры, статистики и других научных вычислений.

import numpy as np

array_1d = np.random.randint(0, 100, size=20)
print("Одномерный массив:", array_1d)

array_2d = array_1d.reshape(4, 5)
print("Двумерный массив:\n", array_2d)

array1, array2 = np.split(array_2d, [3], axis=1)
print("\nПервый массив (3 столбца):\n", array1)
print("\nВторой массив (2 столбца):\n", array2)

matches = np.where(array1 == 6)
if matches[0].size == 0:
    print("Число 6 не найдено в массиве")
else:
    print("Индексы элементов, равных 6:", matches)
    count_matches = len(matches[0])
    print("Количество элементов, равных 6:", count_matches)

min_value = np.min(array2)
max_value = np.max(array2)
mean_value = np.mean(array2)

print("Минимальное значение:", min_value)
print("Максимальное значение:", max_value)
print("Среднее значение:", mean_value)

# pandas — библиотека для работы с данными в табличном виде. Она предоставляет инструменты
# для анализа и манипуляции данными, таких как фильтрация, группировка, агрегация и объединение таблиц.

import pandas as pd

series_from_array = pd.Series(np.random.randint(0, 10, 5))
print("Series из массива NumPy:\n", series_from_array)

series_from_dict = pd.Series({'a': 1, 'b': 2, 'c': 3})
print("Series из словаря:\n", series_from_dict)

result = series_from_array + 10
print("Series после добавления 10:\n", result)

df_from_array = pd.DataFrame(np.random.rand(4, 3), columns=['A', 'B', 'C'])
print("DataFrame из массива NumPy:\n", df_from_array)

df_from_dict = pd.DataFrame({'name': ['Alice', 'Bob'], 'age': [25, 30]})
print("DataFrame из словаря:\n", df_from_dict)

df_from_series = pd.DataFrame({'values': series_from_array})
print("DataFrame из Series:\n", df_from_series)

# matplotlib — библиотека для создания статичных, анимированных и интерактивных графиков и визуализаций данных.

import matplotlib.pyplot as plt

x_values = np.linspace(-10, 10, 100)
y_values = x_values**2 + 1
plt.plot(x_values, y_values)
plt.title("График функции f(x) = x^2 + 1")
plt.xlabel("x")
plt.ylabel("f(x)")
plt.grid(True)
plt.show()

x = np.linspace(-5, 5, 100)
y = np.linspace(-5, 5, 100)
# создаёт две 2D-матрицы, которые представляют сетку координат для плоскости
x, y = np.meshgrid(x, y)
z = x**2 + 2*y**2 + 1
fig = plt.figure() # создает контейнер верхнего уровня для всего графика
ax = fig.add_subplot(111, projection='3d') # добавляет систему координат (axes) к нашей фигуре
ax.plot_surface(x, y, z, cmap='viridis') # 3д оси
ax.set_title("График функции f(x, y) = x^2 + 2y^2 + 1")
plt.show()

# Столбчатая диаграмма
plt.bar(['A', 'B', 'C'], [10, 20, 30])
plt.title('Столбчатая диаграмма')
plt.show()

# Круговая диаграмма
labels = ['A', 'B', 'C']
sizes = [10, 20, 30]
plt.pie(sizes, labels=labels, autopct='%1.1f%%')
plt.title('Круговая диаграмма')
plt.show()

# scipy — это набор расширений для numpy, включающий более сложные математические функции, такие как оптимизация,
# статистика, линейная алгебра, численные интеграции и решение дифференциальных уравнений.

# ipython — это улучшенная интерактивная оболочка Python, предоставляющая дополнительные возможности, такие как автодополнение,
# истории команд, магические команды для работы с системой и другие полезные функции.

# scikit-learn — одна из самых популярных библиотек для машинного обучения в Python, предоставляющая инструменты для
# классификации, регрессии, кластеризации, уменьшения размерности и других методов анализа данных.

# mglearn — библиотека для машинного обучения, предоставляющая вспомогательные функции, примеры и визуализации,
# которые полезны при обучении и объяснении алгоритмов машинного обучения.