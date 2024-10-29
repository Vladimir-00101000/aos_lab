#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void print_file_reverse(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("Error getting file size");
        close(fd);
        exit(1);
    }

    for (off_t pos = file_size - 1; pos >= 0; pos--) {
        if (lseek(fd, pos, SEEK_SET) == -1) {
            perror("Error seeking in file");
            close(fd);
            exit(1);
        }

        char c;
        if (read(fd, &c, 1) != 1) {
            perror("Error reading from file");
            close(fd);
            exit(1);
        }

        write(STDOUT_FILENO, &c, 1);
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    print_file_reverse(argv[1]);
    return 0;
}
