#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_LEN 32
#define PROJECT_ID 123
#define SERVER_TYPE 1

typedef struct {
    long type;
    int sender_id;
    char data[MSG_LEN];
} Message;

int main() {
    key_t key = ftok(".", PROJECT_ID);
    if (key == -1) {
        perror("Ошибка создания ключа");
        return 1;
    }

    int queue = msgget(key, IPC_CREAT | 0666);
    if (queue == -1) {
        perror("Ошибка создания очереди");
        return 1;
    }

    printf("Сервер запущен\n");
    printf("ID очереди: %d\n", queue);

    Message msg;

    while(1) {
        if (msgrcv(queue, &msg, sizeof(Message) - sizeof(long), SERVER_TYPE, 0) == -1) {
            perror("Ошибка получения запроса");
            break;
        }

        printf("\nПолучен запрос от клиента %d: %s\n", msg.sender_id, msg.data);

        msg.type = msg.sender_id;

        if (msgsnd(queue, &msg, sizeof(Message) - sizeof(long), 0) == -1) {
            perror("Ошибка отправки ответа");
            break;
        }

        printf("Отправлен ответ клиенту %ld: %s\n", msg.type, msg.data);
    }

    msgctl(queue, IPC_RMID, NULL);
    printf("Сервер завершил работу\n");
    return 0;
}
