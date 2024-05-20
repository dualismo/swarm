#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

const float PI = 3.141592653589793238463;  // число Пи

unsigned int number_one = 1;  // для записи в файл

unsigned int ROWS = 120;  // количество строк поля
unsigned int COLS = 120;  // количество столбцов поля
unsigned int STEP = 100;  // количество итераций
unsigned int BASE = 36;   // длина базы


// заполнение полей на основе файла
void LoadFromFile(const std::string&);

// выгрузка данных в csv файл
void SaveToFile(const std::string&, unsigned int**&, unsigned int&);


namespace Red {

    unsigned int SIZE = 1000;  // размер команды
    unsigned int NORM = 0;  // переменная для нормализации
    unsigned int WINS = 0;  // счётчик побед
    unsigned int HALT = 0;  // счётчик ходов, когда юнит не смог сделать шаг

    // параметры для математических распределений
    float A = -0.0001;
    float B = -0.001;

    // текущее поле
    static unsigned int** field = nullptr;

    // координаты игроков команды (соответствие одному игроку двух координат: х и у)
    static unsigned int** position = nullptr;

    // поле с координтатами игроков команды (о <=> игрока нет, другое число <=> индекс игрока + 1)
    static unsigned int** field_with_players = nullptr;


    // --------------------- Fucntions -----------------------------------

    // появление одного юнита на своей базе
    void Emergence(const int&);

    // много математики для вычислений
    float Inferno(const int&, const int&);

    // рассчёт значения для потенциального шага игрока Red
    float Calculation(const int&, const int&);

    // итерация команды
    void Iteration();
}

namespace Sea {

    unsigned int SIZE = 1000;  // размер команды
    unsigned int NORM = 0;  // переменная для нормализации
    unsigned int WINS = 0;  // счётчик побед
    unsigned int HALT = 0;  // счётчик ходов, когда юнит не смог сделать шаг

    // параметры для математических распределений
    float A = -0.0001;
    float B = -0.001;

    // текущее поле
    static unsigned int** field = nullptr;

    // координаты игроков команды (соответствие одному игроку двух координат: х и у)
    static unsigned int** position = nullptr;

    // поле с координтатами игроков команды (о <=> игрока нет, другое число <=> индекс игрока + 1)
    static unsigned int** field_with_players = nullptr;


    // --------------------- Fucntions -----------------------------------

    // появление одного юнита на своей базе
    void Emergence(const int&);

    // много математики для вычислений
    float Inferno(const int&, const int&);

    // рассчёт значения для потенциального шага игрока Red
    float Calculation(const int&, const int&);

    // итерация команды
    void Iteration();
}

int main()
{
    // название файла с параметрами настройки
    const string settings = "settings.txt";

    // для названия файла при сохранении срезов
    string shot = "";

    // заполнение полей на основе файла
    LoadFromFile(settings);

    // Появление игроков на поле
    for (int i = 0; i < Red::SIZE; ++i) Red::Emergence(i);
    for (int i = 0; i < Sea::SIZE; ++i) Sea::Emergence(i);

    // Общий цикл программы
    for (int i = 0; i < STEP; ++i) {

        // Шагает команда атакующих
        Red::Iteration();

        // Шагает команда защищающихся
        Sea::Iteration();

        // выгрузка каждого каждого перцентиля в бд
        if ((i + 1) % (STEP / 100) == 0) {
            shot = "output/field/redteam/shot" + to_string((i + 1) / (STEP / 100)) + ".csv";
            SaveToFile(shot, Red::field, Red::NORM);
            shot = "output/field/seateam/shot" + to_string((i + 1) / (STEP / 100)) + ".csv";
            SaveToFile(shot, Sea::field, Red::NORM);
            shot = "output/players/redteam/shot" + to_string((i + 1) / (STEP / 100)) + ".csv";
            SaveToFile(shot, Red::field_with_players, number_one);
            shot = "output/players/seateam/shot" + to_string((i + 1) / (STEP / 100)) + ".csv";
            SaveToFile(shot, Sea::field_with_players, number_one);
        }


        // экран загрузки в консоли
        if ((i + 1) % (STEP / 100) == 0) {
            system("cls");
            cout << string(45, ' ') << "loaging... " << (i + 1) / (STEP / 100) << '%' << endl;
            cout << '+' << string(100, '-') << '+' << endl;
            cout << '+' << setw(100) << left << string((i + 1) / (STEP / 100), '|') << '+' << endl;
            cout << '+' << string(100, '-') << '+' << endl;
        }
    }

    cout << endl << "FINAL SCORE" << endl;
    cout << "RED WINS:  " << Red::WINS << endl;
    cout << "SEA WINS:  " << Sea::WINS << endl;
    cout << endl;
    cout << "RED STOPS: " << Red::HALT << endl;
    cout << "SEA STOPS: " << Sea::HALT << endl;


    system("pause");
    return 0;
}

