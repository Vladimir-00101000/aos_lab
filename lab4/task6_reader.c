#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    char buf[100];
    int fd;

    fd = open("/tmp/myfifo", O_RDONLY);
    // fd = open("/tmp/myfifo", O_RDONLY | O_NDELAY);
    printf("Канал открыт для чтения\n");

    while(1) {
        read(fd, buf, sizeof(buf));
        printf("Получено: %s\n", buf);
    }

    close(fd);
    return 0;
}
