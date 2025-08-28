import mglearn                     # Не используется в этом коде, но оставлен, если был нужен ранее
import sklearn                     # Не используется явно, но подмодули импортируются
import matplotlib.pyplot as plt    # Не используется в этом коде, но оставлен
import numpy as np                 # Для работы с массивами NumPy

from sklearn.naive_bayes import BernoulliNB
from sklearn.naive_bayes import MultinomialNB
from sklearn.naive_bayes import GaussianNB

# === Классификатор BernoulliNB ===
# Подходит для бинарных ("да"/"нет", 0/1) признаков.

print("--- BernoulliNB ---")

X_bern = np.array([[0, 1, 0, 1],
                   [1, 0, 1, 1],
                   [0, 0, 0, 1],
                   [1, 0, 1, 0]])
Y_bern = np.array([0, 1, 0, 1])

# Подсчет частоты встречаемости каждого признака для каждого класса
counts = {}
for label in np.unique(Y_bern):
    counts[label] = X_bern[Y_bern == label].sum(axis=0)
print(f"Частоты признаков (сколько раз признак был равен 1 для каждого класса):\n{counts}")

# Создание и обучение классификатора BernoulliNB
clf_bern = BernoulliNB()
clf_bern.fit(X_bern, Y_bern)

# Предсказание для одного образца (третий образец, индекс 2)
# X_bern[2:3] используется для сохранения двумерности ( [[0, 0, 0, 1]] )
prediction_bern = clf_bern.predict(X_bern[2:3])
print(f"Предсказание для X_bern[2:3] (образец [0, 0, 0, 1]):\n{prediction_bern}")
print("-" * 20)

# === Классификатор MultinomialNB ===
# Подходит для дискретных признаков (например, счетчики слов).

print("--- MultinomialNB ---")
# Генерируем случайные данные: 6 образцов, 100 признаков
# Значения признаков - целые числа от 0 до 4
rng = np.random.RandomState(1) # Генератор случайных чисел
X_multi = rng.randint(5, size=(6, 100))
# Метки классов для 6 образцов
y_multi = np.array([1, 2, 3, 4, 5, 6])

# Создание и обучение классификатора MultinomialNB
clf_multi = MultinomialNB()
clf_multi.fit(X_multi, y_multi)

# Предсказание для одного образца (третий образец, индекс 2)
prediction_multi = clf_multi.predict(X_multi[2:3])
print(f"Предсказание для X_multi[2:3]:\n{prediction_multi}")
print("-" * 20)

# === Классификатор GaussianNB ===
# Подходит для непрерывных признаков, предполагая нормальное (гауссово) распределение.

print("--- GaussianNB ---")
# Пример данных: 6 образцов, 2 непрерывных признака
X_gauss = np.array([[-1, -1], [-2, -1], [-3, -2],
                    [ 1,  1], [ 2,  1], [ 3,  2]])
# Метки классов (два класса: 1 и 2)
Y_gauss = np.array([1, 1, 1, 2, 2, 2])

# Создание и обучение классификатора GaussianNB на всех данных сразу
clf_gauss = GaussianNB()
clf_gauss.fit(X_gauss, Y_gauss)

# Предсказание для нового образца [-0.8, -1]
prediction_gauss = clf_gauss.predict([[-0.8, -1]])
print(f"Предсказание для [[-0.8, -1]] (после fit): {prediction_gauss}")

# Пример с частичным обучением (partial_fit)
clf_pf = GaussianNB()
# При первом вызове partial_fit нужно передать все возможные классы
clf_pf.partial_fit(X_gauss, Y_gauss, np.unique(Y_gauss))

# Предсказание для того же образца после partial_fit (результат должен быть таким же)
prediction_pf = clf_pf.predict([[-0.8, -1]])
print(f"Предсказание для [[-0.8, -1]] (после partial_fit): {prediction_pf}")
print("-" * 20)

# partial_fit полезен, когда датасет очень большой и не помещается в память целиком,
# или когда данные поступают потоком. Он позволяет обновлять модель по частям (батчам).
