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
int client_queue;

void handle_sigint(int sig) {
    running = 0;
}

int main() {
    signal(SIGINT, handle_sigint);

    key_t server_key = ftok(".", PROJECT_ID);
    if (server_key == -1) {
        perror("Ошибка создания ключа сервера");
        return 1;
    }

    int server_queue = msgget(server_key, 0666);
    if (server_queue == -1) {
        perror("Ошибка получения очереди сервера");
        return 1;
    }

    client_queue = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if (client_queue == -1) {
        perror("Ошибка создания очереди клиента");
        return 1;
    }

    printf("ID очереди клиента: %d\n", client_queue);

    Message request, response;
    char input[MSG_LEN];

    while(running) {
        printf("\nВведите сообщение (или 'exit' для выхода): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0 || !running) {
            break;
        }

        request.type = client_queue;
        strncpy(request.data, input, MSG_LEN - 1);
        request.data[MSG_LEN - 1] = '\0';

        if (msgsnd(server_queue, &request, sizeof(request.data), 0) == -1) {
            perror("Ошибка отправки запроса");
            break;
        }

        printf("Отправлен запрос: %s\n", request.data);

        if (msgrcv(client_queue, &response, sizeof(response.data), 0, 0) == -1) {
            if (running) {
                perror("Ошибка получения ответа");
            }
            break;
        }

        printf("Получен ответ: %s\n", response.data);
    }

    msgctl(client_queue, IPC_RMID, NULL);
    printf("Клиент завершил работу\n");
    return 0;
}
