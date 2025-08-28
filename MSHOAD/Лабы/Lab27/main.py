import graphviz # Используется mglearn для визуализации графов
import matplotlib.pyplot as plt
import mglearn # Библиотека с вспомогательными функциями для книги "Введение в машинное обучение с Python"
import numpy as np
import sklearn # Основная библиотека машинного обучения
from IPython.display import display # Для отображения объектов в средах IPython/Jupyter
from sklearn.datasets import load_breast_cancer, make_moons # Функции для генерации и загрузки датасетов
from sklearn.model_selection import train_test_split # Функция для разделения данных на обучающую и тестовую выборки
from sklearn.neural_network import MLPClassifier # Класс многослойного перцептрона (нейронной сети) для классификации

# Многослойный Персептрон (MLP)

# --- Визуализация архитектуры нейронной сети (с помощью mglearn) ---
print("Визуализация однослойной нейронной сети:")
# Отображение схематического графа нейронной сети с одним скрытым слоем
display(mglearn.plots.plot_single_hidden_layer_graph())
plt.show() # Показать график немедленно

print("\nВизуализация двухслойной нейронной сети:")
# Отображение схематического графа нейронной сети с двумя скрытыми слоями
display(mglearn.plots.plot_two_hidden_layer_graph())
plt.show() # Показать график немедленно


# --- Визуализация функций активации (tanh и ReLU) ---
print("\nГрафики функций активации (tanh и ReLU):")
# Создаем массив значений от -3 до 3
line = np.linspace(-3, 3, 100)
plt.figure() # Создаем новую фигуру для графиков активаций
# Строим график гиперболического тангенса (tanh)
plt.plot(line, np.tanh(line), label="tanh")
# Строим график ReLU (Rectified Linear Unit)
plt.plot(line, np.maximum(line, 0), label="relu")
# Добавляем легенду в наилучшее место на графике
plt.legend(loc="best")
# Подписываем оси
plt.xlabel("x")
plt.ylabel("relu(x), tanh(x)")
plt.title("Функции активации") # Добавляем заголовок
plt.grid(True) # Добавляем сетку для лучшей читаемости
plt.show() # Показать график немедленно


# --- Работа с синтетическим датасетом "Две луны" (make_moons) ---
print("\nРабота с датасетом 'make_moons':")
# Генерируем датасет "две луны": 100 точек, 2 класса, с некоторым шумом
# random_state обеспечивает воспроизводимость генерации
X, y = make_moons(n_samples=100, noise=0.25, random_state=3)

# Разделяем данные на обучающую (75%) и тестовую (25%) выборки
X_train, X_test, y_train, y_test = train_test_split(X, y, stratify=y, random_state=42)

# --- Обучение и визуализация MLP с разными параметрами ---

# 1. MLP с параметрами по умолчанию (1 скрытый слой, 100 нейронов, ReLU, adam solver)
#    Здесь используется 'lbfgs' - оптимизатор, хорошо работающий на малых датасетах.
#    random_state=0 для воспроизводимости инициализации весов.
print("\nMLP с 'lbfgs' и параметрами по умолчанию (скрытый слой [100]):")
mlp_default = MLPClassifier(solver='lbfgs', random_state=0).fit(X_train, y_train)
plt.figure() # Новая фигура
mglearn.plots.plot_2d_separator(mlp_default, X_train, fill=True, alpha=.3)
mglearn.discrete_scatter(X_train[:, 0], X_train[:, 1], y_train)
plt.xlabel("Признак 0")
plt.ylabel("Признак 1")
plt.title("MLP (lbfgs, по умолчанию hidden_layer=[100])")
plt.show()

# 2. MLP с одним скрытым слоем из 10 нейронов
print("\nMLP с 'lbfgs' и одним скрытым слоем [10]:")
mlp_10 = MLPClassifier(solver='lbfgs', random_state=0, hidden_layer_sizes=[10])
mlp_10.fit(X_train, y_train)
plt.figure() # Новая фигура
mglearn.plots.plot_2d_separator(mlp_10, X_train, fill=True, alpha=.3)
mglearn.discrete_scatter(X_train[:, 0], X_train[:, 1], y_train)
plt.xlabel("Признак 0")
plt.ylabel("Признак 1")
plt.title("MLP (lbfgs, hidden_layer=[10])")
plt.show()

# 3. MLP с двумя скрытыми слоями по 10 нейронов
print("\nMLP с 'lbfgs' и двумя скрытыми слоями [10, 10]:")
mlp_10_10 = MLPClassifier(solver='lbfgs', random_state=0, hidden_layer_sizes=[10, 10])
mlp_10_10.fit(X_train, y_train)
plt.figure() # Новая фигура
mglearn.plots.plot_2d_separator(mlp_10_10, X_train, fill=True, alpha=.3)
mglearn.discrete_scatter(X_train[:, 0], X_train[:, 1], y_train)
plt.xlabel("Признак 0")
plt.ylabel("Признак 1")
plt.title("MLP (lbfgs, hidden_layer=[10, 10])")
plt.show()

