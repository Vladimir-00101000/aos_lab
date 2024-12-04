#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int pipe_fd[2];
    pipe(pipe_fd);

    int pid = fork();

    if (pid < 0) {
        printf("Ошибка при создании процесса\n");
        exit(1);
    }

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

        char buf[100];

        while(fgets(buf, sizeof(buf), stdin) != NULL) {
            write(pipe_fd[1], buf, strlen(buf));
        }

        close(pipe_fd[1]);
        exit(0);
    }

    return 0;
}
