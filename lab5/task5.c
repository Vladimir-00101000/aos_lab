#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define PROJECT_ID 123

const char* getErrorDescription(int error) {
    switch(error) {
        case EACCES:
            return "Отказано в доступе к очереди сообщений";
        case EIDRM:
            return "Очередь сообщений уже удалена";
        case EINVAL:
            return "Неверный идентификатор очереди";
        case ENOENT:
            return "Очередь сообщений не существует";
        case EPERM:
            return "Недостаточно прав для удаления очереди";
        default:
            return strerror(error);
    }
}

int queueExists(int queueId) {
    struct msqid_ds info;
    if (msgctl(queueId, IPC_STAT, &info) == -1) {
        return 0;
    }
    return 1;
}

int createQueue() {
    key_t key;
    int msqid;

    if ((key = ftok(".", PROJECT_ID)) == -1) {
        fprintf(stderr, "Ошибка создания ключа: %s\n", strerror(errno));
        return -1;
    }

    if ((msqid = msgget(key, IPC_CREAT | 0666)) == -1) {
        fprintf(stderr, "Ошибка создания очереди: %s\n", strerror(errno));
        return -1;
    }

    return msqid;
}

int removeQueue(int queueId) {
    if (!queueExists(queueId)) {
        fprintf(stderr, "Ошибка: очередь с ID %d не существует или нет доступа\n", queueId);
        return -1;
    }

    if (msgctl(queueId, IPC_RMID, NULL) == -1) {
        fprintf(stderr, "Ошибка при удалении очереди: %s\n", getErrorDescription(errno));
        return -1;
    }

    printf("Очередь %d успешно удалена\n", queueId);
    return 0;
}

int main(int argc, char *argv[]) {
    int queueId;

    if (argc == 1) {
        queueId = createQueue();
        if (queueId == -1) {
            return 1;
        }
        printf("Создана новая очередь с ID: %d\n", queueId);
        printf("Чтобы удалить эту очередь, запустите программу с аргументом: %s %d\n",
               argv[0], queueId);
    }
    else if (argc == 2) {
        queueId = atoi(argv[1]);
        if (queueId <= 0) {
            fprintf(stderr, "Ошибка: некорректный идентификатор очереди\n");
            return 1;
        }
        if (removeQueue(queueId) == -1) {
            return 1;
        }
    }
    else {
        fprintf(stderr, "Использование:\n");
        fprintf(stderr, "Для создания очереди: %s\n", argv[0]);
        fprintf(stderr, "Для удаления очереди: %s <id_очереди>\n", argv[0]);
        return 1;
    }

    return 0;
}
