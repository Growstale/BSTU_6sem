import mglearn
import matplotlib.pyplot as plt
import numpy as np
from sklearn.ensemble import GradientBoostingClassifier
from sklearn.datasets import make_blobs, make_circles
from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LogisticRegression

# Часть 1: Неопределенность в бинарной классификации
# создали набор данных в виде двух концентрических кругов, которые не являются линейно разделимыми
print("--- Неопределенность в бинарной классификации ---")
X, y = make_circles(noise=0.25, factor=0.5, random_state=1)
y_named = np.array(["blue", "red"])[y]
# Разделение данных на обучающую и тестовую выборки
X_train, X_test, y_train_named, y_test_named, y_train, y_test = train_test_split(
    X, y_named, y, random_state=0
    )

# Обучение модели градиентного бустинга
gbrt = GradientBoostingClassifier(random_state=0)
gbrt.fit(X_train, y_train_named)

print("\n--- Метод decision_function ---")
print("Форма массива X_test: {}".format(X_test.shape))
print("Форма решающей функции: {}".format(gbrt.decision_function(X_test).shape))
print("Решающая функция:\n{}".format(gbrt.decision_function(X_test)[:6]))
print("Решающая функция с порогом отсечения:\n{}".format(
    gbrt.decision_function(X_test) > 0)
    )
print("Прогнозы:\n{}".format(gbrt.predict(X_test)))

greater_zero = (gbrt.decision_function(X_test) > 0).astype(int)
pred_from_df = gbrt.classes_[greater_zero]
print("pred идентичен прогнозам: {}".format(np.all(pred_from_df == gbrt.predict(X_test))))

decision_function_vals = gbrt.decision_function(X_test)
print("Решающая функция минимум: {:.2f} максимум: {:.2f}".format(
    np.min(decision_function_vals), np.max(decision_function_vals))
      )

print("\n--- Визуализация decision_function (Рис. 1) ---")
fig, axes = plt.subplots(1, 2, figsize=(13, 5))

# Левый график: граница решения
mglearn.tools.plot_2d_separator(gbrt, X, ax=axes[0], alpha=.4,
                                fill=True, cm=mglearn.cm2)
# Правый график: теплокарта decision_function
scores_image = mglearn.tools.plot_2d_scores(gbrt, X, ax=axes[1], alpha=.4,
                                             cm=mglearn.ReBl,
                                             function='decision_function')

for ax in axes:
    mglearn.discrete_scatter(X_test[:, 0], X_test[:, 1], y_test, markers='^', ax=ax)
    mglearn.discrete_scatter(X_train[:, 0], X_train[:, 1], y_train, markers='o', ax=ax)
    ax.set_xlabel("Характеристика 0")
    ax.set_ylabel("Характеристика 1")

cbar = plt.colorbar(scores_image, ax=axes.tolist())
cbar.set_label("Decision Function Score")
axes[0].legend(["Тест класс 0", "Тест класс 1", "Обучение класс 0", "Обучение класс 1"],
               ncol=4, loc=(.1, 1.1))
axes[0].set_title("Граница принятия решений")
axes[1].set_title("Решающая функция")
plt.suptitle("Рис. 1: Граница и Решающая функция GBDT")
plt.show()

print("\n--- Метод predict_proba ---")

# Вывод формы массива вероятностей
print("Форма вероятностей: {}".format(gbrt.predict_proba(X_test).shape))

print("Спрогнозированные вероятности:\n{}".format(gbrt.predict_proba(X_test)[:6]))

print("\n--- Визуализация predict_proba (Рис. 2) ---")
fig, axes = plt.subplots(1, 2, figsize=(13, 5))

# Левый график: граница решения
mglearn.tools.plot_2d_separator(gbrt, X, ax=axes[0], alpha=.4,
                                fill=True, cm=mglearn.cm2)
# Правый график: теплокарта вероятности КЛАССА 1 ('red')
scores_image_proba = mglearn.tools.plot_2d_scores(gbrt, X, ax=axes[1], alpha=.5,
                                                   cm=mglearn.ReBl, function='predict_proba')

for ax in axes:
    mglearn.discrete_scatter(X_test[:, 0], X_test[:, 1], y_test, markers='^', ax=ax)
    mglearn.discrete_scatter(X_train[:, 0], X_train[:, 1], y_train, markers='o', ax=ax)
    ax.set_xlabel("Характеристика 0")
    ax.set_ylabel("Характеристика 1")

cbar_proba = plt.colorbar(scores_image_proba, ax=axes.tolist())
cbar_proba.set_label("Вероятность класса 'red'")
axes[0].legend(["Тест класс 0", "Тест класс 1", "Обуч класс 0", "Обуч класс 1"], # 'Обуч' как в лабе
               ncol=4, loc=(.1, 1.1))
axes[0].set_title("Граница принятия решений")
axes[1].set_title("Спрогнозированные вероятности (класс 'red')")
plt.suptitle("Рис. 2: Граница и Вероятности GBDT")
plt.show()

# Часть 2: Неопределенность в мультиклассовой классификации
print("\n--- Неопределенность в мультиклассовой классификации (Ирисы) ---")

iris = load_iris()
X_train_i, X_test_i, y_train_i, y_test_i = train_test_split(
    iris.data, iris.target, random_state=42
    )

# Обучение GBDT
gbrt_iris = GradientBoostingClassifier(learning_rate=0.01, random_state=0)
gbrt_iris.fit(X_train_i, y_train_i)

print("\n--- Мультикласс: decision_function ---")
decision_func_iris = gbrt_iris.decision_function(X_test_i)
print("Форма решающей функции: {}".format(decision_func_iris.shape))
# Вывод значений для первых 6 примеров, по одному столбцу на класс
print("Решающая функция:\n{}".format(decision_func_iris[:6, :]))

print("Argmax решающей функции:\n{}".format(
    np.argmax(decision_func_iris, axis=1))
      )
print("Прогнозы:\n{}".format(gbrt_iris.predict(X_test_i)))

print("\n--- Мультикласс: predict_proba ---")
predict_proba_iris = gbrt_iris.predict_proba(X_test_i)
print("Форма вероятностей: {}".format(predict_proba_iris.shape))
print("Спрогнозированные вероятности:\n{}".format(predict_proba_iris[:6]))
print("Суммы: {}".format(predict_proba_iris[:6].sum(axis=1)))

print("Argmax спрогнозированных вероятностей:\n{}".format(
    np.argmax(predict_proba_iris, axis=1))
      )
print("Прогнозы:\n{}".format(gbrt_iris.predict(X_test_i)))


print("\n--- Использование classes_ для получения имен ---")
logreg = LogisticRegression(max_iter=1000) # Создаем модель Логистической Регрессии
named_target_i = iris.target_names[y_train_i]
# iris.target_names: Это массив с названиями классов ирисов: ['setosa', 'versicolor', 'virginica'].
# y_train_i: Это массив с числовыми метками для обучающих данных, например [0, 1, 2, 0, 1, 1, ...].
logreg.fit(X_train_i, named_target_i)

print("уникальные классы в обучающем наборе: {}".format(logreg.classes_))
print("прогнозы: {}".format(logreg.predict(X_test_i)[:10]))

argmax_dec_func_i = np.argmax(logreg.decision_function(X_test_i), axis=1)
print("argmax решающей функции: {}".format(argmax_dec_func_i[:10]))
print("argmax объединенный с классами : {}".format(
    logreg.classes_[argmax_dec_func_i][:10])
      )
