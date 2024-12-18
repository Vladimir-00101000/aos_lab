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

int client_qid = -1;

void client_signal_handler(int signo) {
    if (signo == SIGINT) {
        printf("\nПолучен сигнал SIGINT. Завершение работы клиента...\n");

        if (client_qid != -1) {
            if (msgctl(client_qid, IPC_RMID, NULL) == -1) {
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
    int server_qid;
    struct request_msg request;
    struct response_msg response;
    char buffer[MAX_TEXT];

    struct sigaction sa;
    sa.sa_handler = client_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

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
        if (fgets(buffer, MAX_TEXT, stdin) == NULL) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }

        buffer[strcspn(buffer, "\n")] = '\0';
        strncpy(request.text, buffer, MAX_TEXT);

        if (msgsnd(server_qid, &request, sizeof(request) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            break;
        }

        if (msgrcv(client_qid, &response, sizeof(response) - sizeof(long), 0, 0) == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("msgrcv");
            break;
        }

        printf("Ответ от сервера: %s\n", response.text);

        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }

    if (msgctl(client_qid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
    }

    printf("Клиент завершил работу\n");
    return 0;
}
