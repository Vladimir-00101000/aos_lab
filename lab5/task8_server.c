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

struct request_msg {
    long mtype;
    int client_queue_id;
    char text[MAX_TEXT];
};

struct response_msg {
    long mtype;
    char text[MAX_TEXT];
};

int server_qid = -1;

void server_signal_handler(int signo) {
    if (signo == SIGINT) {
        printf("\nПолучен сигнал SIGINT. Завершение работы сервера...\n");

        if (server_qid != -1) {
            if (msgctl(server_qid, IPC_RMID, NULL) == -1) {
                perror("msgctl");
            }
        }

        exit(0);
    }
}

int create_queue(key_t key) {
    int qid = msgget(key, IPC_CREAT | 0666);
    if (qid == -1) {
        perror("msgget");
        exit(1);
    }
    return qid;
}

int main() {
    struct request_msg request;
    struct response_msg response;

    struct sigaction sa;
    sa.sa_handler = server_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    server_qid = create_queue(SERVER_KEY);
    printf("Сервер запущен. ID очереди: %d\n", server_qid);

    while (1) {
        if (msgrcv(server_qid, &request, sizeof(request) - sizeof(long), 0, 0) == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("msgrcv");
            break;
        }

        printf("\nПолучен запрос от клиента (очередь: %d): %s\n",
               request.client_queue_id, request.text);

        response.mtype = 1;
        sprintf(response.text, "Сервер получил сообщение: %s", request.text);

        if (msgsnd(request.client_queue_id, &response, sizeof(response) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            break;
        }

        if (strcmp(request.text, "exit") == 0) {
            break;
        }
    }

    if (msgctl(server_qid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
    }

    printf("Сервер завершил работу\n");
    return 0;
}
