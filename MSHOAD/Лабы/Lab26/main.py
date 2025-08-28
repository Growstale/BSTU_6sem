import matplotlib.pyplot as plt
import numpy as np
import mglearn
from sklearn.datasets import make_blobs, load_breast_cancer
from sklearn.model_selection import train_test_split
from sklearn.svm import LinearSVC, SVC
from sklearn.preprocessing import MinMaxScaler
from mpl_toolkits.mplot3d import Axes3D

# --- Часть 1: Линейный SVM и трюк с ядром (ручной) ---

# 1. Генерация и визуализация исходных данных (нелинейно разделимых)
X, y = make_blobs(centers=4, random_state=8)
y = y % 2  # Преобразуем в 2 класса

plt.figure(figsize=(8, 6))
mglearn.discrete_scatter(X[:, 0], X[:, 1], y)
plt.xlabel("Признак 0")
plt.ylabel("Признак 1")
plt.title("Исходные данные (2 класса)")
plt.show()

# 2. Обучение линейного SVM на исходных данных и визуализация
linear_svm = LinearSVC(random_state=42, dual=True).fit(X, y)

plt.figure(figsize=(8, 6))
mglearn.plots.plot_2d_separator(linear_svm, X)
mglearn.discrete_scatter(X[:, 0], X[:, 1], y)
plt.xlabel("Признак 0")
plt.ylabel("Признак 1")
plt.title("Линейный SVM на исходных данных")
plt.show()

# 3. Добавление нового признака (квадрат второго признака)
X_new = np.hstack([X, X[:, 1:] ** 2])

# 4. Визуализация данных в 3D
figure = plt.figure(figsize=(8, 6))
ax = figure.add_subplot(111, projection='3d')
ax.view_init(elev=-152, azim=-26)

mask = y == 0
ax.scatter(X_new[mask, 0], X_new[mask, 1], X_new[mask, 2], c='blue',
           label='Класс 0', s=60)
ax.scatter(X_new[~mask, 0], X_new[~mask, 1], X_new[~mask, 2], c='red', marker='^',
           label='Класс 1', s=60)
ax.set_xlabel("признак0")
ax.set_ylabel("признак1")
ax.set_zlabel("признак1 ** 2")
ax.set_title("Данные с добавленным признаком в 3D")
ax.legend()
plt.show()

# 5. Обучение линейного SVM на данных с новым признаком (в 3D)
linear_svm_3d = LinearSVC(random_state=42, dual=True).fit(X_new, y)
# Извлекаем коэффициенты (w0, w1, w2) и свободный член (b) найденной разделяющей ПЛОСКОСТИ (ax + by + cz + d = 0)
coef, intercept = linear_svm_3d.coef_.ravel(), linear_svm_3d.intercept_

# 6. Визуализация разделяющей плоскости в 3D
figure = plt.figure(figsize=(8, 6))
ax = figure.add_subplot(111, projection='3d')
ax.view_init(elev=-152, azim=-26)

# Создаем сетку X, Y в диапазоне исходных признаков 0 и 1
xx = np.linspace(X_new[:, 0].min() - 2, X_new[:, 0].max() + 2, 50)
yy = np.linspace(X_new[:, 1].min() - 2, X_new[:, 1].max() + 2, 50)
XX, YY = np.meshgrid(xx, yy) # XX, YY - это 2D-массивы координат X и Y для всех точек сетки

# Уравнение плоскости: coef[0]*x + coef[1]*y + coef[2]*z + intercept = 0
# Выражаем z (координату на третьей оси) через x и y:
ZZ = (coef[0] * XX + coef[1] * YY + intercept) / -coef[2]
# Рисуем поверхность этой плоскости
ax.plot_surface(XX, YY, ZZ, rstride=8, cstride=8, alpha=0.3, color='gray') # Явно задаем цвет плоскости

# Снова рисуем точки данных (как в шаге 4)
ax.scatter(X_new[mask, 0], X_new[mask, 1], X_new[mask, 2], c='blue',
           label='Класс 0', s=60)
ax.scatter(X_new[~mask, 0], X_new[~mask, 1], X_new[~mask, 2], c='red', marker='^',
           label='Класс 1', s=60)

ax.set_xlabel("признак0")
ax.set_ylabel("признак1")
ax.set_zlabel("признак1 ** 2")
ax.set_title("Разделяющая плоскость в 3D")
ax.legend()
plt.show()

# 7. Визуализация нелинейной границы решения в исходном 2D пространстве
plt.figure(figsize=(8, 6))
ZZ_dec = YY**2 # Новый признак это y^2
# Собираем точки сетки в 3D-формат, подходящий для decision_function
# decision_function вычисляет "расстояние со знаком" от каждой точки сетки до разделяющей плоскости в 3D
dec = linear_svm_3d.decision_function(np.c_[XX.ravel(), YY.ravel(), ZZ_dec.ravel()])
plt.contourf(XX, YY, dec.reshape(XX.shape), levels=[dec.min(), 0, dec.max()],
             cmap='coolwarm', alpha=0.5) # Используем стандартный cmap
mglearn.discrete_scatter(X[:, 0], X[:, 1], y)
plt.xlabel("Признак 0")
plt.ylabel("Признак 1")
plt.title("Проекция разделяющей плоскости на 2D")
plt.show()


# --- Часть 2: SVM с RBF-ядром ---

