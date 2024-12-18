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

int main() {
    int qid;
    struct message msg;
    char buffer[MAX_TEXT];

    qid = msgget(SERVER_KEY, 0);
    if (qid == -1) {
        printf("Сервер не запущен\n");
        exit(1);
    }

    pid_t my_pid = getpid();
    printf("Клиент запущен. PID: %d\n", my_pid);

    while (1) {
        printf("\nВведите сообщение (или 'exit' для выхода): ");
        fgets(buffer, MAX_TEXT, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        msg.mtype = REQUEST_TYPE;
        msg.sender_pid = my_pid;
        strncpy(msg.text, buffer, MAX_TEXT);

        if (msgsnd(qid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }

        struct message response;
        if (msgrcv(qid, &response, sizeof(response) - sizeof(long), my_pid, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("Ответ от сервера (PID: %d): %s\n",
               response.sender_pid, response.text);

        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }

    printf("Клиент завершил работу\n");
    return 0;
}
