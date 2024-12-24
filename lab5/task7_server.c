#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_LEN 128
#define PROJECT_ID 123
#define MAX_CLIENTS 10

typedef struct {
    long type;
    int sender_id;
    int receiver_id;
    char data[MSG_LEN];
} Message;

int main() {
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
    int active_clients[MAX_CLIENTS] = {0};
    int client_count = 0;

    while(1) {
        if (msgrcv(server_queue, &request, sizeof(Message) - sizeof(long), 0, 0) == -1) {
            perror("Ошибка получения запроса");
            break;
        }

        int client_found = 0;
        for (int i = 0; i < client_count; i++) {
            if (active_clients[i] == request.sender_id) {
                client_found = 1;
                break;
            }
        }

        if (!client_found && client_count < MAX_CLIENTS) {
            active_clients[client_count++] = request.sender_id;
            printf("Новый клиент подключился: %d\n", request.sender_id);
        }

        printf("\nПолучено сообщение типа %ld от клиента %d: %s\n", request.type, request.sender_id, request.data);

        response.type = request.type;
        response.sender_id = request.sender_id;
        strncpy(response.data, request.data, MSG_LEN - 1);
        response.data[MSG_LEN - 1] = '\0';

        if (request.receiver_id == 0) {
            for (int i = 0; i < client_count; i++) {
                if (active_clients[i] != request.sender_id) {
                    response.receiver_id = active_clients[i];
                    if (msgsnd(active_clients[i], &response, sizeof(Message) - sizeof(long), 0) == -1) {
                        perror("Ошибка отправки ответа");
                        for (int j = i; j < client_count - 1; j++) {
                            active_clients[j] = active_clients[j + 1];
                        }
                        client_count--;
                        i--;
                    }
                }
            }
        } else {
            response.receiver_id = request.receiver_id;
            if (msgsnd(request.receiver_id, &response, sizeof(Message) - sizeof(long), 0) == -1) {
                perror("Ошибка отправки ответа");
            }
        }

        printf("Сообщение обработано и отправлено\n");
    }

    msgctl(server_queue, IPC_RMID, NULL);
    printf("Сервер завершил работу\n");
    return 0;
}
