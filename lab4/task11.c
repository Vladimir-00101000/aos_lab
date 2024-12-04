#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#define BUFFER_SIZE 256

void process_work(const char* process_name, int fd) {
    char buffer[BUFFER_SIZE];

    while (1) {
        if (flock(fd, LOCK_EX | LOCK_NB) == 0) {
            printf("%s (PID: %d): Получил доступ к терминалу\n",
                   process_name, getpid());

            printf("%s: Введите строку (или 'q' для выхода): ", process_name);
            fflush(stdout);

            if (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
                if (buffer[0] == 'q' && buffer[1] == '\n') {
                    flock(fd, LOCK_UN);
                    break;
                }
                printf("%s прочитал: %s", process_name, buffer);
            }

            flock(fd, LOCK_UN);
            printf("%s: Освободил терминал\n", process_name);
        } else if (errno == EWOULDBLOCK) {
            printf("%s: Ждёт освобождения терминала...\n", process_name);
        }

        sleep(2);
    }
}

int main() {
    pid_t pid;
    int terminal_fd;

    terminal_fd = open("/dev/tty", O_RDWR);
    if (terminal_fd == -1) {
        perror("Ошибка открытия терминала");
        exit(1);
    }

    pid = fork();

    if (pid < 0) {
        perror("Ошибка при создании процесса");
        close(terminal_fd);
        exit(1);
    }

    if (pid == 0) {
        process_work("Дочерний", terminal_fd);
    } else {
        process_work("Родительский", terminal_fd);
        wait(NULL);
    }

    close(terminal_fd);
    return 0;
}