// заполнение полей на основе файла
void LoadFromFile(const std::string& filename)
{
    // чтение файла в массив строк с мгновенной обработкой
    std::string lines[9];
    std::ifstream file;
    file.open(filename);
    int i = 0;
    while (getline(file, lines[i])) {
        lines[i].erase(0, lines[i].find(" "));
        i += 1;
    }
    file.close();

    // обновление значений переменных
    ROWS = stoi(lines[0]);
    COLS = stoi(lines[1]);
    STEP = stoi(lines[2]);
    Red::SIZE = stoi(lines[3]);
    Red::A = stof(lines[4]);
    Red::B = stof(lines[5]);
    Sea::SIZE = stoi(lines[6]);
    Sea::A = stof(lines[7]);
    Sea::B = stof(lines[8]);

    // длина базы считается как 30% от наименьшей стороны поля
    if (ROWS <= COLS)
        BASE = int(ROWS * 0.3);
    else
        BASE = int(COLS * 0.3);

    // проверка на превыщение максимальной численности команды
    if (Red::SIZE > pow(BASE, 2) || Sea::SIZE > pow(BASE, 2)) {
        std::cout << "(!) Error. Exceeding maximum number of players allowed." << std::endl;
        std::cout << "The maximum value for a " << ROWS << " x " << COLS << " field is " << pow(BASE, 2) << std::endl;
        exit(-1000);
    }

    // создание массивов текущего распределения каждой команды и положения игроков на поле
    Red::field = new unsigned int* [ROWS];
    Sea::field = new unsigned int* [ROWS];
    Red::field_with_players = new unsigned int* [ROWS];
    Sea::field_with_players = new unsigned int* [ROWS];
    for (int i = 0; i < ROWS; ++i) {
        Red::field[i] = new unsigned int[COLS];
        Sea::field[i] = new unsigned int[COLS];
        Red::field_with_players[i] = new unsigned int [COLS];
        Sea::field_with_players[i] = new unsigned int [COLS];
        for (int j = 0; j < COLS; ++j) {
            Red::field[i][j] = 0;
            Sea::field[i][j] = 0;
            Red::field_with_players[i][j] = 0;
            Sea::field_with_players[i][j] = 0;
        }
    }

    // создание массива для позиций игроков красной команды
    Red::position = new unsigned int* [Red::SIZE];
    for (int i = 0; i < Red::SIZE; ++i) {
        Red::position[i] = new unsigned int[2];
        Red::position[i][0] = 0;
        Red::position[i][1] = 0;
    }

    // создание массива для позиций игроков синей команды
    Sea::position = new unsigned int* [Sea::SIZE];
    for (int i = 0; i < Sea::SIZE; ++i) {
        Sea::position[i] = new unsigned int[2];
        Sea::position[i][0] = 0;
        Sea::position[i][1] = 0;
    }
}

// выгрузка данных в csv файл
void SaveToFile(const std::string& filename, unsigned int**& field, unsigned int& NORM) {
    std::ofstream file;
    file.open(filename);
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            file << float(field[i][j]) / NORM;
            if (j != COLS - 1)
                file << ';';
        }
        file << std::endl;
    }
    file.close();
}

//================================= RED ==================================

// появление одного юнита на своей базе
void Red::Emergence(const int& index)
{

    // локальные переменные для строки и столбца
    static unsigned int row = 0, col = 0;
    row = ROWS - BASE + rand() % BASE;
    col = rand() % BASE;

    // проверка координаты на использование другим игроком
    // бесконечный цикл проверки, который прерывается нахождением свободной координаты
    while (true) {
        if (Red::field_with_players[row][col] > 0 || Sea::field_with_players[row][col] > 0)
        {
            // если клетка занята, то получаем новые координаты
            row = ROWS - BASE + rand() % BASE;
            col = rand() % BASE;
        }
        else 
        {
            // если клетка свободна, то выходим из бесконечного цикла
            break;
        }
    }

    // игрок делает шаг
    Red::field_with_players[row][col] = index + 1;
    Red::position[index][0] = row;
    Red::position[index][1] = col;
    Red::field[row][col] += 1;
    Red::NORM += 1;
}

