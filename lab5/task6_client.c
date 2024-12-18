#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_KEY 12345
#define MAX_TEXT 512

// Структура сообщения для запроса
struct request_msg {
    long mtype;             // Тип сообщения
    int client_queue_id;    // ID очереди клиента для ответа
    char text[MAX_TEXT];    // Текст запроса
};

// Структура сообщения для ответа
struct response_msg {
    long mtype;             // Тип сообщения
    char text[MAX_TEXT];    // Текст ответа
};

// Функция создания очереди сообщений
int create_queue(key_t key) {
    int qid = msgget(key, IPC_CREAT | 0666);
    if (qid == -1) {
        perror("msgget");
        exit(1);
    }
    return qid;
}

int main() {
    int server_qid, client_qid;
    struct request_msg request;
    struct response_msg response;
    char buffer[MAX_TEXT];

    server_qid = msgget(SERVER_KEY, 0);
    if (server_qid == -1) {
        printf("Сервер не запущен\n");
        exit(1);
    }

    client_qid = create_queue(IPC_PRIVATE);
    printf("Клиент запущен. ID очереди: %d\n", client_qid);

    request.mtype = 1;
    request.client_queue_id = client_qid;

    while (1) {
        printf("\nВведите сообщение (или 'exit' для выхода): ");
        fgets(buffer, MAX_TEXT, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        strncpy(request.text, buffer, MAX_TEXT);

        if (msgsnd(server_qid, &request, sizeof(request) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }

        if (msgrcv(client_qid, &response, sizeof(response) - sizeof(long), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("Ответ от сервера: %s\n", response.text);

        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }

    if (msgctl(client_qid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    printf("Клиент завершил работу\n");
    return 0;
}

// gcc -o server task6_server.c
// gcc -o client task6_client.c
