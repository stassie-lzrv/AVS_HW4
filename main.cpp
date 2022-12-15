#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <sstream>
#include <stdexcept>

using namespace std;

// Структура для передачи параметров в функцию поток
struct args_t {
    pthread_mutex_t mutex;
    int index;
};

// status - 0 - пчела в улье; 1 - пчела собирает мед
struct bee_t {
    int index;
    int status;
};

// Вектор с пчелами
static vector<bee_t> hive;
static const int honey_max_count = 30;
static int honey_count;
// Вектор с сообщениями для записи в файл
static vector<string> logs;
static bool run = true;
static bool isFileInput = false;


// Функция потока отвечающая за поведение пчел
void* bee_thread(void *args) {
    args_t *arg = (args_t*) args;
    pthread_mutex_t mutex = arg->mutex;
    int index = arg->index;
    while(true) {
        // Проверяем что нам не надо завершать работу
        pthread_mutex_lock(&mutex);
        if (!run) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        sleep(index + 2);
        pthread_mutex_lock(&mutex);
        // Если пчела улетела, то не трогаем
        if (hive[index].status == 1) {
            pthread_mutex_unlock(&mutex);
            sleep(3);
            continue;
        }
        // Проверяем что в улье кроме нее нет других пчел
        int bee_count = 0;
        for (int i = 0; i < hive.size(); i++) {
            if (hive[i].status == 0)
                bee_count += 1;
        }
        if (bee_count == 1) {
            pthread_mutex_unlock(&mutex);
            sleep(3);
            continue;
        }
        pthread_mutex_unlock(&mutex);
        // Выбираем что делать пчеле: 0 - сидит в улье, 1,2 - летит за медом
        int choice = rand() % 3;
        if (choice == 0) {
            sleep(rand() % 5 + 5);
        } else {
            stringstream ss;
            ss << "Пчела #" << index << " отправилась за медом\n";
            if (isFileInput) {
                logs.push_back(ss.str());
            } else {
                cout << ss.str();
            }
            // Ставим статус пчеле что она улетела
            pthread_mutex_lock(&mutex);
            hive[index].status = 1;
            pthread_mutex_unlock(&mutex);
            // Даем время пчеле на сбор меда
            sleep(rand() % 5 + 5);
            pthread_mutex_lock(&mutex);
            // Увеличивем количество меда и ставим статус что она в улье
            honey_count += 1;
            hive[index].status = 0;
            ss.str(string());
            ss << "Пчела #" << index << " принесла мед, количество меда в улье: " << honey_count << "\n";
            if (isFileInput) {
                logs.push_back(ss.str());
            } else {
                cout << ss.str();
            }
            pthread_mutex_unlock(&mutex);
        }
    }
}

