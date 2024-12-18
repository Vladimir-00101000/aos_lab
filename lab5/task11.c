#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define MSG_KEY 12345

struct message {
    long mtype;
    int number;
};

void producer(int qid) {
    struct message msg;
    msg.mtype = 1;

    for (int i = 1; i <= 5; i++) {
        msg.number = i;

        msgsnd(qid, &msg, sizeof(struct message) - sizeof(long), 0);
        printf("Производитель: отправлено число %d\n", i);

        msgrcv(qid, &msg, sizeof(struct message) - sizeof(long), 2, 0);
        printf("Производитель: получено подтверждение для числа %d\n", i);

        sleep(1);
    }
}

void consumer(int qid) {
    struct message msg;
    struct message ack;
    ack.mtype = 2;
    ack.number = 0;

    for (int i = 1; i <= 5; i++) {
        msgrcv(qid, &msg, sizeof(struct message) - sizeof(long), 1, 0);
        printf("Потребитель: получено число %d\n", msg.number);

        msgsnd(qid, &ack, sizeof(struct message) - sizeof(long), 0);
        printf("Потребитель: отправлено подтверждение\n");

        sleep(1);
    }
}

int main() {
    int qid = msgget(MSG_KEY, IPC_CREAT | 0666);

    if (fork() == 0) {
        consumer(qid);
        exit(0);
    }

    producer(qid);
    wait(NULL);

    msgctl(qid, IPC_RMID, NULL);
    return 0;
}