// много математики для вычислений
float Red::Inferno(const int& x, const int& y) {

    // переменная для ответа
    static float answer = 0.0;

    // локальные переменные для упрощения читабельности
    static float a = Red::A, b = Red::B;

    // Идеальное распределение поля без учета игроков
    answer = exp(a * (pow(0 - x, 2) + pow(COLS - 1 - y, 2))) / (PI / 4 / abs(a) * erf(ROWS * sqrt(abs(a))) * erf(COLS * sqrt(abs(a))));

    for (int i = 0; i < Sea::SIZE; ++i)
        answer += (1 - exp(b * (pow(x - Sea::position[i][0], 2) + pow(y - Sea::position[i][1], 2)))) /
        (ROWS * COLS - PI / 4 / abs(b) * (erf(Sea::position[i][0] * sqrt(abs(b))) - erf((Sea::position[i][0] - ROWS) * sqrt(abs(b)))) * (erf(Sea::position[i][1] * sqrt(abs(b))) - erf((Sea::position[i][1] - COLS) * sqrt(abs(b)))));

    return (answer - Red::field[x][y] / Red::NORM);
}

// функция рассчёта значения для потенциального шага игрока redteam
float Red::Calculation(const int& x, const int& y)
{
    // проверка на выход за границы поля
    if (x >= 0 && x <= (ROWS - 1) && y >= 0 && y <= (COLS - 1))
    {
        // проверка наличия в ячейке других игроков
        if (Red::field_with_players[x][y] > 0 || Sea::field_with_players[x][y] > 0)
            return -1.0;

        // если ничего не произошло, то возвращаем значение поля для точки (х, у)
        return Red::Inferno(x, y);
    }

    // возвращаем -1, если потенциальная клетка не является полем
    return -1.0;
}

// ходьба всей команды в течение одной итерации
void Red::Iteration() {

    // универсальный массив вариантов для ходьбы
    // пара содержит значение для шага и значение для сравнения
    static float variant[8] = { 0 };

    // цикл для хода каждого игрока команды
    for (int i = 0; i < Red::SIZE; ++i) {

        // расчёт значений для каждой клетки, в которую можно пойти
        variant[0] = Red::Calculation(Red::position[i][0] + 1, Red::position[i][1] + 1);
        variant[1] = Red::Calculation(Red::position[i][0] + 1, Red::position[i][1]    );
        variant[2] = Red::Calculation(Red::position[i][0] + 1, Red::position[i][1] - 1);
        variant[3] = Red::Calculation(Red::position[i][0]    , Red::position[i][1] - 1);
        variant[4] = Red::Calculation(Red::position[i][0] - 1, Red::position[i][1] - 1);
        variant[5] = Red::Calculation(Red::position[i][0] - 1, Red::position[i][1]    );
        variant[6] = Red::Calculation(Red::position[i][0] - 1, Red::position[i][1] + 1);
        variant[7] = Red::Calculation(Red::position[i][0]    , Red::position[i][1] + 1);


        // локальнаые переменные для максимального элемента и его индекса
        static float maximum = 0.0;
        static unsigned int choice = 0;

        // поиск максимального элемента
        maximum = variant[0];
        choice = 0;
        for (int j = 1; j < 8; ++j)
            if (variant[j] > maximum) {
                choice = j;
                maximum = variant[j];
            }


        // проверка клеток на невозможность ходить (юнит будет стоять)
        if (maximum == -1.0) {
            choice = 8;
            Red::HALT += 1;
        }

        // очистка значения игрока на поле с игроками перед назначением нового положения
        Red::field_with_players[Red::position[i][0]][Red::position[i][1]] = 0;

        // назначение следующей позиции игрока
        switch (choice) {
        case 0:
            Red::position[i][0] = Red::position[i][0] + 1;
            Red::position[i][1] = Red::position[i][1] + 1;
            break;
        case 1:
            Red::position[i][0] = Red::position[i][0] + 1;
            Red::position[i][1] = Red::position[i][1];
            break;
        case 2:
            Red::position[i][0] = Red::position[i][0] + 1;
            Red::position[i][1] = Red::position[i][1] - 1;
            break;
        case 3:
            Red::position[i][0] = Red::position[i][0];
            Red::position[i][1] = Red::position[i][1] - 1;
            break;
        case 4:
            Red::position[i][0] = Red::position[i][0] - 1;
            Red::position[i][1] = Red::position[i][1] - 1;
            break;
        case 5:
            Red::position[i][0] = Red::position[i][0] - 1;
            Red::position[i][1] = Red::position[i][1];
            break;
        case 6:
            Red::position[i][0] = Red::position[i][0] - 1;
            Red::position[i][1] = Red::position[i][1] + 1;
            break;
        case 7:
            Red::position[i][0] = Red::position[i][0];
            Red::position[i][1] = Red::position[i][1] + 1;
            break;
        case 8:
            Red::position[i][0] = Red::position[i][0];
            Red::position[i][1] = Red::position[i][1];
            break;
        }

        if (Red::position[i][0] >= 0 && Red::position[i][0] <= (BASE - 1) && 
            Red::position[i][1] >= (COLS - BASE) && Red::position[i][1] <= (COLS - 1))
        {
            // если игрок достигает поля соперника, то он телепортируется на свою базу
            // и зарабатывает 1 очко для команды 
            Red::Emergence(i);
            Red::WINS += 1;
        }
        else
        {
            // иначе делает обычный ход
            Red::field_with_players[Red::position[i][0]][Red::position[i][1]] = i + 1;
            Red::field[Red::position[i][0]][Red::position[i][1]] += 1;
            Red::NORM += 1;
        }
    }
}