# 4. MLP с двумя скрытыми слоями [10, 10] и функцией активации tanh
print("\nMLP с 'lbfgs', слоями [10, 10] и активацией 'tanh':")
mlp_tanh = MLPClassifier(solver='lbfgs', activation='tanh', # Явно указываем tanh
                         random_state=0, hidden_layer_sizes=[10, 10])
mlp_tanh.fit(X_train, y_train)
plt.figure() # Новая фигура
mglearn.plots.plot_2d_separator(mlp_tanh, X_train, fill=True, alpha=.3)
mglearn.discrete_scatter(X_train[:, 0], X_train[:, 1], y_train)
plt.xlabel("Признак 0")
plt.ylabel("Признак 1")
plt.title("MLP (lbfgs, tanh, hidden_layer=[10, 10])")
plt.show()


# --- Исследование влияния параметра регуляризации alpha и числа нейронов ---
print("\nИсследование влияния alpha и числа нейронов:")
# Создаем сетку 2x4 для графиков
# figsize задает размер всей фигуры в дюймах
fig, axes = plt.subplots(2, 4, figsize=(20, 8), constrained_layout=True) # constrained_layout помогает избежать наложения подписей

# Перебираем количество нейронов в каждом из двух скрытых слоев (10 и 100)
for axx, n_hidden_nodes in zip(axes, [10, 100]):
    # Перебираем разные значения alpha (коэффициент L2 регуляризации)
    for ax, alpha in zip(axx, [0.0001, 0.01, 0.1, 1]):
        # Создаем и обучаем MLP
        mlp_alpha = MLPClassifier(solver='lbfgs', random_state=0,
                                  hidden_layer_sizes=[n_hidden_nodes, n_hidden_nodes],
                                  alpha=alpha) # alpha - параметр регуляризации
        mlp_alpha.fit(X_train, y_train)
        # Визуализируем разделяющую границу на соответствующем subplot (ax)
        mglearn.plots.plot_2d_separator(mlp_alpha, X_train, fill=True, alpha=.3, ax=ax)
        # Отображаем точки обучающей выборки
        mglearn.discrete_scatter(X_train[:, 0], X_train[:, 1], y_train, ax=ax)
        # Устанавливаем заголовок для каждого subplot
        ax.set_title("n_hidden=[{}, {}]\nalpha={:.4f}".format(
            n_hidden_nodes, n_hidden_nodes, alpha))
        ax.set_xlabel("Признак 0") # Добавим подписи осей и здесь
        ax.set_ylabel("Признак 1")

# Установим общий заголовок для всей фигуры
fig.suptitle("Влияние числа нейронов и alpha на границу решения MLP")
plt.show()


# --- Исследование влияния инициализации весов (random_state) ---
print("\nИсследование влияния инициализации весов (random_state):")
# Модели с одинаковой архитектурой и параметрами, но разной инициализацией весов
# могут сойтись к разным локальным минимумам и дать разные результаты.
fig, axes = plt.subplots(2, 4, figsize=(20, 8), constrained_layout=True)
# axes.ravel() "выпрямляет" 2D массив осей в 1D для удобного перебора
for i, ax in enumerate(axes.ravel()):
    # Используем индекс цикла 'i' в качестве random_state
    mlp_rs = MLPClassifier(solver='lbfgs', random_state=i, hidden_layer_sizes=[100, 100])
    mlp_rs.fit(X_train, y_train)
    # Визуализация
    mglearn.plots.plot_2d_separator(mlp_rs, X_train, fill=True, alpha=.3, ax=ax)
    mglearn.discrete_scatter(X_train[:, 0], X_train[:, 1], y_train, ax=ax)
    ax.set_title(f"random_state={i}")
    ax.set_xlabel("Признак 0")
    ax.set_ylabel("Признак 1")

fig.suptitle("Влияние random_state на границу решения MLP (слои [100, 100])")
plt.show()


# --- Работа с реальным датасетом Breast Cancer ---
print("\nРабота с датасетом 'Breast Cancer':")
cancer = load_breast_cancer()

# Выводим максимальные значения для каждого признака ДО масштабирования
# Это показывает, что признаки имеют сильно разный масштаб
print("Максимальные значения характеристик до масштабирования:\n{}".format(cancer.data.max(axis=0)))

# Разделяем данные на обучающую и тестовую выборки
X_train_bc, X_test_bc, y_train_bc, y_test_bc = train_test_split(
    cancer.data, cancer.target, random_state=0 # Используем другой random_state для этого датасета
)

