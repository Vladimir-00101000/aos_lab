#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define MSG_LEN 32

typedef struct {
    long type;
    char data[MSG_LEN];
} Message;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <id очереди> <тип сообщения>\n", argv[0]);
        return 1;
    }

    int msqid = atoi(argv[1]);
    if (msqid <= 0) {
        fprintf(stderr, "Некорректный ID очереди сообщений\n");
        return 1;
    }

    long msg_type = atol(argv[2]);
    if (msg_type <= 0) {
        fprintf(stderr, "Некорректный тип сообщения\n");
        return 1;
    }

    Message msg = {0};
    if (msgrcv(msqid, &msg, sizeof(msg.data), msg_type, 0) == -1) {
        perror("Ошибка при получении сообщения");
        return 1;
    }

    printf("Получено сообщение: %s\n", msg.data);

    return 0;
}
