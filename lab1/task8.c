#include <stdio.h>
#include <stdlib.h>

void copy_file(FILE *src, FILE *dest) {
    char buffer[1024]; // Буфер для хранения данных
    size_t bytes;

    // Чтение из исходного файла и запись в целевой файл
    while ((bytes = fread(buffer, sizeof(char), sizeof(buffer), src)) > 0) {
        fwrite(buffer, sizeof(char), bytes, dest);
    }
}

int main(int argc, char *argv[]) {
    FILE *src, *dest;

    if (argc == 3) {
        // Открываем файлы, указанные в аргументах командной строки
        src = fopen(argv[1], "rb"); // Открываем для чтения в бинарном режиме
        if (src == NULL) {
            perror("Error opening source file");
            return 1;
        }

        dest = fopen(argv[2], "wb"); // Открываем для записи в бинарном режиме
        if (dest == NULL) {
            perror("Error opening destination file");
            fclose(src);
            return 1;
        }

        // Копируем файл
        copy_file(src, dest);

        // Закрываем файлы
        fclose(src);
        fclose(dest);
        printf("File copied from %s to %s\n", argv[1], argv[2]);
    } else {
        // Используем стандартный ввод и вывод
        printf("Copying from standard input to standard output. Press Ctrl+D (EOF) to finish.\n");
        copy_file(stdin, stdout);
    }

    return 0;
}
