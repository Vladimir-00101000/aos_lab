#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int pipe_to_child[2];
    int pipe_to_parent[2];

    pipe(pipe_to_child);
    pipe(pipe_to_parent);

    int pid = fork();

    if (pid < 0) {
        printf("Ошибка при создании процесса\n");
        exit(1);
    }

    if (pid > 0) {
        close(pipe_to_child[0]);
        close(pipe_to_parent[1]);

        char buf[100];
        int bytes_read;

        printf("Родитель: введите сообщение (Ctrl+D для выхода)\n");

        while(1) {
            if (fgets(buf, sizeof(buf), stdin) != NULL) {
                write(pipe_to_child[1], buf, strlen(buf));
            } else {
                close(pipe_to_child[1]);
                break;
            }

            bytes_read = read(pipe_to_parent[0], buf, sizeof(buf));
            if (bytes_read <= 0) break;

            buf[bytes_read] = '\0';
            printf("Родитель получил: %s", buf);
        }
    }
    else {
        close(pipe_to_child[1]);
        close(pipe_to_parent[0]);

        char buf[100];
        int bytes_read;

        while(1) {
            bytes_read = read(pipe_to_child[0], buf, sizeof(buf));
            if (bytes_read <= 0) break;

            buf[bytes_read] = '\0';
            printf("Ребенок получил: %s", buf);

            printf("Ребенок: введите ответ\n");
            if (fgets(buf, sizeof(buf), stdin) != NULL) {
                write(pipe_to_parent[1], buf, strlen(buf));
            } else {
                close(pipe_to_parent[1]);
                break;
            }
        }
    }

    return 0;
}
