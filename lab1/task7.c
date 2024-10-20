#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

void print_file_info(const char *filename, struct stat *file_stat) {
    printf("File: %s\n", filename);
    printf("File size: %lld bytes\n", (long long)file_stat->st_size);  // Используем %lld
    printf("Block size: %d bytes\n", (int)file_stat->st_blksize);     // Используем %d
    printf("Number of blocks allocated: %lld\n", (long long)file_stat->st_blocks);  // Используем %lld

    printf("File type: ");
    if (S_ISREG(file_stat->st_mode)) {
        printf("Regular file\n");
    } else if (S_ISDIR(file_stat->st_mode)) {
        printf("Directory\n");
    } else if (S_ISCHR(file_stat->st_mode)) {
        printf("Character device\n");
    } else if (S_ISBLK(file_stat->st_mode)) {
        printf("Block device\n");
    } else if (S_ISFIFO(file_stat->st_mode)) {
        printf("FIFO (named pipe)\n");
    } else if (S_ISLNK(file_stat->st_mode)) {
        printf("Symbolic link\n");
    } else if (S_ISSOCK(file_stat->st_mode)) {
        printf("Socket\n");
    } else {
        printf("Unknown type\n");
    }

    printf("Last access time: %s", ctime(&file_stat->st_atime));
    printf("Last modification time: %s", ctime(&file_stat->st_mtime));
    printf("Last status change time: %s", ctime(&file_stat->st_ctime));
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    struct stat file_stat;

    // Получение информации о файле
    if (stat(argv[1], &file_stat) == -1) {
        perror("Error retrieving file information");
        return 1;
    }

    // Печать информации о файле
    print_file_info(argv[1], &file_stat);

    return 0;
}
