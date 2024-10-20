//gcc task2.c -o task2
//./task2 example.txt 400
#include <stdio.h>
#include <stdlib.h> // функции общего назначения
#include <fcntl.h> // файловые дескрипторы
#include <unistd.h> //системные вызовы

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <filename> <permissions>\n", argv[0]);
        return 1;
    }

    // Создание нового файла с указанными правами доступа
    int fd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, atoi(argv[2]));
    if (fd == -1) {
        perror("Unable to create file");
        return 1;
    }

    // Запись в файл нескольких строк
    write(fd, "First line\n", 11);
    write(fd, "Second line\n", 12);

    // Закрытие файла
    close(fd);

    // Повторное открытие файла на чтение и вывод содержимого
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Unable to open file for reading");
        return 1;
    }

    char buffer[256];
    int bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, bytes_read);  // Вывод содержимого на экран
    }

    // Закрытие файла
    close(fd);

    // Повторное открытие файла на чтение и запись
    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("Unable to open file for reading and writing");
        return 1;
    }

    // Проверка результата системного вызова open
    printf("Successfully opened file for reading and writing\n");

    // Закрытие файла
    close(fd);

    return 0;
}
