import mglearn
import matplotlib.pyplot as plt
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.datasets import load_breast_cancer
from sklearn.datasets import fetch_lfw_people
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from sklearn.neighbors import KNeighborsClassifier

# Иллюстрация PCA (Рис. 1)
print("--- Иллюстрация PCA ---")
try:
    mglearn.plots.plot_pca_illustration()
    plt.suptitle("Рис. 1: Иллюстрация работы PCA")
    plt.show()
except Exception as e:
    print(f"Ошибка при вызове mglearn.plots.plot_pca_illustration(): {e}")

print("\n--- PCA для визуализации Breast Cancer ---")
cancer = load_breast_cancer()

# Гистограммы исходных признаков (Рис. 2)
print("--- Построение гистограмм (Рис. 2) ---")
fig, axes = plt.subplots(15, 2, figsize=(10, 20))
malignant = cancer.data[cancer.target == 0]
benign = cancer.data[cancer.target == 1]
ax = axes.ravel()

for i in range(30):
    _, bins = np.histogram(cancer.data[:, i], bins=50)
    ax[i].hist(malignant[:, i], bins=bins, color=mglearn.cm3(0), alpha=.5)
    ax[i].hist(benign[:, i], bins=bins, color=mglearn.cm3(2), alpha=.5)
    ax[i].set_title(cancer.feature_names[i])
    ax[i].set_yticks(())

ax[0].set_xlabel("Значение признака")
ax[0].set_ylabel("Частота")
ax[0].legend(["злокачественная", "доброкачественная"], loc="best")
fig.tight_layout()
plt.suptitle("Рис. 2: Гистограммы признаков Breast Cancer по классам", y=1.01)
plt.show()

# Масштабирование данных перед PCA
print("\n--- Масштабирование данных Cancer ---")
scaler = StandardScaler()
scaler.fit(cancer.data)
X_scaled = scaler.transform(cancer.data)

# Применение PCA для снижения размерности до 2 компонент
print("--- Применение PCA (n_components=2) ---")
pca = PCA(n_components=2)
pca.fit(X_scaled)

X_pca = pca.transform(X_scaled)
print("Форма исходного массива: {}".format(str(X_scaled.shape)))
print("Форма массива после сокращения размерности: {}".format(str(X_pca.shape)))

# Визуализация данных в пространстве 2 главных компонент (Рис. 3)
print("--- Визуализация Cancer в пространстве PCA (Рис. 3) ---")
plt.figure(figsize=(8, 8))
mglearn.discrete_scatter(X_pca[:, 0], X_pca[:, 1], cancer.target)
plt.legend(cancer.target_names, loc="best")
plt.gca().set_aspect("equal")
plt.xlabel("Первая главная компонента")
plt.ylabel("Вторая главная компонента")
plt.title("Рис. 3: Данные Cancer в пространстве первых 2 ГК")
plt.show()

# Анализ главных компонент (Рис. 4)
print("\n--- Анализ главных компонент (Рис. 4) ---")
print("форма главных компонент: {}".format(pca.components_.shape))

plt.matshow(pca.components_, cmap='viridis')
plt.yticks([0, 1], ["Первая компонента", "Вторая компонента"])
plt.colorbar()
plt.xticks(range(len(cancer.feature_names)),
           cancer.feature_names, rotation=60, ha='left')
plt.xlabel("Характеристика")
plt.ylabel("Главные компоненты")
plt.title("Рис. 4: Теплокарта первых 2 ГК")
plt.show()

# Метод «Собственных лиц» (Eigenfaces) (Рис. 5-10)
print("\n--- Метод Eigenfaces ---")

