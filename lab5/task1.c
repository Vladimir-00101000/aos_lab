#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define MSG_LEN 32
#define MSG_TYPES_COUNT 3
#define PROJECT_ID 123

typedef struct {
    long type;
    char data[MSG_LEN];
} Message;

void print_queue_stats(int msqid) {
    struct msqid_ds stats;

    if (msgctl(msqid, IPC_STAT, &stats) == -1) {
        perror("msgctl failed");
        return;
    }

    printf("\nОчередь сообщений (ID: %d)\n", msqid);
    printf("Количество сообщений: %lu\n", stats.msg_qnum);
    printf("Максимальный размер очереди: %lu\n", stats.msg_qbytes);
    printf("PID последней записи: %d\n", stats.msg_lspid);
    printf("PID последнего чтения: %d\n", stats.msg_lrpid);
    printf("Время последней записи: %ld\n", stats.msg_stime);
    printf("Время последнего чтения: %ld\n", stats.msg_rtime);
}

int main(int argc, char *argv[]) {

    key_t key = ftok(".", PROJECT_ID);
    if (key == -1) {
        perror("ftok failed");
        return 1;
    }

    int msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1) {
        perror("msgget failed");
        return 1;
    }

    Message msg;
    for (long i = 1; i <= MSG_TYPES_COUNT; i++) {
        msg.type = i;
        snprintf(msg.data, MSG_LEN, "Сообщение типа %ld", i);

        if (msgsnd(msqid, &msg, sizeof(msg.data), 0) == -1) {
            perror("msgsnd failed");
            return 1;
        }
        printf("Отправлено: %s\n", msg.data);
    }

    print_queue_stats(msqid);
    return 0;
}
