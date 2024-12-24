#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_LEN 32
#define PROJECT_ID 123

typedef struct {
    long type;
    char data[MSG_LEN];
} Message;

volatile sig_atomic_t running = 1;
int server_queue;

void handle_sigint(int sig) {
    running = 0;
}

void process_message(char *message) {
    for (int i = 0; message[i]; i++) {
        message[i]--;
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, handle_sigint);

    key_t server_key = ftok(".", PROJECT_ID);
    if (server_key == -1) {
        perror("Ошибка создания ключа");
        return 1;
    }

    server_queue = msgget(server_key, IPC_CREAT | 0666);
    if (server_queue == -1) {
        perror("Ошибка создания очереди");
        return 1;
    }

    printf("Сервер запущен. ID очереди: %d\n", server_queue);

    Message request, response;

    while(running) {
        if (msgrcv(server_queue, &request, sizeof(request.data), 0, 0) == -1) {
            if (running) {
                perror("Ошибка получения запроса");
            }
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
    printf("Сервер завершил работу\n");
    return 0;
}
