#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <id очереди>\n", argv[0]);
        return 1;
    }

    int msqid = atoi(argv[1]);
    if (msqid <= 0) {
        fprintf(stderr, "Некорректный ID очереди сообщений\n");
        return 1;
    }

    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("Ошибка при удалении очереди");
        return 1;
    }

    printf("Очередь сообщений %d успешно удалена\n", msqid);
    return 0;
}
