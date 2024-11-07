#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern char **environ;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <имя_исполняемого_файла> [аргументы...]\n", argv[0]);
        return 1;
    }

    printf("Аргументы родительского процесса:\n");
    for (int i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    printf("\nОкружение родительского процесса:\n");
    for (char **env = environ; *env != NULL; env++) {
        printf("%s\n", *env);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Ошибка fork");
        return 1;
    }

    if (pid == 0) {

        printf("\nАргументы дочернего процесса:\n");
        for (int i = 0; i < argc; i++) {
            printf("argv[%d]: %s\n", i, argv[i]);
        }

        printf("\nОкружение дочернего процесса:\n");
        for (char **env = environ; *env != NULL; env++) {
            printf("%s\n", *env);
        }

        execvp(argv[1], &argv[1]);
        perror("Ошибка exec");
        exit(1);
    } else {
        wait(NULL);
    }

    return 0;
}

//Запуск: ./task10 /bin/ls -l
