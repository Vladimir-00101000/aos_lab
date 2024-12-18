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

int main() {
    int request_qid, response_qid;
    struct request_msg request;
    struct response_msg response;
    char buffer[MAX_TEXT];
    pid_t my_pid = getpid();

    request_qid = msgget(REQUEST_QUEUE_KEY, 0);
    response_qid = msgget(RESPONSE_QUEUE_KEY, 0);

    if (request_qid == -1 || response_qid == -1) {
        printf("Сервер не запущен\n");
        exit(1);
    }

    printf("Клиент запущен. PID: %d\n", my_pid);

    request.mtype = 1;
    request.client_pid = my_pid;

    while (1) {
        printf("\nВведите сообщение (или 'exit' для выхода): ");
        fgets(buffer, MAX_TEXT, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        strncpy(request.text, buffer, MAX_TEXT);

        if (msgsnd(request_qid, &request, sizeof(request) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }

        if (msgrcv(response_qid, &response, sizeof(response) - sizeof(long), my_pid, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("Ответ от сервера: %s\n", response.text);

        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }

    printf("Клиент завершил работу\n");
    return 0;
}
