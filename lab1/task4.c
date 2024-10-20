#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <filename> <permissions>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, atoi(argv[2]));
    if (fd == -1) {
        perror("Unable to create file");
        return 1;
    }

    write(fd, "First line\n", 11);
    write(fd, "Second line\n", 12);
    close(fd);

    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("Unable to open file for reading and writing");
        return 1;
    }

    // Позиционирование на начало файла
    lseek(fd, 0, SEEK_SET);
    char buffer[256];
    int bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read > 0) {
        write(1, buffer, bytes_read);  // Вывод содержимого на экран
    }

    // Позиционирование на начало файла и запись
    lseek(fd, 0, SEEK_SET);
    write(fd, "New line\n", 9); // Запись в начало файла

    // Позиционирование для чтения после записи
    lseek(fd, 0, SEEK_SET);
    bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read > 0) {
        write(1, buffer, bytes_read);  // Вывод обновленного содержимого на экран
    }

    close(fd);
    return 0;
}
