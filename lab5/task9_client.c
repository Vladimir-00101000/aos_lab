#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/msg.h>

#define MSG_LEN 32
#define PROJECT_ID_REQ 123
#define PROJECT_ID_RESP 124

typedef struct {
    long type;
    char data[MSG_LEN];
} Message;

int main() {

    key_t request_key = ftok(".", PROJECT_ID_REQ);
    if (request_key == -1) {
        perror("Ошибка создания ключа очереди запросов");
        return 1;
    }

    int request_queue = msgget(request_key, 0666);
    if (request_queue == -1) {
        perror("Ошибка получения очереди запросов");
        return 1;
    }

    key_t response_key = ftok(".", PROJECT_ID_RESP);
    if (response_key == -1) {
        perror("Ошибка создания ключа очереди ответов");
        return 1;
    }

    int response_queue = msgget(response_key, 0666);
    if (response_queue == -1) {
        perror("Ошибка получения очереди ответов");
        return 1;
    }

    int client_id = (int)time(NULL) % 1000 + 1;
    printf("ID клиента: %d\n", client_id);

    Message request, response;
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

        request.type = client_id;
        strncpy(request.data, input, MSG_LEN - 1);
        request.data[MSG_LEN - 1] = '\0';

        if (msgsnd(request_queue, &request, sizeof(request.data), 0) == -1) {
            perror("Ошибка отправки запроса");
            break;
        }

        printf("Отправлен запрос: %s\n", request.data);

        if (msgrcv(response_queue, &response, sizeof(response.data), client_id, 0) == -1) {
            perror("Ошибка получения ответа");
            break;
        }

        printf("Получен ответ: %s\n", response.data);
    }

    printf("Клиент завершил работу\n");
    return 0;
}
