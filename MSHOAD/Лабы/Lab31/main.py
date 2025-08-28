import mglearn
import sklearn
import matplotlib.pyplot as plt
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.datasets import fetch_lfw_people # <--- ДОБАВЛЕНО: для загрузки данных лиц
from sklearn.decomposition import NMF, PCA # Добавил PCA здесь для импорта один раз
from sklearn.datasets import load_digits
from sklearn.manifold import TSNE

# --- Шаг 1: Загрузка и подготовка данных Labeled Faces in the Wild ---
# Этот блок определяет X_train, X_test, image_shape
try:
    # Загружаем данные лиц
    # min_faces_per_person=70 используется для получения достаточного количества изображений
    # resize=0.4 уменьшает размер изображений для ускорения обработки
    lfw_people = fetch_lfw_people(min_faces_per_person=70, resize=0.4)

    # Получаем форму изображений
    image_shape = lfw_people.images[0].shape

    # X содержит данные изображений (каждая строка - одно развернутое изображение)
    # y содержит метки (ID человека)
    X_faces = lfw_people.data
    y_faces = lfw_people.target

    # Разделяем данные на обучающую и тестовую выборки
    # stratify=y_faces гарантирует, что пропорции классов будут одинаковыми в обеих выборках
    X_train, X_test, y_train, y_test = train_test_split(
        X_faces, y_faces, stratify=y_faces, random_state=42
    )
    print(f"Данные LFW загружены: X_train shape: {X_train.shape}, image_shape: {image_shape}")
    LFW_DATA_AVAILABLE = True
except Exception as e:
    print(f"Ошибка при загрузке данных LFW: {e}")
    print("Часть кода, использующая данные лиц, может не работать корректно.")
    # Создаем заглушки, чтобы код не упал сразу, если остальная часть не защищена
    # В реальном проекте здесь была бы более сложная обработка ошибок
    X_train, X_test, image_shape = np.array([]), np.array([]), (0,0)
    y_train, y_test = np.array([]), np.array([])
    LFW_DATA_AVAILABLE = False

# --- Иллюстрация NMF (общая) ---
mglearn.plots.plot_nmf_illustration()
plt.show()

# --- NMF на изображениях лиц ---
if LFW_DATA_AVAILABLE and X_train.size > 0 : # Проверяем, что данные загружены и не пусты
    # Эта строка теперь должна работать, если данные LFW загружены
    mglearn.plots.plot_nmf_faces(X_train, X_test, image_shape)
    plt.suptitle("Рис. 2 Реконструкция лиц (из mglearn.plots.plot_nmf_faces)")
    plt.show()

    nmf_faces = NMF(n_components=15, random_state=0, max_iter=500, tol=1e-3) # Добавил max_iter и tol для сходимости
    nmf_faces.fit(X_train)
    X_train_nmf = nmf_faces.transform(X_train)
    X_test_nmf = nmf_faces.transform(X_test) # X_test_nmf создается, но не используется в этом скрипте далее

    # Отображение компонент NMF
    fig_comp, axes_comp = plt.subplots(3, 5, figsize=(15, 12), subplot_kw={'xticks': (), 'yticks': ()})
    for i, (component, ax_item) in enumerate(zip(nmf_faces.components_, axes_comp.ravel())): # ax -> ax_item
        ax_item.imshow(component.reshape(image_shape))
        ax_item.set_title("{}. component".format(i))
    plt.suptitle("Рис. 3 Компоненты NMF для лиц (15 компонент)")
    plt.show()

    # Лица с наибольшим коэффициентом для компоненты 3
    compn3 = 3
    inds3 = np.argsort(X_train_nmf[:, compn3])[::-1]
    fig_inds3, axes_inds3 = plt.subplots(2, 5, figsize=(15, 8), subplot_kw={'xticks': (), 'yticks': ()})
    for i, (ind, ax_item) in enumerate(zip(inds3[:10], axes_inds3.ravel())): # ax -> ax_item, показываем 10
        ax_item.imshow(X_train[ind].reshape(image_shape))
    plt.suptitle(f"Рис. 4 Лица с наибольшим коэффициентом компоненты {compn3}")
    plt.show()

    # Лица с наибольшим коэффициентом для компоненты 7
    compn7 = 7
    inds7 = np.argsort(X_train_nmf[:, compn7])[::-1]
    fig_inds7, axes_inds7 = plt.subplots(2, 5, figsize=(15, 8), subplot_kw={'xticks': (), 'yticks': ()})
    for i, (ind, ax_item) in enumerate(zip(inds7[:10], axes_inds7.ravel())): # ax -> ax_item, показываем 10
        ax_item.imshow(X_train[ind].reshape(image_shape))
    plt.suptitle(f"Рис. 5 Лица с наибольшим коэффициентом компоненты {compn7}")
    plt.show()
else:
    print("\nПропускаем часть кода с NMF на лицах из-за отсутствия данных LFW или ошибки при их загрузке.")


