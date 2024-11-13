#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void sigchld_handler(int signo) {
    int status;
    pid_t pid = wait(&status);

    if (pid > 0) {
        printf("Дочерний процесс %d завершился с кодом %d\n", pid, WEXITSTATUS(status));
    }
}

int main() {
    signal(SIGCHLD, sigchld_handler);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        for (int i = 0; i < 100; i++) {
            printf("Дочерний процесс %d, итерация %d\n", getpid(), i);
            for (int j = 0; j < 100; j++) {
                // body
            }
        }
        exit(EXIT_SUCCESS);
    }

    while (1) {
        printf("Родительский процесс %d работает...\n", getpid());
        sleep(1);
    }

    return 0;
}
