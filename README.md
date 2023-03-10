# АBC ИДЗ №4 
## Лазарева Анастасия БПИ217
### Вариант 3 : Задача о Винни-Пухе - 2 или неправильные пчелы. N пчел живет в улье, каждая пчела может собирать мед и сторожить улей (N > 3). Пчела не покинет улей, если кроме нее в нем нет других пчел. Каждая пчела приносит за раз одну порцию меда. Всего в улей может войти тридцать порций меда. Вини-Пух спит пока меда в улье меньше половины, но как только его становится достаточно, он просыпается и пытается достать весь мед из улья. Если в улье находится менее чем три пчелы, Вини-Пух забирает мед, убегает, съедает мед и снова засыпает. Если в улье пчел больше, они кусают Вини-Пуха, он убегает, лечит укус, и снова бежит за медом. Создать многопоточное приложение, моделирующее поведение пчел и медведя. Осуществить балансировку, обеспечивающую циклическое освобождение улья от меда.

### 8 баллов

ВАЖНО: Код программы на 8 баллов полностью удовлетворяет критериям на 4-7.

Модель параллельных вычислений, используемая при разработке многопоточной программы: Общая память

В модели программирования с общей памяти, все процессы совместно используют общее адресное пространство, к которому они асинхронно обращаются с запросами на чтение и запись.

Все потоки обращаются к ветору hive

- Реализовано консольное приложение, решающее поставленную задачу с использованием одного варианта синхропримитивов.
- В программу добавлены комментарии, поясняющие выполняемые действия и описание используемых переменных.
- Реализованны разные типы ввода данных:

Для консольного ввода: ./main -c <кол-во пчел>

Для файлового ввода: ./main -f <входной файл> <выходной файл>

Для случайной генерации: ./main -r <нижнаяя граница> <верхняя граница> <опционально выходной файл>

### Реализация

- Функция потока отвечающая за поведение пчел: void* bee_thread(void *args)
1) Проверка на то нужно ли завершение работы
2) Случайный выбор действия пчелы (остаться в улье/ улетель за медом)
- Функция потока для удаления лишнего меда при привышении предела: void* honey_thread(void *args)
1) Проверка на то нужно ли завершение работы
2) Если меда > 30, то уменьшаем его количество и выводим соответсвующее сообщение
- Функция потока отвечающая за поведение Винни-Пуха: void* pooh_thread(void *args)
1) Проверка на то нужно ли завершение работы
2) Если меда больше половины, то отправляем Винни-Пуха за ним
3) Если Винни-Пух пошел за медом - он забирает мед, если пчел < 3, иначе его кусают и он спит дольше
- Функция потока которая отслеживает нажатие кнопки выхода: void* exit_thread(void *args)
1) Если введено 'q', то выход

### Входные данные

- Пчел должно быть > 3 (по условию задания)
- Для генерации min < max

### Тестирование
- Тест 1 (Винни-Пух не смог забрать мед, улей переполнился)
![img](/img1.png)
![img](/img2.png)
![img](/img3.png)
![img](/img4.png)

- Тест 2 (Неверное количество пчел)
![img](/img5.png)
![img](/img8.png)

- Тест 3 (Винни-Пух забрал мед)
![img](/img6.png)
![img](/img7.png)

- Тест 4 (Случайное значение (результаты в файле output.txt))
![img](/img9.png)

- Тест 4 (Текстовый ввод и вывод (файлы input1.txt и output1.txt))
![img](/img10.png)