# --- Обучение MLP на НЕмасштабированных данных ---
print("\nОбучение MLP на НЕмасштабированных данных:")
mlp_unscaled = MLPClassifier(random_state=42, max_iter=1000) # Увеличим max_iter для сходимости
# По умолчанию используется 'adam' solver и активация 'relu'
mlp_unscaled.fit(X_train_bc, y_train_bc)

# Оцениваем качество модели (accuracy - доля правильных ответов)
print("Правильность на обучающем наборе (немасштаб.): {:.3f}".format(mlp_unscaled.score(X_train_bc, y_train_bc)))
print("Правильности на тестовом наборе (немасштаб.): {:.3f}".format(mlp_unscaled.score(X_test_bc, y_test_bc)))
# Ожидаемый результат: точность может быть не очень высокой, т.к. MLP чувствительны к масштабу признаков


# --- Масштабирование данных ---
# Нейронные сети ОЧЕНЬ чувствительны к масштабированию данных.
# Обычно используется StandardScaler (вычитание среднего и деление на станд. отклонение).

# Вычисляем среднее и стандартное отклонение ТОЛЬКО на обучающих данных
mean_on_train = X_train_bc.mean(axis=0)
std_on_train = X_train_bc.std(axis=0)

# Масштабируем обучающую выборку
X_train_scaled = (X_train_bc - mean_on_train) / std_on_train
# Масштабируем тестовую выборку, ИСПОЛЬЗУЯ СРЕДНЕЕ И СТАНД.ОТКЛОНЕНИЕ ОБУЧАЮЩЕЙ ВЫБОРКИ!
X_test_scaled = (X_test_bc - mean_on_train) / std_on_train

# --- Обучение MLP на МАСШТАБИРОВАННЫХ данных ---
print("\nОбучение MLP на МАСШТАБИРОВАННЫХ данных (StandardScaler):")
# Используем те же параметры MLP, но на масштабированных данных
# Заметим, что в sklearn MLPClassifier параметры по умолчанию могут быть неплохими:
# hidden_layer_sizes=(100,), activation='relu', solver='adam', alpha=0.0001, batch_size='auto',
# learning_rate='constant', learning_rate_init=0.001, max_iter=200 ...
mlp_scaled = MLPClassifier(random_state=0, max_iter=1000) # random_state=0 для сравнения с alpha ниже
mlp_scaled.fit(X_train_scaled, y_train_bc)

print("Правильность на обучающем наборе (масштаб.): {:.3f}".format(mlp_scaled.score(X_train_scaled, y_train_bc)))
print("Правильность на тестовом наборе (масштаб.): {:.3f}".format(mlp_scaled.score(X_test_scaled, y_test_bc)))
# Ожидаемый результат: точность ЗНАЧИТЕЛЬНО выше, чем на немасштабированных данных.

# --- Обучение MLP с увеличенным параметром регуляризации alpha ---
# Увеличение alpha усиливает L2-регуляризацию, что может помочь бороться с переобучением,
# делая веса модели меньше.
print("\nОбучение MLP на масштабированных данных с alpha=1:")
mlp_scaled_alpha = MLPClassifier(max_iter=1000, alpha=1, random_state=0)
mlp_scaled_alpha.fit(X_train_scaled, y_train_bc)

print("Правильность на обучающем наборе (масштаб., alpha=1): {:.3f}".format(mlp_scaled_alpha.score(X_train_scaled, y_train_bc)))
print("Правильность на тестовом наборе (масштаб., alpha=1): {:.3f}".format(mlp_scaled_alpha.score(X_test_scaled, y_test_bc)))
# Ожидаемый результат: точность на тесте может немного улучшиться или остаться прежней,
# точность на обучении может немного упасть - это признаки уменьшения переобучения.


# --- Визуализация весов первого слоя обученной модели ---
# Это может дать некоторое представление о том, какие признаки модель считает важными.
print("\nВизуализация весов первого слоя MLP (обученной на масштабир. данных):")
# Используем модель mlp_scaled_alpha (обученную на масштабированных данных с alpha=1)
plt.figure(figsize=(20, 5))
# mlp.coefs_ - это список матриц весов. coefs_[0] - веса между входным и первым скрытым слоем.
# Форма матрицы: [количество_входных_признаков, количество_нейронов_в_скрытом_слое]
# Транспонируем для удобства визуализации: [количество_нейронов, количество_входных_признаков]
plt.imshow(mlp_scaled_alpha.coefs_[0].T, interpolation='none', cmap='viridis') # Используем .T для транспонирования
plt.yticks(range(cancer.data.shape[1]), cancer.feature_names) # Подписываем оси Y именами признаков
plt.xlabel("Нейроны первого скрытого слоя") # Изменил подпись X
plt.ylabel("Входная характеристика")
plt.colorbar() # Добавляем шкалу цветов
plt.title("Веса первого слоя MLP (масштабированные данные, alpha=1)")
plt.show()

print("\n--- Конец скрипта ---")