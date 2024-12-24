#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

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

    int request_queue = msgget(request_key, IPC_CREAT | 0666);
    if (request_queue == -1) {
        perror("Ошибка создания очереди запросов");
        return 1;
    }

    key_t response_key = ftok(".", PROJECT_ID_RESP);
    if (response_key == -1) {
        perror("Ошибка создания ключа очереди ответов");
        return 1;
    }

    int response_queue = msgget(response_key, IPC_CREAT | 0666);
    if (response_queue == -1) {
        perror("Ошибка создания очереди ответов");
        return 1;
    }

    printf("Сервер запущен\n");
    printf("ID очереди запросов: %d\n", request_queue);
    printf("ID очереди ответов: %d\n", response_queue);

    Message request, response;

    while(1) {
        if (msgrcv(request_queue, &request, sizeof(request.data), 0, 0) == -1) {
            perror("Ошибка получения запроса");
            break;
        }

        printf("\nПолучен запрос от клиента %ld: %s\n", request.type, request.data);

        response.type = request.type;
        strncpy(response.data, request.data, MSG_LEN - 1);
        response.data[MSG_LEN - 1] = '\0';

        if (msgsnd(response_queue, &response, sizeof(response.data), 0) == -1) {
            perror("Ошибка отправки ответа");
            break;
        }

        printf("Отправлен ответ клиенту %ld: %s\n", response.type, response.data);
    }

    msgctl(request_queue, IPC_RMID, NULL);
    msgctl(response_queue, IPC_RMID, NULL);
    printf("Сервер завершил работу\n");
    return 0;
}