//================================= SEA ==================================

// появление одного юнита на своей базе
void Sea::Emergence(const int& index)
{

    // локальные переменные для строки и столбца
    static unsigned int row = 0, col = 0;
    row = rand() % BASE;
    col = COLS - BASE + rand() % BASE;

    // проверка координаты на использование другим игроком
    // бесконечный цикл проверки, который прерывается нахождением свободной координаты
    while (true) {
        if (Sea::field_with_players[row][col] > 0)
        {
            // если клетка занята, то получаем новые координаты
            row = rand() % BASE;
            col = COLS - BASE + rand() % BASE;
        }
        else
        {
            // если клетка свободна, то выходим из бесконечного цикла
            break;
        }
    }

    // игрок делает шаг
    Sea::field_with_players[row][col] = index + 1;
    Sea::position[index][0] = row;
    Sea::position[index][1] = col;
    Sea::field[row][col] += 1;
    Sea::NORM += 1;
}

// много математики для вычислений
float Sea::Inferno(const int& x, const int& y) {

    // переменная для ответа
    static float answer = 0.0;

    // локальные переменные для упрощения читабельности
    static float a = Sea::A, b = Sea::B;

    // Идеальное распределение поля без учета игроков
    answer = exp(a * (pow(x, 2) + pow(COLS - 1 - y, 2))) / (PI / 4 / abs(a) * erf(ROWS * sqrt(abs(a))) * erf(COLS * sqrt(abs(a))));
    if (x >= 0 && x <= (BASE - 1) && y >= (COLS - BASE) && y <= (COLS - 1))
        answer /= 100;


    for (int i = 0; i < Sea::SIZE; ++i)
        answer += exp(b * (pow(x - Red::position[i][0], 2) + pow(y - Red::position[i][1], 2))) /
        (PI / 4 / abs(b) * (erf(Red::position[i][0] * sqrt(abs(b))) - erf((Red::position[i][0] - ROWS) * sqrt(abs(b)))) * (erf(Red::position[i][1] * sqrt(abs(b)) - erf((Red::position[i][1] - COLS) * sqrt(abs(b))))));

    return (answer - Sea::field[x][y] / Sea::NORM);
}

// функция рассчёта значения для потенциального шага игрока redteam
float Sea::Calculation(const int& x, const int& y)
{
    // проверка на выход за границы поля
    if (x >= 0 && x <= (ROWS - 1) && y >= 0 && y <= (COLS - 1))
    {
        // проверка наличия в ячейке союзников и противников
        if (Sea::field_with_players[x][y] > 0)
            return -1.0;
        else if (Red::field_with_players[x][y] > 0)
            return 1.0 * Red::field_with_players[x][y];

        // если ничего не произошло, то возвращаем значение поля для точки (х, у)
        return Sea::Inferno(x, y);
    }

    // возвращаем -1, если потенциальная клетка не является полем
    return -1.0;
}

