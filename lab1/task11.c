#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> <file2> ... <fileN>\n", argv[0]);
        return 1;
    }

    const char *largest_file = NULL;
    off_t largest_size = 0;

    for (int i = 1; i < argc; i++) {
        struct stat file_stat;

        // Получаем информацию о файле
        if (stat(argv[i], &file_stat) == -1) {
            perror("Error getting file information");
            continue; // Пропускаем файл, если возникла ошибка
        }

        // Проверяем, является ли файл обычным файлом и сравниваем его размер
        if (S_ISREG(file_stat.st_mode)) {
            if (file_stat.st_size > largest_size) {
                largest_size = file_stat.st_size;
                largest_file = argv[i];
            }
        }
    }

    if (largest_file) {
        printf("The largest file is: %s\n", largest_file);
        printf("Size: %lld bytes\n", (long long)largest_size);
    } else {
        printf("No regular files found.\n");
    }

    return 0;
}
