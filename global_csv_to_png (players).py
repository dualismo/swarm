from PIL import Image
import numpy as np
import csv

# параметры изображения
cpp_height = 100
cpp_width = 100
scale = 5
scaled_height = cpp_height * scale
scaled_width = cpp_width * scale


def converting(name):
    name_red = "output/players/redteam/" + name
    name_sea = "output/players/seateam/" + name
    # создание массива нулей нужной размерности
    data_red = np.zeros((cpp_height, cpp_width))
    data_sea = np.zeros((cpp_height, cpp_width))

    # заполнение массива данными из файла csv
    with open(name_red + ".csv") as csvfile:
        lines = csv.reader(csvfile, delimiter=';')
        i = 0
        for line in lines:
            for j in range(len(line)):
                data_red[i][j] = float(line[j])
            i += 1

    # заполнение массива данными из файла csv
    with open(name_sea + ".csv") as csvfile:
        lines = csv.reader(csvfile, delimiter=';')
        i = 0
        for line in lines:
            for j in range(len(line)):
                data_sea[i][j] = float(line[j])
            i += 1

    # замена значений в клетках с игроками на 1
    for i in range(cpp_height):
        for j in range(cpp_width):
            if (data_red[i][j] > 0):
                data_red[i][j] = 1
            if (data_sea[i][j] > 0):
                data_sea[i][j] = 1   


    # масштабирование данных для масштабирования изображения
    scaled_red_data = np.zeros((scaled_height, scaled_width))
    scaled_sea_data = np.zeros((scaled_height, scaled_width))
    for i in range(scaled_height):
        for j in range(scaled_width):
            scaled_red_data[i][j] = data_red[i // scale][j // scale]
            scaled_sea_data[i][j] = data_sea[i // scale][j // scale]

    
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
            value_red = int(scaled_red_data[j][i]) * 255
            value_sea = int(scaled_sea_data[j][i]) * 255
            pixel_map[i, j] = (value_red, 0, value_sea)

    # сохранение изображения
    image.save("mix/players/" + name + ".png", format="png")


for i in range(1, 101):
    converting("shot" + str(i))
    print(i, "done")