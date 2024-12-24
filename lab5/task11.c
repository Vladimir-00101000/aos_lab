#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <signal.h>

struct message {
    long type;
    char dummy;
};

static int queue_id;

void cleanup(int sig) {
    msgctl(queue_id, IPC_RMID, NULL);
    exit(0);
}

int main() {
    queue_id = msgget(IPC_PRIVATE, 0600);
    if (queue_id == -1) {
        perror("Ошиибка получения id очереди");
        exit(1);
    }

    signal(SIGINT, cleanup);

    struct message msg = {0};
    pid_t pid = fork();

    if (pid > 0) {
        while (1) {
            printf("Родитель %d отправляет\n", getpid());
            msg.type = 1;
            msgsnd(queue_id, &msg, sizeof(msg.dummy), 0);

            msgrcv(queue_id, &msg, sizeof(msg.dummy), 2, 0);
            sleep(1);
        }
    }
    else if (pid == 0) {
        while (1) {
            msgrcv(queue_id, &msg, sizeof(msg.dummy), 1, 0);
            printf("Ребенок %d отвечает\n", getpid());

            msg.type = 2;
            msgsnd(queue_id, &msg, sizeof(msg.dummy), 0);
            sleep(1);
        }
    }
    else {
        perror("Ошибка fork");
        cleanup(0);
    }

    return 0;
}
