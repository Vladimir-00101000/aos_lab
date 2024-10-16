#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    // Убедимся, что файл передан как аргумент командной строки
    if (argc != 2) {
        fprintf(stderr, "Необходимо указать имя файла!\n");
        return 1;
    }

    const char* filename = argv[1];

    // Создание нового файла с правом только на чтение
    int fileDesc = open(filename, O_CREAT | O_WRONLY, S_IRUSR);
    if (fileDesc == -1) {
        perror("Ошибка при создании файла");
        return 1;
    }

    // Запись информации в файл
    if (write(fileDesc, "строка 1\n", 8) != 8 || write(fileDesc, "строка 2\n", 8) != 8 || write(fileDesc, "строка 3\n", 8) != 8) {
        perror("Ошибка при записи данных");
        return 1;
    }

    // Закрытие файла после записи
    if (close(fileDesc) == -1) {
        perror("Ошибка при закрытии файла");
        return 1;
    }

    // Открытие файла на чтение и вывод информации на экран
    fileDesc = open(filename, O_RDONLY);
    if (fileDesc == -1) {
        perror("Ошибка при открытии файла на чтение");
        return 1;
    }

    char buffer[1024] = {0};
    while (!feof(fd)) {
        ssize_t bytesRead = read(fileDesc, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            printf("%.*s", (int)bytesRead, buffer);
        }
    }

    // Проверка возможности открытия файла на чтение и запись
    fileDesc = open(filename, O_RDWR);
    if (fileDesc == -1) {
        puts("Файл открыт только на чтение.");
    } else {
        puts("Доступна запись в файл.");
    }

    return 0;
}
