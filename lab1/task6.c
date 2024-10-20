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

    // Создание файла
    int fd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, atoi(argv[2]));
    if (fd == -1) {
        perror("Unable to create file");
        return 1;
    }
    printf("File created successfully with descriptor: %d\n", fd);

    // Запись данных с разреженностью
    const char *lines[] = {
        "First line\n",
        "Second line\n",
        "Third line\n"
    };

    for (int i = 0; i < 3; i++) {
        // Устанавливаем позицию записи с пропусками
        off_t position = i * 100; // Пропускаем 100 байт между записями
        lseek(fd, position, SEEK_SET);

        // Записываем строку
        if (write(fd, lines[i], strlen(lines[i])) != strlen(lines[i])) {
            perror("Error writing to file");
        }
    }

    // Закрытие файла
    close(fd);
    printf("File closed after writing.\n");

    // Открытие файла для чтения
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Unable to open file for reading");
        return 1;
    }
    printf("File opened for reading with descriptor: %d\n", fd);

    // Чтение содержимого файла
    char buffer[256];
    int bytes_read;
    lseek(fd, 0, SEEK_SET); // Устанавливаем курсор на начало
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, bytes_read);  // Вывод содержимого на экран
    }

    // Закрытие файла
    close(fd);
    printf("File closed after reading.\n");

    return 0;
}
