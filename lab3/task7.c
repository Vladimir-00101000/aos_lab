#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

volatile sig_atomic_t signal_received = 0;

void sigusr1_handler(int signo) {
    signal_received = 1;
}

int main() {
    signal(SIGUSR1, sigusr1_handler);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf("Дочерний процесс %d ожидает сигнал...\n", getpid());

        pause();

        printf("Дочерний процесс %d получил сигнал.\n", getpid());
        sleep(3);
        kill(getppid(), SIGUSR1);
        exit(EXIT_SUCCESS);
    }

    sleep(2);

    kill(pid, SIGUSR1);

    while (!signal_received) {
        printf("Родительский процесс %d работает...\n", getpid());
        sleep(1);
    }

    printf("Сигнал SIGUSR1 получен! Завершение родительского процесса.\n");

    int status;
    pid_t child_pid = wait(&status);

    if (child_pid > 0) {
        printf("Дочерний процесс %d завершился с кодом %d\n", child_pid, WEXITSTATUS(status));
    }

    return 0;
}
