#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void child_process(int pipe_fd) {
    char buf[100];

    printf("Введите текст (Ctrl+D для выхода):\n");
    while(fgets(buf, sizeof(buf), stdin) != NULL) {
        write(pipe_fd, buf, strlen(buf));
    }

    close(pipe_fd);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        child_process(atoi(argv[1]));
        return 0;
    }

    int pipe_fd[2];
    pipe(pipe_fd);

    int pid = fork();

    if (pid > 0) {
        close(pipe_fd[1]);

        char buf[100];
        int bytes_read;

        while((bytes_read = read(pipe_fd[0], buf, sizeof(buf))) > 0) {
            buf[bytes_read] = '\0';
            printf("Получено: %s", buf);
        }

        close(pipe_fd[0]);
    }
    else {
        close(pipe_fd[0]);

        char fd_str[10];
        sprintf(fd_str, "%d", pipe_fd[1]);

        execl(argv[0], argv[0], fd_str, NULL);

        printf("Ошибка при запуске exec\n");
        exit(1);
    }

    return 0;
}
