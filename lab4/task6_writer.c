#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    char buf[100];
    int fd;

    mkfifo("/tmp/myfifo", 0666);

    fd = open("/tmp/myfifo", O_WRONLY);
    // fd = open("/tmp/myfifo", O_WRONLY | O_NDELAY);
    printf("Канал открыт для записи\n");

    while(1) {
        printf("Введите сообщение: ");
        scanf("%s", buf);
        write(fd, buf, sizeof(buf));
        printf("Сообщение отправлено\n");
    }

    close(fd);
    return 0;
}