// Функция потока для удаления лишнего меда при привышении предела
void* honey_thread(void *args) {
    // Проверяем что нам не надо завершать работу
    args_t *arg = (args_t*) args;
    pthread_mutex_t mutex = arg->mutex;
    while(true) {
        pthread_mutex_lock(&mutex);
        if (!run) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        // Следим за количеством меда, если его больше 30, то уменьшаем его значение на случайную величину
        if (honey_count > honey_max_count) {
            honey_count -= rand() % 15 + 10;
            stringstream ss;
            ss << "Уменьшение количества меда в улье, новое значение: " << honey_count << "\n";
            if (isFileInput) {
                logs.push_back(ss.str());
            } else {
                cout << ss.str();
            }
        }
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}


// Функция потока отвечающая за поведение Винни-Пуха
void* pooh_thread(void *args) {
    args_t *arg = (args_t*) args;
    pthread_mutex_t mutex = arg->mutex;
    while(true) {
        // Проверяем что нам не надо завершать работу
        pthread_mutex_lock(&mutex);
        if (!run) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        bool eat_honey = true;
        // Проверяем количество меда, если больше половины, то отправляемся за ним
        if (honey_count >= honey_max_count / 2) {
            // Считаем количество пчел в улье
            int bee_count = 0;
            for (int i = 0; i < hive.size(); i++) {
                if (hive[i].status == 0)
                    bee_count++;
            }
            // Если меньше 3, то забираем мед, иначе уходим
            if (bee_count < 3) {
                honey_count = 0;
                stringstream ss;
                ss << "Винни-Пух забрал мед\n";
                if (isFileInput) {
                    logs.push_back(ss.str());
                } else {
                    cout << ss.str();
                }
            } else {
                eat_honey = false;
                stringstream ss;
                ss << "Винни-Пуха покусали, он убежал\n";
                if (isFileInput) {
                    logs.push_back(ss.str());
                } else {
                    cout << ss.str();
                }
            }
        }
        pthread_mutex_unlock(&mutex);
        // Если покусали, то спим дольше
        if (!eat_honey) {
            sleep(10);
        } else {
            sleep(2);
        }
    }
}

// Функция потока которая отслеживает нажатие кнопки выхода
void* exit_thread(void *args) {
    while(cin.get() != 'q');
}


int main(int argc, char const *argv[]) {
    // Проверка на правильность ввода
    if (argc < 3) {
        cout << "Неправильное количество аргументов!\n";
        return 0;
    }
    int n;
    string output_filename = "";
    if (strcmp(argv[1], "-c") == 0) {
        try {
            n = atoi(argv[2]);
        } catch (exception e) {
            cout << "Неверное количество пчел!\n";
            return 0;
        }
        if (n < 4) {
            cout << "Неверное количество пчел!\n";
            return 0;
        }
    } else if (strcmp(argv[1], "-f") == 0) {
        if (argc < 4) {
            cout << "Неправильное количество аргументов!\n";
            return 0;
        }
        ifstream fin;
        fin.open(argv[2]);
        if (!fin) {
            cout << "Неверный входной файл\n";
            return 0;
        }
        fin >> n;
        fin.close();
        if (n < 4) {
            cout << "Неверное количество пчел!\n";
            return 0;
        }
        isFileInput = true;
        output_filename = argv[3];
    } else if (strcmp(argv[1], "-r") == 0) {
        if (argc < 4) {
            cout << "Неправильное количество аргументов!\n";
            return 0;
        }
        try {
            int low = atoi(argv[2]);
            int high = atoi(argv[3]);
            if (low < 4 || low >= high) {
                throw invalid_argument("");
            }
            n = rand() % (high - low) + low;
        } catch (exception e) {
            cout << "Неправильные границы для случайной генерации\n";
            return 0;
        }
        if (argc == 5) {
            isFileInput = true;
            output_filename = argv[4];
        }
    } else {
        cout << "Неправильное значение флага\n";
        return 0;
    }


    srand(time(nullptr));
    stringstream ss;
    ss << "Программа запущена с " << n << " пчелами\n";
    if (isFileInput) {
        logs.push_back(ss.str());
    } else {
        cout << ss.str();
    }
    cout << "Нажмите 'q' чтобы остановить программу\n";
    sleep(1);

    // Объявляем все необходимые потки
    pthread_t threads[n + 2];
    pthread_t controller;
    // Создаем мьютекс для управления обращения к общим данным
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr);

    // Создаем вектор пчел
    hive = vector<bee_t>(n);
    for (int i = 0; i < n; i++) {
        bee_t bee = {i, 0};
        hive[i] = bee;
    }
    args_t params[n];
    // Создаем поток под каждую пчелу
    for (int i = 0; i < n; i++) {
        params[i].mutex = mutex;
        params[i].index = i;
        pthread_create(&threads[i], nullptr, bee_thread, &params[i]);
    }
    // Создаем поток под винни пуха и управлением количеста меда в улье
    pthread_create(&threads[n], nullptr, pooh_thread, &params[0]);
    pthread_create(&threads[n + 1], nullptr, honey_thread, &params[0]);

    // Создаем поток для отслеживания нажатия кнопки 'q'
    pthread_create(&controller, nullptr, exit_thread, nullptr);

    // Делаем джоин чтобы программа не заершалась пока не нажмем 'q'
    pthread_join(controller, nullptr);
    // Когда выходим, останавливаем другие потоки
    pthread_mutex_lock(&mutex);
    run = false;
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
    // Если вывод в файл, то записываем все строчки в него
    if (isFileInput) {
        cout << "Сохранение логов в файл\n";
        ofstream fout;
        fout.open(output_filename);
        for (string line: logs) {
            fout << line;
        }
        fout.close();
    }
    cout << "Выход из программы\n";
    return 0;
}
