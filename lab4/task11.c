#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE 256

int set_lock(int fd, int type) {
    struct flock lock;

    lock.l_type = type;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;

    return fcntl(fd, F_SETLK, &lock);
}

int main() {
    pid_t pid;
    char buffer[BUFFER_SIZE];
    int stdin_fd = fileno(stdin);
    int stdout_fd = fileno(stdout);

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Ошибка при создании процесса\n");
        exit(1);
    }

    if (pid == 0) {
        while (1) {
            if (set_lock(stdin_fd, F_WRLCK) == -1) {
                sleep(1);
                continue;
            }

            if (set_lock(stdout_fd, F_WRLCK) == -1) {
                set_lock(stdin_fd, F_UNLCK);
                sleep(1);
                continue;
            }

            printf("Процесс-потомок (PID: %d) ожидает ввод: ", getpid());
            fflush(stdout);

            if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
                set_lock(stdout_fd, F_UNLCK);
                set_lock(stdin_fd, F_UNLCK);
                break;
            }

            printf("Процесс-потомок получил: %s", buffer);
            fflush(stdout);

            set_lock(stdout_fd, F_UNLCK);
            set_lock(stdin_fd, F_UNLCK);

            sleep(1);
        }
    } else {
        while (1) {
            if (set_lock(stdin_fd, F_WRLCK) == -1) {
                sleep(1);
                continue;
            }

            if (set_lock(stdout_fd, F_WRLCK) == -1) {
                set_lock(stdin_fd, F_UNLCK);
                sleep(1);
                continue;
            }

            printf("Родительский процесс (PID: %d) ожидает ввод: ", getpid());
            fflush(stdout);

            if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
                set_lock(stdout_fd, F_UNLCK);
                set_lock(stdin_fd, F_UNLCK);
                break;
            }

            printf("Родительский процесс получил: %s", buffer);
            fflush(stdout);

            set_lock(stdout_fd, F_UNLCK);
            set_lock(stdin_fd, F_UNLCK);

            sleep(1);
        }

        wait(NULL);
    }

    return 0;
}
