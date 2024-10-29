#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

void copy_file(int src_fd, int dest_fd) {
    char buffer[1024];
    ssize_t bytes;

    while ((bytes = read(src_fd, buffer, sizeof(buffer))) > 0) {
        write(dest_fd, buffer, bytes);
    }
}

int main(int argc, char *argv[]) {
    int src_fd, dest_fd;

    if (argc == 3) {
        src_fd = open(argv[1], O_RDONLY);
        if (src_fd == -1) {
            perror("Error opening source file");
            return 1;
        }

        dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (dest_fd == -1) {
            perror("Error opening destination file");
            close(src_fd);
            return 1;
        }

        copy_file(src_fd, dest_fd);

        close(src_fd);
        close(dest_fd);
        printf("File copied from %s to %s\n", argv[1], argv[2]);
    } else {
        printf("Copying from standard input to standard output. Press Ctrl+D (EOF) to finish.\n");
        copy_file(STDIN_FILENO, STDOUT_FILENO);
    }

    return 0;
}
