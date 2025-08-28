import mglearn
import matplotlib.pyplot as plt
from sklearn.datasets import load_breast_cancer
from sklearn.datasets import make_blobs
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import MinMaxScaler
from sklearn.preprocessing import StandardScaler
from sklearn.svm import SVC

# Визуализация различных методов масштабирования (Рис. 1)
print("--- Визуализация методов масштабирования ---")
try:
    mglearn.plots.plot_scaling()
    plt.suptitle("Рис. 1: Различные способы масштабирования")
    plt.show()
except Exception as e:
    print(f"Ошибка при вызове mglearn.plots.plot_scaling(): {e}")

# Применение преобразований данных (на примере Breast Cancer)
print("\n--- Применение MinMaxScaler к Breast Cancer ---")
cancer = load_breast_cancer()
X_train, X_test, y_train, y_test = train_test_split(cancer.data, cancer.target,
                                                    random_state=1)
print("Исходные формы:")
print("X_train.shape:", X_train.shape)
print("X_test.shape:", X_test.shape)

scaler_mm = MinMaxScaler() # Создание экземпляра MinMaxScaler
scaler_mm.fit(X_train) # Обучение скейлера ТОЛЬКО на обучающих данных

# Ручное масштабирование
print("\n--- Ручное применение MinMaxScaler (демонстрация) ---")
min_on_training = X_train.min(axis=0)
range_on_training = (X_train.max(axis=0) - min_on_training) # Размах
range_on_training[range_on_training == 0] = 1
# Формула MinMaxScaler: (значение - минимум) / (максимум - минимум)
X_train_scaled_manual = (X_train - min_on_training) / range_on_training

print("форма преобразованного массива (обуч.): {}".format(X_train_scaled_manual.shape))
print("min значение признака после масштабирования (обуч.):\n {}".format(X_train_scaled_manual.min(axis=0)))
print("max значение признака после масштабирования (обуч.):\n {}".format(X_train_scaled_manual.max(axis=0)))

# Для масштабирования тестовых данных (X_test) мы используем min_on_training и range_on_training, которые мы вычислили на обучающих данных

'''
Потому что тестовые данные имитируют новые, не виденные ранее данные, которые придут в будущем. Мы должны обрабатывать их точно так же, как обрабатывали обучающие данные.
Из-за этого минимальные значения в X_test_scaled_manual могут быть меньше 0, а максимальные — больше 1. Это нормально! Это значит, что в тестовом наборе встретились значения, которые были меньше самого маленького или больше самого большого в обучающем наборе.
'''

X_test_scaled_manual = (X_test - min_on_training) / range_on_training
print("\nmin значение признака после масштабирования (тест):\n{}".format(X_test_scaled_manual.min(axis=0)))
print("max значение признака после масштабирования (тест):\n{}".format(X_test_scaled_manual.max(axis=0)))

# Масштабирование обучающего и тестового наборов одинаковым образом (Рис. 2)
print("\n--- Масштабирование train/test одинаковым образом (Рис. 2) ---")
X_blobs, _ = make_blobs(n_samples=50, centers=5, random_state=4, cluster_std=2)
X_train_b, X_test_b = train_test_split(X_blobs, random_state=5, test_size=.1)

fig, axes = plt.subplots(1, 3, figsize=(13, 4))

# График 1: Исходные данные
axes[0].scatter(X_train_b[:, 0], X_train_b[:, 1],
                color=mglearn.cm2(0), label="Обучающий набор", s=60) # ИЗМЕНЕНО c -> color
axes[0].scatter(X_test_b[:, 0], X_test_b[:, 1], marker='^',
                color=mglearn.cm2(1), label="Тестовый набор", s=60) # ИЗМЕНЕНО c -> color
axes[0].legend(loc='upper left')
axes[0].set_title("Исходные данные")

# График 2: Правильное масштабирование
scaler_blobs = MinMaxScaler()
scaler_blobs.fit(X_train_b)
X_train_scaled_b = scaler_blobs.transform(X_train_b)
X_test_scaled_b = scaler_blobs.transform(X_test_b)

axes[1].scatter(X_train_scaled_b[:, 0], X_train_scaled_b[:, 1],
                color=mglearn.cm2(0), label="Обучающий набор", s=60)
axes[1].scatter(X_test_scaled_b[:, 0], X_test_scaled_b[:, 1], marker='^',
                color=mglearn.cm2(1), label="Тестовый набор", s=60)
axes[1].set_title("Масштабированные данные")

# График 3: Неправильное масштабирование
test_scaler_blobs = MinMaxScaler()
test_scaler_blobs.fit(X_test_b)
X_test_scaled_badly = test_scaler_blobs.transform(X_test_b)

axes[2].scatter(X_train_scaled_b[:, 0], X_train_scaled_b[:, 1],
                color=mglearn.cm2(0), label="Обучающий набор", s=60)
axes[2].scatter(X_test_scaled_badly[:, 0], X_test_scaled_badly[:, 1], marker='^',
                color=mglearn.cm2(1), label="Тестовый на бор", s=60)
axes[2].set_title("Неправильно масштабированные данные")

for ax in axes:
    ax.set_xlabel("Признак 0")
    ax.set_ylabel("Признак 1")

fig.tight_layout()
plt.suptitle("Рис. 2: Правильное и неправильное масштабирование", y=1.02)
plt.show()

# Быстрые и эффективные способы (fit_transform)
print("\n--- Использование fit_transform ---")
scaler_st = StandardScaler()

# X_train_scaled = scaler_st.fit_transform(X_train)
# X_test_scaled = scaler_st.transform(X_test)

# Влияние предварительной обработки на обучение с учителем
print("\n--- Влияние масштабирования на SVM ---")
svm_unscaled = SVC(C=100)
svm_unscaled.fit(X_train, y_train)
print("Правильность на тестовом наборе (без масштабирования): {:.2f}".format(
    svm_unscaled.score(X_test, y_test)))

# Масштабируем с помощью MinMaxScaler
scaler_mm_svm = MinMaxScaler()
scaler_mm_svm.fit(X_train)
X_train_scaled_svm = scaler_mm_svm.transform(X_train)
X_test_scaled_svm = scaler_mm_svm.transform(X_test)

# Обучаем SVM на масштабированных данных MinMaxScaler
svm_mm_scaled = SVC(C=100)
svm_mm_scaled.fit(X_train_scaled_svm, y_train)
print("Правильность на масштабированном тестовом наборе (MinMaxScaler): {:.2f}".format(
    svm_mm_scaled.score(X_test_scaled_svm, y_test)))

# Масштабируем с помощью StandardScaler
scaler_st_svm = StandardScaler()
scaler_st_svm.fit(X_train)
X_train_scaled_st = scaler_st_svm.transform(X_train)
X_test_scaled_st = scaler_st_svm.transform(X_test)

# Обучаем SVM на масштабированных данных StandardScaler
svm_st_scaled = SVC(C=100)
svm_st_scaled.fit(X_train_scaled_st, y_train)
print("Правильность SVM на тестовом наборе (StandardScaler): {:.2f}".format( # Формулировка как в лабе
    svm_st_scaled.score(X_test_scaled_st, y_test)))
