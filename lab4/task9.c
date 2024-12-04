#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/file.h>

int main(int argc, char *argv[]) {
    int pipe_fd[2];
    int pid;
    char buf[100];
    int file;

    if (pipe(pipe_fd) == -1) {
        printf("Ошибка pipe\n");
        return 1;
    }

    pid = fork();
    if (pid == 0) {
        close(pipe_fd[0]);
        file = open(argv[1], O_WRONLY | O_CREAT, 0666);

        if (flock(file, LOCK_EX | LOCK_NB) == -1) {
            printf("Дочерний процесс: Не удалось получить блокировку\n");
        } else {
            printf("Дочерний процесс: Блокировка получена\n");
        }

        for(int i = 0; i < 5; i++) {
            sprintf(buf, "Текст %d\n", i);
            write(file, buf, strlen(buf));
            write(pipe_fd[1], ".", 1);
            sleep(1);
        }

        flock(file, LOCK_UN);
        close(file);
    } else {
        close(pipe_fd[1]);
        file = open(argv[1], O_RDONLY);

        if (flock(file, LOCK_SH | LOCK_NB) == -1) {
            printf("Родительский процесс: Не удалось получить блокировку\n");
        } else {
            printf("Родительский процесс: Блокировка для чтения получена\n");
        }

        char signal;
        while(read(pipe_fd[0], &signal, 1) > 0) {
            read(file, buf, sizeof(buf));
            printf("Прочитано: %s", buf);
        }

        flock(file, LOCK_UN);
        close(file);
    }
    return 0;
}
