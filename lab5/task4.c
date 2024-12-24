#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define MSG_LEN 32

typedef struct {
    long type;
    char data[1024];
} SendMessage;

typedef struct {
    long type;
    char data[MSG_LEN];
} ReceiveMessage;

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

    SendMessage test_msg = {0};
    test_msg.type = msg_type;
    strcpy(test_msg.data, "Это очень длинное тестовое сообщение, которое не поместится в буфер размером 32 байта");

    if (msgsnd(msqid, &test_msg, strlen(test_msg.data) + 1, 0) == -1) {
        perror("Ошибка при отправке тестового сообщения");
        return 1;
    }
    printf("Отправлено длинное сообщение: %s\n\n", test_msg.data);

    ReceiveMessage msg = {0};
    if (msgrcv(msqid, &msg, sizeof(msg.data), msg_type, IPC_NOWAIT | MSG_NOERROR) == -1) {
        if (errno == ENOMSG) {
            printf("В очереди нет сообщений типа %ld\n", msg_type);
        } else {
            perror("Ошибка при получении сообщения");
        }
        return 1;
    }

    printf("Получено сообщение (%ld байт): %s\n", strlen(msg.data), msg.data);
    return 0;
}
