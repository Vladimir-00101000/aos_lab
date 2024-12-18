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
#define REQUEST_TYPE 1

struct message {
    long mtype;
    pid_t sender_pid;
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
    int qid;
    struct message msg;

    qid = create_queue(SERVER_KEY);
    printf("Сервер запущен. ID очереди: %d\n", qid);

    while (1) {
        if (msgrcv(qid, &msg, sizeof(msg) - sizeof(long), REQUEST_TYPE, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("\nПолучен запрос от клиента (PID: %d): %s\n",
               msg.sender_pid, msg.text);

        struct message response;
        response.mtype = msg.sender_pid;
        sprintf(response.text, "Сервер получил сообщение: %s", msg.text);
        response.sender_pid = getpid();

        if (msgsnd(qid, &response, sizeof(response) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }

        if (strcmp(msg.text, "exit") == 0) {
            break;
        }
    }

    if (msgctl(qid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    printf("Сервер завершил работу\n");
    return 0;
}
