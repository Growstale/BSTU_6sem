import matplotlib.pyplot as plt
import mglearn
import numpy as np
import pandas as pd
from pandas.plotting import scatter_matrix
from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
from sklearn.neighbors import KNeighborsClassifier

iris_dataset = load_iris()

X_train, X_test, y_train, y_test = train_test_split(
    iris_dataset["data"], iris_dataset["target"], random_state=0
)

print(f"форма массива X_train: {X_train.shape}")
print(f"форма массива y_train: {y_train.shape}")
print(f"форма массива X_test: {X_test.shape}")
print(f"форма массива y_test: {y_test.shape}")

knn = KNeighborsClassifier(n_neighbors=1)
knn.fit(X_train, y_train)

X_new = np.array([[5, 2.9, 1, 0.2]])
print(f"форма массива X_new: {X_new.shape}")

prediction = knn.predict(X_new)
print(f"Прогноз: {prediction}")
print(f"Спрогнозированная метка: {iris_dataset['target_names'][prediction]}")

y_pred = knn.predict(X_test)
print(f"Прогнозы для тестового набора:\n{y_pred}")
print(f"Правильность на тестовом наборе: {np.mean(y_pred == y_test):.2f}")
print(f"Правильность на тестовом наборе: {knn.score(X_test, y_test):.2f}")