# --- NMF на синтетических сигналах ---
print("\n--- NMF на синтетических сигналах ---")
S_signals = mglearn.datasets.make_signals()
plt.figure(figsize=(6, 1))
plt.plot(S_signals, '-')
plt.xlabel("Время")
plt.ylabel("Сигнал")
plt.title("Рис. 6 Исходные источники сигнала")
plt.show()

A_synth = np.random.RandomState(0).uniform(size=(100, 3)) # Переименовал A -> A_synth
X_synth = np.dot(S_signals, A_synth.T) # Переименовал X -> X_synth
print("Форма измерений (синтетика): {}".format(X_synth.shape))

nmf_synth = NMF(n_components=3, random_state=42) # nmf -> nmf_synth
S_reconstructed_nmf = nmf_synth.fit_transform(X_synth) # S_ -> S_reconstructed_nmf
print("Форма восстановленного сигнала NMF (синтетика): {}".format(S_reconstructed_nmf.shape))

pca_synth = PCA(n_components=3) # pca -> pca_synth
H_reconstructed_pca = pca_synth.fit_transform(X_synth) # H -> H_reconstructed_pca

models_synth = [X_synth, S_signals, S_reconstructed_nmf, H_reconstructed_pca] # models -> models_synth
names_synth = ['Наблюдения (первые три измерения)', 'Фактические источники',
               'Сигналы, восстановленные NMF', 'Сигналы, восстановленные РСА'] # names -> names_synth
fig_synth, axes_synth_list = plt.subplots(4, 1, figsize=(8, 6), gridspec_kw={'hspace': .5}, # axes -> axes_synth_list, figsize изменен
                                     subplot_kw={'xticks': (), 'yticks': ()})
if not isinstance(axes_synth_list, np.ndarray): axes_synth_list = [axes_synth_list] # Для случая одного subplot

for model_item, name_item, ax_item in zip(models_synth, names_synth, axes_synth_list.ravel()): # model,name,ax -> model_item,name_item,ax_item
    ax_item.set_title(name_item)
    ax_item.plot(model_item[:, :3], '-')
plt.suptitle("Рис. 7 Восстановление источников сигнала (NMF и PCA)")
plt.tight_layout(rect=[0,0,1,0.96]) # Для корректного отображения suptitle
plt.show()


# --- t-SNE на рукописных цифрах ---
print("\n--- t-SNE на рукописных цифрах ---")
digits = load_digits()
fig_digits, axes_digits_list = plt.subplots(2, 5, figsize=(10, 5), subplot_kw={'xticks': (), 'yticks': ()}) # axes -> axes_digits_list
for ax_item, img_item in zip(axes_digits_list.ravel(), digits.images): # ax,img -> ax_item,img_item
    ax_item.imshow(img_item, cmap='gray') # Добавлен cmap='gray'
plt.suptitle("Рис. 8 Примеры изображений digits")
plt.show()

# PCA для визуализации digits
pca_digits_vis = PCA(n_components=2) # pca -> pca_digits_vis
pca_digits_vis.fit(digits.data)
digits_pca_transformed = pca_digits_vis.transform(digits.data) # digits_pca -> digits_pca_transformed
colors_digits = ["#476A2A", "#7851B8", "#BD3430", "#4A2D4E", "#875525", "#A83683", "#4E655E", "#853541", "#3A3120", "#535D8E"] # colors -> colors_digits

plt.figure(figsize=(10, 10))
plt.xlim(digits_pca_transformed[:, 0].min() - 1, digits_pca_transformed[:, 0].max() + 1) # Добавлены отступы
plt.ylim(digits_pca_transformed[:, 1].min() - 1, digits_pca_transformed[:, 1].max() + 1) # Добавлены отступы
for i in range(len(digits.data)):
    plt.text(digits_pca_transformed[i, 0], digits_pca_transformed[i, 1],
             str(digits.target[i]),
             color=colors_digits[digits.target[i]],
             fontdict={'weight': 'bold', 'size': 9})
plt.xlabel("Первая главная компонента (PCA)")
plt.ylabel("Вторая главная компонента (PCA)")
plt.title("Рис. 9 Диаграмма рассеяния digits (PCA)")
plt.show()

# t-SNE для визуализации digits
tsne_digits = TSNE(random_state=42) # tsne -> tsne_digits
digits_tsne_transformed = tsne_digits.fit_transform(digits.data) # digits_tsne -> digits_tsne_transformed

plt.figure(figsize=(10, 10))
plt.xlim(digits_tsne_transformed[:, 0].min() -1 , digits_tsne_transformed[:, 0].max() + 1) # Добавлены отступы
plt.ylim(digits_tsne_transformed[:, 1].min() -1 , digits_tsne_transformed[:, 1].max() + 1) # Добавлены отступы
for i in range(len(digits.data)):
    plt.text(digits_tsne_transformed[i, 0], digits_tsne_transformed[i, 1],
             str(digits.target[i]),
             color=colors_digits[digits.target[i]],
             fontdict={'weight': 'bold', 'size': 9})
plt.xlabel("t-SNE признак 0")
plt.ylabel("t-SNE признак 1") # ИСПРАВЛЕНО: plt.xlabel -> plt.ylabel
plt.title("Рис. 10 Диаграмма рассеяния digits (t-SNE)")
plt.show()