#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main() {
    // Пытаемся открыть несуществующий файл
    FILE *file = fopen("nonexistentfile.txt", "r");

    if (file == NULL) {
        // Вывод значения errno
        printf("Value of errno: %d\n", errno);

        // Вывод сообщения об ошибке из массива sys_errlist[]
        printf("Error message using sys_errlist[]: %s\n", sys_errlist[errno]);

        // Использование perror для вывода сообщения об ошибке
        perror("Error message using perror");
    }

    return 0;
}
