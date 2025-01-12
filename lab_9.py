from PIL import Image # Подключим необходимые библиотеки
import numpy as np
import matplotlib.pyplot as plt
from scipy.ndimage import binary_erosion, binary_dilation

file_path = r'C:\Users\sidel\practis_alg\polygon-1\кощки.png'  # Путь к изображению
image = Image.open(file_path).convert("RGB")  # Открываем изображение и конвертируем в RGB
data = np.array(image)  # Преобразуем изображение в массив numpy
h, w, _ = data.shape  # Высота, ширина, количество каналов

''' Главная проблема возникла с определением RGB фона,
 поэтому упростила себе задачу и поделила изображение по секторам,
 а после определила RGB фон для каждого сектора'''

# Делим изображение на 4 сектора
sectors = [
    data[:h // 2, :w // 2, :],  # Верхний левый
    data[:h // 2, w // 2:, :],  # Верхний правый
    data[h // 2:, :w // 2, :],  # Нижний левый
    data[h // 2:, w // 2:, :]   # Нижний правый
]

# Функция для удаления фона из сектора с индивидуальной настройкой маски
def remove_background(sector, lower_bound, upper_bound):
    red, green, blue = sector[:, :, 0], sector[:, :, 1], sector[:, :, 2]  # Разделяем каналы

    mask = (red >= lower_bound[0]) & (green >= lower_bound[1]) & (blue <= lower_bound[2])  # Создаем маску

    # Удаление шума
    mask_cleaned = binary_dilation(binary_erosion(mask, structure=np.ones((3, 3))), structure=np.ones((3, 3)))

    sector[mask_cleaned] = [255, 255, 255]  # Заменяем фон на белый цвет
    return sector # Возвращаем обработанный сектор

# Параметры маски для каждого сектора
# [минимальное значение, максимальное значение]
bounds = [
    ([200, 170, 120], [255, 248, 238]),  # Верхний левый сектор 
    ([180, 140, 70], [255, 250, 240]), # Верхний правый сектор
    ([190, 150, 100], [255, 255, 239]),  # Нижний левый сектор
    ([100, 50, 0], [220, 140, 6])    # Нижний правый сектор(я старалась подобрать параметры, однако нижний правый сектор слегка не идеальный)
]

# Удаляем фон из каждого сектора
processed_sectors = [remove_background(sectors[i].copy(), bounds[i][0], bounds[i][1]) for i in range(4)]

# Соединяем все сектора
top_half = np.hstack((processed_sectors[0], processed_sectors[1]))  # Верхние два сектора
bottom_half = np.hstack((processed_sectors[2], processed_sectors[3]))  # Нижние два сектора
final_image = np.vstack((top_half, bottom_half))  # Соединяем верх и низ

# Преобразуем массив в изображение
result_image = Image.fromarray(final_image)

# Отображаем результат
plt.figure(figsize=(8, 8))  # Создаем график
plt.imshow(result_image)  # Отображаем изображение
plt.axis('off')  # Отключаем оси
plt.title('Изображение без фона')  # Заголовок
plt.show()  # Показываем изображение

# Сохраняем результат
result_path = 'C:/Users/sidel/practis_alg/polygon-1/cats_no_background_by_sector.png'  # Путь для сохранения(можно изменить, я сохраняю в ту же папку с изображением)
result_image.save(result_path)  # Сохраняем изображение
print(f'Изображение сохранено по пути: {result_path}')  # Проверяем, что изображение сохранено