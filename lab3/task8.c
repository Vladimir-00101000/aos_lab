#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

volatile sig_atomic_t signal_received = 0;

void sigusr1_handler(int signo) {
    signal_received = 1;
    printf("Дочерний процесс %d получил сигнал SIGUSR1.\n", getpid());
}

int main() {
    signal(SIGUSR1, sigusr1_handler);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        signal(SIGUSR1, sigusr1_handler);

        printf("Дочерний процесс %d начинает выполнение...\n", getpid());

        for (int i = 0; i < 10; i++) {
            printf("Дочерний процесс %d, итерация %d\n", getpid(), i);
            sleep(1);

            if (i == 5) {
                kill(getppid(), SIGUSR1);
            }
        }

        exit(EXIT_SUCCESS);
    }

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
