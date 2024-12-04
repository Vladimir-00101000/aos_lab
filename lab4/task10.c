#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 256

int main() {
    pid_t pid;
    char buffer[BUFFER_SIZE];

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Ошибка при создании процесса\n");
        exit(1);
    }

    if (pid == 0) {
        printf("Дочерний процесс (PID: %d): Готов читать ввод\n", getpid());
        while (1) {
            printf("Дочерний: Введите строку: ");
            if (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
                printf("Дочерний прочитал: %s", buffer);
            }
            sleep(1);
        }
    } else {
        printf("Родительский процесс (PID: %d): Готов читать ввод\n", getpid());
        while (1) {
            printf("Родительский: Введите строку: ");
            if (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
                printf("Родительский прочитал: %s", buffer);
            }
            sleep(1);
        }
    }

    return 0;
}
