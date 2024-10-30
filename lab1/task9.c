#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

void copy_file(int src_fd, int dest_fd) {
    char buffer[1024];
    int bytes;

    while ((bytes = read(src_fd, buffer, sizeof(buffer))) > 0) {
        write(dest_fd, buffer, bytes);
    }
}

int main(int argc, char *argv[]) {
    int src_fd, dest_fd;
    char buf1[100], buf2[100];

    if (argc == 3) {
        src_fd = open(argv[1], O_RDONLY);
        if (src_fd == -1) {
            perror("Error opening source file");
            return 1;
        }

        dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 400);
        if (dest_fd == -1) {
            perror("Error opening destination file");
            close(src_fd);
            return 1;
        }

        dup2(src_fd, STDIN_FILENO);
        dup2(dest_fd, STDOUT_FILENO);

        copy_file(STDIN_FILENO, STDOUT_FILENO);

        close(src_fd);
        close(dest_fd);

        printf("File copied from %s to %s\n", argv[1], argv[2]);
    } else {
        read(STDIN_FILENO, buf1, 100);
        buf1[strcspn(buf1, "\n")] = 0;
        read(STDIN_FILENO, buf2, 100);
        buf2[strcspn(buf2, "\n")] = 0;

        src_fd = open(buf1, O_RDONLY);
        if (src_fd == -1) {
            perror("Error opening source file");
            return 1;
        }

        dest_fd = open(buf2, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (dest_fd == -1) {
            perror("Error opening destination file");
            close(src_fd);
            return 1;
        }

        dup2(src_fd, STDIN_FILENO);
        dup2(dest_fd, STDOUT_FILENO);

        copy_file(STDIN_FILENO, STDOUT_FILENO);

        close(src_fd);
        close(dest_fd);

        printf("File copied from %s to %s\n", buf1, buf2);
    }

    return 0;
}
