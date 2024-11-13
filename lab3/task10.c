#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t signal_received = 0;

void sigusr1_handler(int signo) {
    printf("Обработчик сигнала %d запущен.\n", signo);
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    sleep(3);

    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    signal_received = 1;
    printf("Обработчик сигнала %d завершён.\n", signo);
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigusr1_handler;
    sigaction(SIGUSR1, &sa, NULL);

    for (int i = 0; i < 5; i++) {
        kill(getpid(), SIGUSR1);
        sleep(1);
    }

    while (!signal_received) {
        printf("Ожидание обработки сигнала...\n");
        sleep(1);
    }

    printf("Программа завершена.\n");
    return 0;
}
