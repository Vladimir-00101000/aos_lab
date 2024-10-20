#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>


int main() {
    int fd = open("example.txt", O_RDONLY); // Открываем несуществующий файл для чтения

    if(fd == -1) {
        printf("Ошибка выполнения системного вызова 'open':\n");
        printf("Value of errno: %d\n", errno);
        printf("Message from sys_errlist: %s\n", sys_errlist[errno]);
        perror("Error message via perror(): ");
    } else {
        close(fd);
        printf("Системный вызов 'open' выполнен успешно.\n");
    }

    return 0;
}
