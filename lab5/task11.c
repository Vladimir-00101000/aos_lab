#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#define MSG_SIZE 100

struct message {
    long type;
    char text[MSG_SIZE];
};

static int queue_id;

void cleanup(int sig) {
    msgctl(queue_id, IPC_RMID, NULL);
    exit(0);
}

int main() {
    queue_id = msgget(IPC_PRIVATE, 0600);
    if (queue_id == -1) {
        perror("Ошибка получения id очереди");
        exit(1);
    }

    signal(SIGINT, cleanup);

    struct message msg;
    pid_t pid = fork();

    if (pid > 0) {
        int counter = 1;
        while (1) {
            msg.type = 1;
            sprintf(msg.text, "Сообщение №%d от родителя", counter);
            printf("Родитель отправляет: %s\n", msg.text);
            msgsnd(queue_id, &msg, sizeof(msg.text), 0);

            msgrcv(queue_id, &msg, sizeof(msg.text), 2, 0);
            printf("Родитель получает: %s\n", msg.text);

            counter++;
            sleep(1);
        }
    }
    else if (pid == 0) {
        int counter = 1;
        while (1) {
            msgrcv(queue_id, &msg, sizeof(msg.text), 1, 0);
            printf("Ребенок получает: %s\n", msg.text);

            msg.type = 2;
            sprintf(msg.text, "Сообщение №%d от ребенка", counter);
            printf("Ребенок отправляет: %s\n", msg.text);
            msgsnd(queue_id, &msg, sizeof(msg.text), 0);

            counter++;
            sleep(1);
        }
    }
    else {
        perror("Ошибка fork");
        cleanup(0);
    }

    return 0;
}
