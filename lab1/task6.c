#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <filename> <permissions>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, atoi(argv[2]));
    if (fd == -1) {
        perror("Unable to create file");
        return 1;
    }
    printf("File created successfully with descriptor: %d\n", fd);

    const char *lines[] = {
        "First line\n",
        "Second line\n",
        "Third line\n"
    };

    for (int i = 0; i < 3; i++) {
        off_t position = i * 100;
        lseek(fd, position, SEEK_SET);

        int bytes_writer = write(fd, lines[i], strlen(lines[i]));
        if (bytes_writer != strlen(lines[i])) {
            perror("Error writing to file");
        }
    }

    close(fd);
    printf("File closed after writing.\n");

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Unable to open file for reading");
        return 1;
    }
    printf("File opened for reading with descriptor: %d\n", fd);

    char buffer[256];
    int bytes_read;
    lseek(fd, 0, SEEK_SET);
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, bytes_read);
    }

    close(fd);
    printf("File closed after reading.\n");

    return 0;
}
