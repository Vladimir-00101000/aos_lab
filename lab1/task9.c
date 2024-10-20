#include <stdio.h>
#include <stdlib.h>

void copy_file() {
    char buffer[1024]; // Буфер для хранения данных
    size_t bytes;

    // Чтение из стандартного ввода и запись в стандартный вывод
    while ((bytes = fread(buffer, sizeof(char), sizeof(buffer), stdin)) > 0) {
        fwrite(buffer, sizeof(char), bytes, stdout);
    }
}

int main(int argc, char *argv[]) {
    FILE *src, *dest;

    if (argc == 3) {
        // Переназначаем стандартный ввод и вывод
        src = freopen(argv[1], "rb", stdin); // Открываем для чтения
        if (src == NULL) {
            perror("Error opening source file");
            return 1;
        }

        dest = freopen(argv[2], "wb", stdout); // Открываем для записи
        if (dest == NULL) {
            perror("Error opening destination file");
            fclose(src);
            return 1;
        }

        // Копируем файл
        copy_file();

        // Закрываем файлы (freopen уже закрывает их)
        fclose(src);
        fclose(dest);
        printf("File copied from %s to %s\n", argv[1], argv[2]);
    } else {
        // Если аргументы отсутствуют, используем стандартный ввод и вывод
        printf("Copying from standard input to standard output. Press Ctrl+D (EOF) to finish.\n");
        copy_file();
    }

    return 0;
}
