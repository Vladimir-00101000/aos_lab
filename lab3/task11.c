#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t signal_received = 0;


void sigusr1_handler(int signo) {
    printf("Обработчик сигнала %d запущен.\n", signo);
    signal_received = 1;
}

int main() {
    struct sigaction sa;
    sigset_t mask;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigusr1_handler;
    sigaction(SIGUSR1, &sa, NULL);

    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        sleep(2);
        printf("Дочерний процесс %d отправляет сигнал SIGUSR1.\n", getpid());
        kill(getppid(), SIGUSR1);
        exit(0);
    }

    printf("Родительский процесс %d ожидает сигнал...\n", getpid());

    sigsuspend(&mask);

    printf("Родительский процесс %d получил сигнал SIGUSR1.\n", getpid());

    int status;
    wait(&status);

    return 0;
}
