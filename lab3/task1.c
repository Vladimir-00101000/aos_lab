#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum) {
    printf("Сигнал %d пойман. Восстанавливаю поведение по умолчанию.\n", signum);
    signal(SIGINT, SIG_DFL);
}

int main() {
    signal(SIGINT, handler);

    while (1) {
        printf("Работаю... Нажмите Ctrl+C для отправки SIGINT.\n");
        sleep(1);
    }

    return 0;
}
