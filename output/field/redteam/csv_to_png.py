from PIL import Image
import numpy as np
import csv

# параметры изображения
cpp_height = 90
cpp_width = 160
scale = 10
scaled_height = cpp_height * scale
scaled_width = cpp_width * scale


def converting(filename):
    # создание массива нулей нужной размерности
    data = np.zeros((cpp_height, cpp_width))

    # заполнение массива данными из файла csv
    with open(filename + ".csv") as csvfile:
        lines = csv.reader(csvfile, delimiter=';')
        i = 0
        for line in lines:
            for j in range(len(line)):
                data[i][j] = float(line[j])
            i += 1

    # нахождение максимального элемента в массиве        
    data_max = data.max()

    # нормализация всего массива по максимальному элементу (для улучшения качества изображения)
    for i in range(cpp_height):
        for j in range(cpp_width):
            data[i][j] = data[i][j] / data_max


    # масштабирование данных для масштабирования изображения
    scaled_data = np.zeros((scaled_height, scaled_width))
    for i in range(scaled_height):
        for j in range(scaled_width):
            scaled_data[i][j] = data[i // scale][j // scale]

    
    # инвертирование параметров для работы с функциями графики
    height = scaled_width
    width = scaled_height

    # создание изображения
    image = Image.new(mode="RGB", size=(height, width), color="white")
    
    # извлечение карты пикселей изображения
    pixel_map = image.load()
    
    # изменение каждого пикселя в зависимости от значения массива
    # !!! ОСТОРОЖНО !!! ИНВЕРТИРОВАНЫ i и j
    for i in range(height): 
        for j in range(width):
            value = int((1 - scaled_data[j][i]) * 255)
            pixel_map[i, j] = (value, value, value)

    # сохранение изображения
    image.save(filename + ".png", format="png")


for i in range(1, 101):
    converting("shot" + str(i))
    print(i, "done")