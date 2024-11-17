#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t count = 0;

void signal_handler(int signo) {
    count++;
    printf("\nНачало обработки сигнала %d (всего: %d)\n", signo, count);

    sleep(3);

    printf("Конец обработки сигнала\n");
}

int main() {
    struct sigaction sa = {0};
    sa.sa_handler = signal_handler;

    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGUSR1);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("PID: %d\n", getpid());
    printf("Для тестирования используйте команду: kill -SIGUSR1 %d\n", getpid());

    while(1) {
        printf("Ожидание сигнала...\n");
        sleep(1);
    }

    return 0;
}
