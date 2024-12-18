#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define REQUEST_QUEUE_KEY 12345
#define RESPONSE_QUEUE_KEY 12346
#define MAX_TEXT 512

struct request_msg {
    long mtype;
    pid_t client_pid;
    char text[MAX_TEXT];
};

struct response_msg {
    long mtype;
    char text[MAX_TEXT];
};

int create_queue(key_t key) {
    int qid = msgget(key, IPC_CREAT | 0666);
    if (qid == -1) {
        perror("msgget");
        exit(1);
    }
    return qid;
}

int main() {
    int request_qid, response_qid;
    struct request_msg request;
    struct response_msg response;

    request_qid = create_queue(REQUEST_QUEUE_KEY);
    response_qid = create_queue(RESPONSE_QUEUE_KEY);

    printf("Сервер запущен.\nОчередь запросов: %d\nОчередь ответов: %d\n",
           request_qid, response_qid);

    while (1) {
        if (msgrcv(request_qid, &request, sizeof(request) - sizeof(long), 1, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("\nПолучен запрос от клиента (PID: %d): %s\n",
               request.client_pid, request.text);

        response.mtype = request.client_pid;
        sprintf(response.text, "Сервер получил сообщение: %s", request.text);

        if (msgsnd(response_qid, &response, sizeof(response) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }

        if (strcmp(request.text, "exit") == 0) {
            break;
        }
    }

    msgctl(request_qid, IPC_RMID, NULL);
    msgctl(response_qid, IPC_RMID, NULL);

    printf("Сервер завершил работу\n");
    return 0;
}
