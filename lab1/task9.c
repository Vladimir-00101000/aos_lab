#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int fd_in, fd_out;
    char buf[4096];
    int n;

    if (argc > 2) {
        fd_in = open(argv[1], O_RDONLY);
        if (fd_in < 0) {
            perror("Ошибка открытия входного файла");
            return 1;
        }

        fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out < 0) {
            perror("Ошибка открытия выходного файла");
            close(fd_in);
            return 1;
        }

        dup2(fd_in, STDIN_FILENO);
        dup2(fd_out, STDOUT_FILENO);
    }

    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, n);
    }

    if (argc > 2) {
        close(fd_in);
        close(fd_out);
    }

    return 0;
}

// ./task9 input.txt output.txt   # Копирование из файла в файл
// ./task9                        # Копирование из stdin в stdout