# Загрузка и отображение данных LFW (Рис. 5)
print("--- Загрузка данных LFW ---")
try:
    people = fetch_lfw_people(min_faces_per_person=20, resize=0.7)
    image_shape = people.images[0].shape

    print("--- Отображение примеров лиц (Рис. 5) ---")
    fix, axes = plt.subplots(2, 5, figsize=(15, 8),
                             subplot_kw={'xticks': (), 'yticks': ()})
    for target, image, ax in zip(people.target, people.images, axes.ravel()):
        ax.imshow(image, cmap='gray')
        ax.set_title(people.target_names[target])
    plt.suptitle("Рис. 5: Примеры изображений LFW")
    plt.show()

    print("форма массива изображений лиц: {}".format(people.images.shape))
    print("количество классов: {}".format(len(people.target_names)))

    print("\n--- Частота классов в LFW ---")
    counts = np.bincount(people.target)
    for i, (count, name) in enumerate(zip(counts, people.target_names)):
        print("{0:25} {1:3}".format(name, count), end=' ')
        if (i + 1) % 3 == 0: print()

    print("\n--- Уменьшение асимметрии датасета ---")
    mask = np.zeros(people.target.shape, dtype=bool)
    for target in np.unique(people.target):
        mask[np.where(people.target == target)[0][:50]] = True

    X_people = people.data[mask]
    y_people = people.target[mask]

    X_people = X_people / 255.
    print("Форма данных после фильтрации и масштабирования:", X_people.shape)

    print("\n--- k-NN на исходных пикселях ---")
    X_train, X_test, y_train, y_test = train_test_split(
        X_people, y_people, stratify=y_people, random_state=0)
    knn = KNeighborsClassifier(n_neighbors=1)
    knn.fit(X_train, y_train)
    print("Правильность на тестовом наборе для 1-nn: {:.2f}".format(knn.score(X_test, y_test)))

    # Иллюстрация PCA Whitening (Рис. 6)
    print("\n--- Иллюстрация PCA Whitening (Рис. 6) ---")
    try:
        mglearn.plots.plot_pca_whitening()
        plt.suptitle("Рис. 6: Преобразование PCA с выбеливанием")
        plt.show()
    except Exception as e:
        print(f"Ошибка при вызове mglearn.plots.plot_pca_whitening(): {e}")

    print("\n--- PCA с выбеливанием на данных лиц ---")
    pca = PCA(n_components=100, whiten=True, random_state=0).fit(X_train)
    X_train_pca = pca.transform(X_train)
    X_test_pca = pca.transform(X_test)
    print("обучающие данные после PCA: {}".format(X_train_pca.shape))

    print("\n--- k-NN на данных после PCA ---")
    knn_pca = KNeighborsClassifier(n_neighbors=1)
    knn_pca.fit(X_train_pca, y_train)
    print("Правильность на тестовом наборе для 1-nn после PCA: {:.2f}".format(
        knn_pca.score(X_test_pca, y_test)))

    # Визуализация главных компонент (Eigenfaces) (Рис. 7)
    print("\n--- Визуализация Eigenfaces (Рис. 7) ---")
    print("форма pca.components_: {}".format(pca.components_.shape))

    fix, axes = plt.subplots(3, 5, figsize=(15, 12),
                             subplot_kw={'xticks': (), 'yticks': ()})
    for i, (component, ax) in enumerate(zip(pca.components_, axes.ravel())):
        ax.imshow(component.reshape(image_shape), cmap='viridis')
        ax.set_title("{}. component".format((i + 1)))
    plt.suptitle("Рис. 7: Первые 15 главных компонент (Eigenfaces)")
    plt.show()

    # --- Реконструкция лиц (Рис. 9) ---
    print("\n--- Реконструкция лиц (Рис. 9) ---")
    try:
        mglearn.plots.plot_pca_faces(X_train, X_test, image_shape)
        plt.show()
    except Exception as e:
        print(f"Ошибка при вызове mglearn.plots.plot_pca_faces(): {e}")


    # Визуализация данных лиц в пространстве 2 ГК (Рис. 10)
    print("\n--- Диаграмма рассеяния для набора лиц, использующая первые две главные компоненты (Рис. 10) ---")
    mglearn.discrete_scatter(X_train_pca[:, 0], X_train_pca[:, 1], y_train)
    plt.xlabel("Первая главная компонента")
    plt.ylabel("Вторая главная компонента")
    plt.title("Рис. 10: Данные LFW в пространстве первых 2 ГК")
    plt.show()

except Exception as data_error:
    print(f"Ошибка при загрузке или обработке данных LFW: {data_error}")
    print("Пропуск части примеров с лицами.")

print("\n--- Конец кода Лабораторной работы №30 ---")