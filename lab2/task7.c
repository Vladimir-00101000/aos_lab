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

        pause();

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Child process exited with status: %d\n", WEXITSTATUS(status));
        } else {
            printf("Child process exited with an error\n");
        }

    } else {
        setpgrp();

        printf("Child process - PID: %d, Parent PID: %d, Process Group ID: %d\n", getpid(), getppid(), getpgrp());

        pause();

        exit(0);
    }

    return 0;
}
