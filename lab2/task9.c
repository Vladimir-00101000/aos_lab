#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_файла>\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Ошибка fork");
        return 1;
    }

    if (pid == 0) {
        int input_fd_child = open(argv[1], O_RDONLY);
        if (input_fd_child == -1) {
            perror("Ошибка открытия файла в дочернем процессе");
            return 1;
        }

        int output_fd_child = open("output_child.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (output_fd_child == -1) {
            perror("Ошибка открытия файла для записи дочернего процесса");
            close(input_fd_child);
            return 1;
        }

        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;

        while ((bytes_read = read(input_fd_child, buffer, BUFFER_SIZE)) > 0) {
            write(output_fd_child, buffer, bytes_read);
        }

        close(output_fd_child);
        close(input_fd_child);
        return 0;
    } else {
        int input_fd_parent = open(argv[1], O_RDONLY);
        if (input_fd_parent == -1) {
            perror("Ошибка открытия файла в родительском процессе");
            return 1;
        }

        int output_fd_parent = open("output_parent.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (output_fd_parent == -1) {
            perror("Ошибка открытия файла для записи родительского процесса");
            close(input_fd_parent);
            return 1;
        }

        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;

        while ((bytes_read = read(input_fd_parent, buffer, BUFFER_SIZE)) > 0) {
            write(output_fd_parent, buffer, bytes_read);
        }

        close(output_fd_parent);
        close(input_fd_parent);

        wait(NULL);

        printf("Содержимое файла дочернего процесса:\n");

        int output_child_fd = open("output_child.txt", O_RDONLY);
        while ((bytes_read = read(output_child_fd, buffer, BUFFER_SIZE)) > 0) {
            write(STDOUT_FILENO, buffer, bytes_read);
        }

        close(output_child_fd);

        printf("\nСодержимое файла родительского процесса:\n");

        int output_parent_fd = open("output_parent.txt", O_RDONLY);
        while ((bytes_read = read(output_parent_fd, buffer, BUFFER_SIZE)) > 0) {
            write(STDOUT_FILENO, buffer, bytes_read);
        }

        close(output_parent_fd);
    }

    return 0;
}
