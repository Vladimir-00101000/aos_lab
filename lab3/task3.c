#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void sigchld_handler(int signo) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    signal(SIGCHLD, sigchld_handler);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf("Дочерний процесс %d работает...\n", getpid());
        sleep(5);
        printf("Дочерний процесс %d завершен.\n", getpid());
        exit(EXIT_SUCCESS);
    }

    while (1) {
        printf("Родительский процесс %d работает...\n", getpid());
        sleep(1);
    }

    return 0;
}
