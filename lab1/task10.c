#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Использование: %s filename\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Ошибка открытия файла");
        return 1;
    }

    off_t size = lseek(fd, 0, SEEK_END);

    char c;
    while (size > 0) {
        lseek(fd, --size, SEEK_SET);
        if (read(fd, &c, 1) > 0) {
            write(STDOUT_FILENO, &c, 1);
        }
    }

    close(fd);
    return 0;
}