// ходьба всей команды в течение одной итерации
void Sea::Iteration() {

    // универсальный массив вариантов для ходьбы
    // пара содержит значение для шага и значение для сравнения
    static float variant[8] = { 0 };

    // цикл для хода каждого игрока команды
    for (int i = 0; i < Sea::SIZE; ++i) {

        // расчёт значений для каждой клетки, в которую можно пойти
        variant[0] = Sea::Calculation(Sea::position[i][0] + 1, Sea::position[i][1] + 1);
        variant[1] = Sea::Calculation(Sea::position[i][0] + 1, Sea::position[i][1]    );
        variant[2] = Sea::Calculation(Sea::position[i][0] + 1, Sea::position[i][1] - 1);
        variant[3] = Sea::Calculation(Sea::position[i][0],     Sea::position[i][1] - 1);
        variant[4] = Sea::Calculation(Sea::position[i][0] - 1, Sea::position[i][1] - 1);
        variant[5] = Sea::Calculation(Sea::position[i][0] - 1, Sea::position[i][1]    );
        variant[6] = Sea::Calculation(Sea::position[i][0] - 1, Sea::position[i][1] + 1);
        variant[7] = Sea::Calculation(Sea::position[i][0],     Sea::position[i][1] + 1);

        // локальнаые переменные для максимального элемента и его индекса
        static float maximum = 0.0;
        static unsigned int choice = 0;

        // поиск максимального элемента
        maximum = variant[0];
        choice = 0;
        for (int j = 1; j < 8; ++j)
            if (variant[j] > maximum) {
                choice = j;
                maximum = variant[j];
            }


        // проверка клеток на невозможность ходить (юнит будет стоять)
        if (maximum == -1.0) {
            choice = 8;
            Sea::HALT += 1;
        }

        // очистка значения игрока на поле с игроками перед назначением нового положения
        Sea::field_with_players[Sea::position[i][0]][Sea::position[i][1]] = 0;

        // назначение следующей позиции игрока
        switch (choice) {
        case 0:
            Sea::position[i][0] = Sea::position[i][0] + 1;
            Sea::position[i][1] = Sea::position[i][1] + 1;
            break;
        case 1:
            Sea::position[i][0] = Sea::position[i][0] + 1;
            Sea::position[i][1] = Sea::position[i][1];
            break;
        case 2:
            Sea::position[i][0] = Sea::position[i][0] + 1;
            Sea::position[i][1] = Sea::position[i][1] - 1;
            break;
        case 3:
            Sea::position[i][0] = Sea::position[i][0];
            Sea::position[i][1] = Sea::position[i][1] - 1;
            break;
        case 4:
            Sea::position[i][0] = Sea::position[i][0] - 1;
            Sea::position[i][1] = Sea::position[i][1] - 1;
            break;
        case 5:
            Sea::position[i][0] = Sea::position[i][0] - 1;
            Sea::position[i][1] = Sea::position[i][1];
            break;
        case 6:
            Sea::position[i][0] = Sea::position[i][0] - 1;
            Sea::position[i][1] = Sea::position[i][1] + 1;
            break;
        case 7:
            Sea::position[i][0] = Sea::position[i][0];
            Sea::position[i][1] = Sea::position[i][1] + 1;
            break;
        case 8:
            Sea::position[i][0] = Sea::position[i][0];
            Sea::position[i][1] = Sea::position[i][1];
            break;
        }


        if (maximum >= 1)
        {
            // если противник пойман, то + 1 очко команде и телепорт игроков на базы
            // очистка значения игрока на поле с игроками перед назначением нового положения
            Red::field_with_players[Red::position[int(maximum-1)][0]][Red::position[int(maximum-1)][1]] = 0;
            Red::Emergence(int(maximum-1));
            Sea::Emergence(i);
            Sea::WINS += 1;
        }
        else
        {
            // иначе делает обычный ход
            Sea::field_with_players[Sea::position[i][0]][Sea::position[i][1]] = i + 1;
            Sea::field[Sea::position[i][0]][Sea::position[i][1]] += 1;
            Sea::NORM += 1;
        }
    }
}