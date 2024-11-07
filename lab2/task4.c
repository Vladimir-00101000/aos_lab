#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid > 0) {
        printf("Parent process - PID: %d, Parent PID: %d, Process Group ID: %d\n", getpid(), getppid(), getpgrp());
        exit(0);

    } else {
        sleep(2);
        printf("Child process - PID: %d, Parent PID: %d, Process Group ID: %d\n", getpid(), getppid(), getpgrp());

        exit(0);
    }

    return 0;
}
