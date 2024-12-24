#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_LEN 32
#define PROJECT_ID 123

typedef struct {
    long type;
    char data[MSG_LEN];
} Message;

void process_message(char *message) {
    for (int i = 0; message[i]; i++) {
        message[i]--;
    }
}

int main(int argc, char *argv[]) {

    key_t server_key = ftok(".", PROJECT_ID);
    if (server_key == -1) {
        perror("Ошибка создания ключа");
        return 1;
    }

    int server_queue = msgget(server_key, IPC_CREAT | 0666);
    if (server_queue == -1) {
        perror("Ошибка создания очереди");
        return 1;
    }

    printf("Сервер запущен. ID очереди: %d\n", server_queue);

    Message request, response;

    while(1) {
        if (msgrcv(server_queue, &request, sizeof(request.data), 0, 0) == -1) {
            perror("Ошибка получения запроса");
            break;
        }

        printf("\nПолучен запрос от клиента %ld: %s\n", request.type, request.data);

        response.type = 1;
        strncpy(response.data, request.data, MSG_LEN - 1);
        response.data[MSG_LEN - 1] = '\0';

        process_message(response.data);

        if (msgsnd(request.type, &response, sizeof(response.data), 0) == -1) {
            perror("Ошибка отправки ответа");
            break;
        }

        printf("Отправлен ответ: %s\n", response.data);
    }

    msgctl(server_queue, IPC_RMID, NULL);
    return 0;
}
