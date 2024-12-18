#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#define SERVER_KEY 12345
#define MAX_TEXT 512
#define MAX_CLIENTS 10
#define MSG_TYPE_ALL 1

struct client_info {
    int queue_id;
    int type;
    int active;
};

struct request_msg {
    long mtype;
    int client_queue_id;
    int recipient_type;
    int sender_type;
    char text[MAX_TEXT];
};

struct response_msg {
    long mtype;
    char text[MAX_TEXT];
};

int server_qid;
struct client_info clients[MAX_CLIENTS];
int client_count = 0;

void cleanup(int signum) {
    printf("\nПолучен сигнал прерывания. Удаление очереди сообщений...\n");
    if (msgctl(server_qid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
    }
    exit(0);
}

int create_queue(key_t key) {
    int qid = msgget(key, IPC_CREAT | 0666);
    if (qid == -1) {
        perror("msgget");
        exit(1);
    }
    return qid;
}

void register_client(int queue_id, int type) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].queue_id = queue_id;
            clients[i].type = type;
            clients[i].active = 1;
            client_count++;
            printf("Зарегистрирован новый клиент: очередь %d, тип %d\n", queue_id, type);
            return;
        }
    }
}

void remove_client(int queue_id) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].queue_id == queue_id) {
            clients[i].active = 0;
            client_count--;
            printf("Клиент удален: очередь %d\n", queue_id);
            return;
        }
    }
}

int main() {
    struct request_msg request;
    struct response_msg response;

    signal(SIGINT, cleanup);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].active = 0;
    }

    server_qid = create_queue(SERVER_KEY);
    printf("Сервер запущен. ID очереди: %d\n", server_qid);

    while (1) {
        if (msgrcv(server_qid, &request, sizeof(request) - sizeof(long), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        if (strcmp(request.text, "register") == 0) {
            register_client(request.client_queue_id, request.sender_type);
            continue;
        }

        if (strcmp(request.text, "exit") == 0) {
            remove_client(request.client_queue_id);
            continue;
        }

        printf("\nПолучено сообщение от клиента %d (тип %d): %s\n",
               request.client_queue_id, request.sender_type, request.text);

        sprintf(response.text, "[От пользователя типа %d]: %s",
                request.sender_type, request.text);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active) {
                if (request.recipient_type == 0 || request.recipient_type == clients[i].type) {
                    response.mtype = clients[i].type;
                    if (msgsnd(clients[i].queue_id, &response, sizeof(response) - sizeof(long), 0) == -1) {
                        perror("msgsnd");
                        remove_client(clients[i].queue_id);
                    } else {
                        printf("Сообщение отправлено клиенту: очередь %d, тип %d\n",
                               clients[i].queue_id, clients[i].type);
                    }
                }
            }
        }
    }

    return 0;
}
