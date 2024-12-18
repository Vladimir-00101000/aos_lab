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
#define MSG_TYPE_ALL 1

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
    int user_type;

    server_qid = msgget(SERVER_KEY, 0);
    if (server_qid == -1) {
        printf("Сервер не запущен\n");
        exit(1);
    }

    client_qid = create_queue(IPC_PRIVATE);

    printf("Введите ваш тип пользователя (1-5): ");
    scanf("%d", &user_type);
    getchar();

    printf("Клиент запущен. ID очереди: %d, тип: %d\n", client_qid, user_type);

    request.mtype = 1;
    request.client_queue_id = client_qid;
    request.sender_type = user_type;
    strcpy(request.text, "register");

    if (msgsnd(server_qid, &request, sizeof(request) - sizeof(long), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        while (1) {
            if (msgrcv(client_qid, &response, sizeof(response) - sizeof(long), user_type, 0) != -1) {
                printf("\n%s\n", response.text);
                printf("Кому отправить (0 - всем, 1-5 - конкретному типу): ");
                fflush(stdout);
            }
        }
    } else {
        while (1) {
            printf("Кому отправить (0 - всем, 1-5 - конкретному типу): ");
            scanf("%d", &request.recipient_type);
            getchar();

            printf("Введите сообщение (или 'exit' для выхода): ");
            fgets(buffer, MAX_TEXT, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';

            request.mtype = 1;
            request.sender_type = user_type;
            strncpy(request.text, buffer, MAX_TEXT);

            if (msgsnd(server_qid, &request, sizeof(request) - sizeof(long), 0) == -1) {
                perror("msgsnd");
                break;
            }

            if (strcmp(buffer, "exit") == 0) {
                break;
            }
        }

        kill(pid, SIGTERM);
    }

    request.mtype = 1;
    strcpy(request.text, "exit");
    msgsnd(server_qid, &request, sizeof(request) - sizeof(long), 0);

    if (msgctl(client_qid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    printf("Клиент завершил работу\n");
    return 0;
}