# 8. Демонстрация SVC с RBF-ядром на другом наборе данных
X_hc, y_hc = mglearn.tools.make_handcrafted_dataset()
svm = SVC(kernel='rbf', C=10, gamma=0.1).fit(X_hc, y_hc)

plt.figure(figsize=(8, 6))
mglearn.plots.plot_2d_separator(svm, X_hc, eps=.5)
# рисуем все точки данных
mglearn.discrete_scatter(X_hc[:, 0], X_hc[:, 1], y_hc)

# рисуем ОБВОДКУ для опорных векторов с помощью plt.scatter
sv = svm.support_vectors_
# sv_labels определяет класс опорного вектора (True для класса 1, False для класса 0)
sv_labels = svm.dual_coef_.ravel() > 0
# Определим цвета для обводки в соответствии с классом
colors = [mglearn.cm2.colors[i] for i in sv_labels.astype(int)]
plt.scatter(sv[:, 0], sv[:, 1],
            s=150,  # Размер кружка-обводки (больше чем точки данных)
            facecolors='none', # Без заливки
            edgecolors=colors, # Цвет границы зависит от класса
            marker='o',
            linewidths=3, # Толщина линии обводки (вместо markeredgewidth)
            label='Опорные векторы') # Добавим метку для легенды

plt.xlabel("Признак 0")
plt.ylabel("Признак 1")
plt.title("SVC с RBF-ядром (C=10, gamma=0.1) и опорные векторы")
plt.legend()
plt.show()


# 9. Исследование влияния гиперпараметров C и gamma
fig, axes = plt.subplots(3, 3, figsize=(15, 10))
# Перебираем разные значения C (10^-1, 10^0, 10^3) и gamma (10^-1, 10^0, 10^1)
for ax_row, C_log in zip(axes, [-1, 0, 3]):
    for ax, gamma_log in zip(ax_row, range(-1, 2)):
        C = 10**C_log
        gamma = 10**gamma_log
        mglearn.plots.plot_svm(log_C=C_log, log_gamma=gamma_log, ax=ax)
        ax.set_title(f"C={C:.1f}, gamma={gamma:.1f}")

fig.suptitle("Влияние гиперпараметров C и gamma на границу решения SVC (RBF)", fontsize=16)
axes[0, 0].legend(["class 0", "class 1", "sv class 0", "sv class 1"], ncol=4,
                  loc=(.9, 1.2))
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.show()


# --- Часть 3: Важность масштабирования данных для SVM ---

# 10. Загрузка данных рака груди
cancer = load_breast_cancer()

# 11. Разделение данных
X_train, X_test, y_train, y_test = train_test_split(cancer.data, cancer.target,
                                                    random_state=0)

# 12. Обучение SVC на НЕмасштабированных данных
svc_unscaled = SVC(random_state=42) # Добавил random_state
svc_unscaled.fit(X_train, y_train)

print("--- Результаты на НЕмасштабированных данных ---")
print(f"Правильность на обучающем наборе: {svc_unscaled.score(X_train, y_train):.2f}")
print(f"Правильность на тестовом наборе: {svc_unscaled.score(X_test, y_test):.2f}")

# 13. Визуализация диапазонов признаков до масштабирования
plt.figure(figsize=(10, 6))
plt.plot(X_train.min(axis=0), 'o', label="min")
plt.plot(X_train.max(axis=0), '^', label="max")
plt.legend(loc='best')
plt.xlabel("Индекс признака")
plt.ylabel("Величина признака (лог. шкала)")
plt.yscale("log")
plt.title("Диапазоны значений признаков (до масштабирования)")
plt.show()

# 14. Масштабирование данных с использованием MinMaxScaler (стандартный способ)
scaler = MinMaxScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)

# 15. Визуализация диапазонов признаков после масштабирования
plt.figure(figsize=(10, 6))
plt.plot(X_train_scaled.min(axis=0), 'o', label="min")
plt.plot(X_train_scaled.max(axis=0), '^', label="max")
plt.legend(loc='best')
plt.xlabel("Индекс признака")
plt.ylabel("Величина признака")
plt.title("Диапазоны значений признаков (после масштабирования MinMaxScaler)")
plt.ylim(-0.1, 1.1)
plt.show()


# 16. Обучение SVC на МАСШТАБИРОВАННЫХ данных (с C по умолчанию)
svc_scaled_default = SVC(random_state=42)
svc_scaled_default.fit(X_train_scaled, y_train)

print("\n--- Результаты на МАСШТАБИРОВАННЫХ данных (C=1.0) ---")
print(f"Правильность на обучающем наборе: {svc_scaled_default.score(X_train_scaled, y_train):.3f}")
print(f"Правильность на тестовом наборе: {svc_scaled_default.score(X_test_scaled, y_test):.3f}")

# 17. Обучение SVC на МАСШТАБИРОВАННЫХ данных (с большим C)
svc_scaled_high_c = SVC(C=1000, random_state=42) # Используем новое имя переменной
svc_scaled_high_c.fit(X_train_scaled, y_train)

print("\n--- Результаты на МАСШТАБИРОВАННЫХ данных (C=1000) ---")
print(f"Правильность на обучающем наборе: {svc_scaled_high_c.score(X_train_scaled, y_train):.3f}")
print(f"Правильность на тестовом наборе: {svc_scaled_high_c.score(X_test_scaled, y_test):.3f}")