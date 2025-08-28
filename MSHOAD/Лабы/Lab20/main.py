import sklearn
import mglearn
import matplotlib.pyplot as plt
import matplotlib
import numpy as np

'''Простая Линейная Регрессия (на dataset wave)'''
# строит график для данных 'wave', показывает точки данных и линию регрессии OLS (Обычный Метод Наименьших Квадратов)
mglearn.plots.plot_linear_regression_wave()
plt.show()

from sklearn.linear_model import LinearRegression
# Загружаем простой набор данных 'wave'. У него 1 входной признак (X) и 1 выход (y). 60 точек.
X, y = mglearn.datasets.make_wave(n_samples=60)
from sklearn.model_selection import train_test_split
# Разделение на обучающую и тестовую выборки
X_train, X_test, y_train, y_test = train_test_split(X, y, random_state=42)

lr = LinearRegression().fit(X_train, y_train) # Создание и обучение модели OLS

print("lr.coef_: {}".format(lr.coef_)) # Коэффициент w (наклон)
print("lr.intercept_: {}".format(lr.intercept_)) # Свободный член b (сдвиг)
# Оценка R^2 (коэффициент детерминации) - насколько хорошо модель объясняет данные (ближе к 1 - лучше)
print("Правильность на обучающем наборе: {:.2f}".format(lr.score(X_train, y_train)))
print("Правильность на тестовом наборе: {:.2f}".format(lr.score(X_test, y_test)))

'''Линейная Регрессия на Сложных Данных'''
X, y = mglearn.datasets.load_extended_boston() # Датасет с большим кол-вом признаков (105)
X_train, X_test, y_train, y_test = train_test_split(X, y, random_state=0)
lr = LinearRegression().fit(X_train, y_train)

print("Правильность на обучающем наборе: {:.2f}".format(lr.score(X_train, y_train)))
print("Правильность на тестовом наборе: {:.2f}".format(lr.score(X_test, y_test)))

'''Гребневая Регрессия (Ridge) для борьбы с переобучением'''
from sklearn.linear_model import Ridge
# alpha=1.0 (по умолчанию)
ridge = Ridge().fit(X_train, y_train)
print("Правильность на обучающем наборе: {:.2f}".format(ridge.score(X_train, y_train)))
print("Правильность на тестовом наборе: {:.2f}".format(ridge.score(X_test, y_test)))
# alpha=10 (сильная регуляризация)
ridge10 = Ridge(alpha=10).fit(X_train, y_train)
print("Правильность на обучающем наборе: {:.2f}".format(ridge10.score(X_train, y_train)))
print("Правильность на тестовом наборе: {:.2f}".format(ridge10.score(X_test, y_test)))
# alpha=0.1 (слабая регуляризация)
ridge01 = Ridge(alpha=0.1).fit(X_train, y_train)
print("Правильность на обучающем наборе: {:.2f}".format(ridge01.score(X_train, y_train)))
print("Правильность на тестовом наборе: {:.2f}".format(ridge01.score(X_test, y_test)))

'''Визуализация Коэффициентов OLS и Ridge'''
plt.plot(ridge.coef_, 's', label="Гребневая регрессия alpha=1") # Квадраты для alpha=1
plt.plot(ridge10.coef_, '^', label="Гребневая регрессия alpha=10") # Треугольники вверх для alpha=10
plt.plot(ridge01.coef_, 'v', label="Гребневая регрессия alpha=0.1") # Треугольники вниз для alpha=0.1
plt.plot(lr.coef_, 'o', label="Линейная регрессия") # Круги для OLS (LinearRegression)

plt.xlabel("Индекс коэффициента")
plt.ylabel("Оценка коэффициента")
plt.hlines(0, 0, len(lr.coef_)) # Горизонтальная линия на нуле
plt.ylim(-25, 25) # Ограничение оси Y для наглядности
plt.legend()
plt.show()

'''Кривые Обучения (Сравнение OLS и Ridge)'''
# Используем готовую функцию из mglearn для построения кривых обучения
# Она обучает OLS и Ridge(alpha=1) на разных по размеру частях обучающих данных
# и строит графики качества на обучении и тесте для каждого размера
mglearn.plots.plot_ridge_n_samples()
plt.show()

'''Лассо Регрессия'''
from sklearn.linear_model import Lasso
# alpha=1.0 (по умолчанию)
lasso = Lasso().fit(X_train, y_train)
print("Правильность на обучающем наборе: {:.2f}".format(lasso.score(X_train, y_train)))
print("Правильность на контрольном наборе: {:.2f}".format(lasso.score(X_test, y_test)))
print("Количество использованных признаков: {}".format(np.sum(lasso.coef_ != 0))) # Подсчет ненулевых коэффициентов
# alpha=0.01 (слабее регуляризация)
lasso001 = Lasso(alpha=0.01, max_iter=100000).fit(X_train, y_train) # Увеличили max_iter
print("Правильность на обучающем наборе: {:.2f}".format(lasso001.score(X_train, y_train)))
print("Правильность на тестовом наборе: {:.2f}".format(lasso001.score(X_test, y_test)))
print("Количество использованных признаков: {}".format(np.sum(lasso001.coef_ != 0)))
# alpha=0.0001 (очень слабая регуляризация)
lasso00001 = Lasso(alpha=0.0001, max_iter=100000).fit(X_train, y_train)
print("Правильность на обучающем наборе: {:.2f}".format(lasso00001.score(X_train, y_train)))
print("Правильность на тестовом наборе: {:.2f}".format(lasso00001.score(X_test, y_test)))
print("Количество использованных признаков: {}".format(np.sum(lasso00001.coef_ != 0)))

# 10. Построение графика коэффициентов Lasso и Ridge
plt.plot(lasso.coef_, 's', label="Лассо alpha=1")
plt.plot(lasso001.coef_, '^', label="Лассо alpha=0.01")
plt.plot(lasso00001.coef_, 'v', label="Лассо alpha=0.0001")
plt.plot(ridge01.coef_, 'o', label="Гребневая регрессия alpha=0.1") # Лучшая Ridge для сравнения

plt.legend(ncol=2, loc=(0, 1.05))
plt.ylim(-25, 25)
plt.xlabel("Индекс коэффициента")
plt.ylabel("Оценка коэффициента")
plt.show()