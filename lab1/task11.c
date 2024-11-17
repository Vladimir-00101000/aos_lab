#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Использование: %s file1 file2 ...\n", argv[0]);
        return 1;
    }

    int max_length = 0;
    char *max_file = NULL;

    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            printf("Не удалось открыть файл %s\n", argv[i]);
            continue;
        }

        int length = lseek(fd, 0, SEEK_END);
        close(fd);

        if (length > max_length) {
            max_length = length;
            max_file = argv[i];
        }
    }

    if (max_file) {
        printf("Самый длинный файл: %s\n", max_file);
        printf("Размер: %d байт\n", max_length);
    }

    return 0;
}
