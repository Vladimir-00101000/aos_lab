#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int pipe_one_way[2];
    int pipe_to_child[2];
    int pipe_to_parent[2];

    pipe(pipe_one_way);
    pipe(pipe_to_child);
    pipe(pipe_to_parent);

    int pid = fork();

    if (pid > 0) {
        close(pipe_one_way[0]);
        close(pipe_to_child[0]);
        close(pipe_to_parent[1]);

        char msg1[] = "Привет от родителя (один раз)";
        char msg2[] = "Привет от родителя (двунаправленно)";
        write(pipe_one_way[1], msg1, strlen(msg1) + 1);
        write(pipe_to_child[1], msg2, strlen(msg2) + 1);

        char buf[100];
        read(pipe_to_parent[0], buf, 100);
        printf("Родитель получил: %s\n", buf);
    }
    else {
        close(pipe_one_way[1]);
        close(pipe_to_child[1]);
        close(pipe_to_parent[0]);

        char buf1[100];
        char buf2[100];
        read(pipe_one_way[0], buf1, 100);
        read(pipe_to_child[0], buf2, 100);

        printf("Ребенок получил: %s\n", buf1);
        printf("Ребенок получил: %s\n", buf2);

        char msg[] = "Привет от ребенка";
        write(pipe_to_parent[1], msg, strlen(msg) + 1);
    }

    return 0;
}
