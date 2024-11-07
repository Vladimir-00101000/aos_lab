#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <команда>\n", argv[0]);
        return 1;
    }

    int result = system(argv[1]);

    if (result == -1) {
        perror("Ошибка при выполнении команды");
        return 1;
    }

    return 0;
}

//Запуск: ./task10 "ls"
