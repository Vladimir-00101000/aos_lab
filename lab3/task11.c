#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

volatile sig_atomic_t got_signal = 0;

void handler(int signo) {
    got_signal = 1;
}

int main() {
    pid_t pid;
    sigset_t mask, oldmask;

    struct sigaction sa = {0};
    sa.sa_handler = handler;
    sigaction(SIGUSR1, &sa, NULL);

    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    pid = fork();

    if (pid == 0) {
        printf("Потомок начал работу (PID: %d)\n", getpid());
        sleep(2);
        printf("Потомок отправляет сигнал родителю\n");
        kill(getppid(), SIGUSR1);
        exit(0);
    } else {
        printf("Родитель ждет сигнал (PID: %d)\n", getpid());
        while (!got_signal) {
            sigsuspend(&oldmask);
        }
        printf("Родитель получил сигнал и завершается\n");
    }

    return 0;
}
