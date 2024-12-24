#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

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

    int queue = msgget(key, 0666);
    if (queue == -1) {
        perror("Ошибка получения очереди");
        return 1;
    }

    int client_id = (int)time(NULL) % 1000 + 100;
    printf("ID клиента: %d\n", client_id);

    Message msg;
    char input[MSG_LEN];

    while(1) {
        printf("\nВведите сообщение (или 'exit' для выхода): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            break;
        }

        msg.type = SERVER_TYPE;
        msg.sender_id = client_id;
        strncpy(msg.data, input, MSG_LEN - 1);
        msg.data[MSG_LEN - 1] = '\0';

        if (msgsnd(queue, &msg, sizeof(Message) - sizeof(long), 0) == -1) {
            perror("Ошибка отправки запроса");
            break;
        }

        printf("Отправлен запрос: %s\n", msg.data);

        if (msgrcv(queue, &msg, sizeof(Message) - sizeof(long), client_id, 0) == -1) {
            perror("Ошибка получения ответа");
            break;
        }

        printf("Получен ответ от сервера: %s\n", msg.data);
    }

    printf("Клиент завершил работу\n");
    return 0;
}
